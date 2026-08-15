// =============================================================================
// Server.cpp — Web Server implementation with REST endpoints
// =============================================================================

#include "Server.hpp"
#include "vendor/httplib.h"
#include "vendor/json.hpp"

#include "graph/Graph.hpp"
#include "algorithms/BFS.hpp"
#include "algorithms/Dijkstra.hpp"
#include "algorithms/AStar.hpp"
#include "algorithms/BellmanFord.hpp"
#include "algorithms/Maze.hpp"

#include <iostream>

using json = nlohmann::json;

Server::Server(int port, const std::string& staticDir)
    : port_(port), staticDir_(staticDir) {}

void Server::start() {
    httplib::Server svr;

    // Serve static files from staticDir_
    auto ret = svr.set_mount_point("/", staticDir_.c_str());
    if (!ret) {
        std::cerr << "[Warning] Could not mount static directory: " << staticDir_ << "\n";
    }

    // Health check endpoint
    svr.Get("/api/health", [](const httplib::Request&, httplib::Response& res) {
        json reply = {{"status", "ok"}};
        res.set_content(reply.dump(), "application/json");
    });

    // Solve endpoint
    svr.Post("/api/solve", [](const httplib::Request& req, httplib::Response& res) {
        try {
            json body;
            try {
                body = json::parse(req.body);
            } catch (const std::exception& ex) {
                res.status = 400;
                res.set_content(json({{"error", "Invalid JSON payload"}}).dump(), "application/json");
                return;
            }

            int rows = 0;
            int cols = 0;

            if (body.contains("grid") && body["grid"].is_object()) {
                cols = body["grid"].value("width", 0);
                rows = body["grid"].value("height", 0);
            }
            if (rows == 0 && body.contains("rows")) rows = body["rows"].get<int>();
            if (cols == 0 && body.contains("cols")) cols = body["cols"].get<int>();

            if (rows <= 0 || cols <= 0) {
                res.status = 400;
                res.set_content(json({{"error", "Invalid grid dimensions"}}).dump(), "application/json");
                return;
            }

            std::string modeStr = body.value("mode", "4way");
            ConnectivityMode connMode = (modeStr == "8way") ? ConnectivityMode::EightWay : ConnectivityMode::FourWay;

            Graph graph(rows, cols, connMode);

            // Load grid representation (supports grid object with walls array or legacy 2D matrix)
            if (body.contains("grid")) {
                const auto& gridJson = body["grid"];
                if (gridJson.is_object()) {
                    if (gridJson.contains("walls") && gridJson["walls"].is_array()) {
                        for (const auto& w : gridJson["walls"]) {
                            if (w.is_array() && w.size() >= 2) {
                                int wr = w[0].get<int>();
                                int wc = w[1].get<int>();
                                if (wr >= 0 && wr < rows && wc >= 0 && wc < cols) {
                                    graph.setWall(wr, wc, true);
                                }
                            }
                        }
                    }
                    if (gridJson.contains("weights") && gridJson["weights"].is_array()) {
                        for (const auto& item : gridJson["weights"]) {
                            if (item.is_object() && item.contains("row") && item.contains("col") && item.contains("weight")) {
                                int wr = item["row"].get<int>();
                                int wc = item["col"].get<int>();
                                int wt = item["weight"].get<int>();
                                if (wr >= 0 && wr < rows && wc >= 0 && wc < cols) {
                                    graph.setWeight(wr, wc, wt);
                                }
                            }
                        }
                    }
                } else if (gridJson.is_array()) {
                    std::vector<std::vector<Graph::CellDesc>> cells(rows, std::vector<Graph::CellDesc>(cols));
                    for (int r = 0; r < rows; ++r) {
                        for (int c = 0; c < cols; ++c) {
                            auto cell = gridJson[r][c];
                            cells[r][c].isWall = cell.value("isWall", false);
                            cells[r][c].weight = cell.value("weight", 1);
                        }
                    }
                    graph.loadFromGrid(cells);
                }
            }

            // Extract start and end coordinates
            auto parseCoord = [](const json& j, int& r, int& c) -> bool {
                if (j.is_array() && j.size() >= 2) {
                    r = j[0].get<int>();
                    c = j[1].get<int>();
                    return true;
                } else if (j.is_object()) {
                    if (j.contains("row") && j.contains("col")) {
                        r = j["row"].get<int>();
                        c = j["col"].get<int>();
                        return true;
                    }
                }
                return false;
            };

            int startRow = -1, startCol = -1;
            if (!body.contains("start") || !parseCoord(body["start"], startRow, startCol)) {
                res.status = 400;
                res.set_content(json({{"error", "Missing or invalid start coordinates"}}).dump(), "application/json");
                return;
            }

            int endRow = -1, endCol = -1;
            if (!body.contains("end") || !parseCoord(body["end"], endRow, endCol)) {
                res.status = 400;
                res.set_content(json({{"error", "Missing or invalid end coordinates"}}).dump(), "application/json");
                return;
            }

            // Validation 1: Bounds check for start
            if (startRow < 0 || startRow >= rows || startCol < 0 || startCol >= cols) {
                res.status = 400;
                res.set_content(json({{"error", "start coordinates out of bounds"}}).dump(), "application/json");
                return;
            }

            // Validation 2: Bounds check for end
            if (endRow < 0 || endRow >= rows || endCol < 0 || endCol >= cols) {
                res.status = 400;
                res.set_content(json({{"error", "end coordinates out of bounds"}}).dump(), "application/json");
                return;
            }

            // Validation 3: start == end check
            if (startRow == endRow && startCol == endCol) {
                res.status = 400;
                res.set_content(json({{"error", "start and end cannot be the same"}}).dump(), "application/json");
                return;
            }

            // Validation 4: Wall collision check
            if (graph.nodeAt(startRow, startCol).isWall) {
                res.status = 400;
                res.set_content(json({{"error", "start position is blocked by a wall"}}).dump(), "application/json");
                return;
            }
            if (graph.nodeAt(endRow, endCol).isWall) {
                res.status = 400;
                res.set_content(json({{"error", "end position is blocked by a wall"}}).dump(), "application/json");
                return;
            }

            NodeId startId = graph.idOf(startRow, startCol);
            NodeId endId = graph.idOf(endRow, endCol);

            std::string algo = body.value("algorithm", "dijkstra");

            AlgoResult result;
            if (algo == "bfs") {
                result = BFS::run(graph, startId, endId);
            } else if (algo == "dijkstra") {
                result = Dijkstra::run(graph, startId, endId);
            } else if (algo == "astar" || algo == "astar-manhattan") {
                result = AStar::run(graph, startId, endId, HeuristicType::Manhattan);
            } else if (algo == "astar-euclidean") {
                result = AStar::run(graph, startId, endId, HeuristicType::Euclidean);
            } else if (algo == "bellman-ford") {
                result = BellmanFord::run(graph, startId, endId);
            } else {
                res.status = 400;
                res.set_content(json({{"error", "Unknown algorithm: " + algo}}).dump(), "application/json");
                return;
            }

            // Serialize AlgoResult according to explicit API Contract
            json resJson;
            json pathArr = json::array();
            for (const auto& step : result.path) {
                pathArr.push_back({step.row, step.col});
            }
            resJson["path"] = pathArr;

            json visitedArr = json::array();
            for (const auto& step : result.visitedOrder) {
                visitedArr.push_back({step.row, step.col});
            }
            resJson["visitedOrder"] = visitedArr;

            bool noPath = !result.pathFound;
            resJson["noPathFound"] = noPath;

            resJson["stats"] = {
                {"nodesVisited", result.stats.nodesVisited},
                {"pathLength", result.pathFound ? result.stats.pathLength : 0},
                {"executionTimeMs", result.stats.timeMs}
            };

            res.status = 200;
            res.set_content(resJson.dump(), "application/json");

        } catch (const std::exception& ex) {
            res.status = 400;
            res.set_content(json({{"error", ex.what()}}).dump(), "application/json");
        }
    });

    // Maze endpoint
    svr.Post("/api/generate-maze", [](const httplib::Request& req, httplib::Response& res) {
        try {
            auto body = json::parse(req.body);
            int rows = body.at("rows").get<int>();
            int cols = body.at("cols").get<int>();
            std::string algo = body.value("algorithm", "dfs");

            std::vector<std::vector<Graph::CellDesc>> grid;
            if (algo == "prims") {
                grid = MazeGenerator::generatePrims(rows, cols);
            } else {
                grid = MazeGenerator::generateDFS(rows, cols);
            }

            json gridJson = json::array();
            for (int r = 0; r < rows; ++r) {
                json rowJson = json::array();
                for (int c = 0; c < cols; ++c) {
                    rowJson.push_back({
                        {"isWall", grid[r][c].isWall},
                        {"weight", grid[r][c].weight}
                    });
                }
                gridJson.push_back(rowJson);
            }

            res.set_content(json({{"grid", gridJson}}).dump(), "application/json");

        } catch (const std::exception& ex) {
            res.status = 400;
            res.set_content(json({{"error", ex.what()}}).dump(), "application/json");
        }
    });

    std::cout << "🚀 PathFinder Pro Server listening at http://localhost:" << port_ << "\n";
    svr.listen("0.0.0.0", port_);
}
