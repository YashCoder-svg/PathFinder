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
            auto body = json::parse(req.body);

            int rows = body.at("rows").get<int>();
            int cols = body.at("cols").get<int>();
            std::string modeStr = body.value("mode", "4way");
            ConnectivityMode connMode = (modeStr == "8way") ? ConnectivityMode::EightWay : ConnectivityMode::FourWay;

            Graph graph(rows, cols, connMode);

            // Load cell descriptors if present
            if (body.contains("grid")) {
                std::vector<std::vector<Graph::CellDesc>> cells(rows, std::vector<Graph::CellDesc>(cols));
                auto gridJson = body.at("grid");
                for (int r = 0; r < rows; ++r) {
                    for (int c = 0; c < cols; ++c) {
                        auto cell = gridJson[r][c];
                        cells[r][c].isWall = cell.value("isWall", false);
                        cells[r][c].weight = cell.value("weight", 1);
                    }
                }
                graph.loadFromGrid(cells);
            }

            int startRow = body.at("start").at("row").get<int>();
            int startCol = body.at("start").at("col").get<int>();
            int endRow = body.at("end").at("row").get<int>();
            int endCol = body.at("end").at("col").get<int>();

            NodeId startId = graph.idOf(startRow, startCol);
            NodeId endId = graph.idOf(endRow, endCol);

            std::string algo = body.at("algorithm").get<std::string>();

            AlgoResult result;
            if (algo == "bfs") {
                result = BFS::run(graph, startId, endId);
            } else if (algo == "dijkstra") {
                result = Dijkstra::run(graph, startId, endId);
            } else if (algo == "astar-manhattan") {
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

            // Serialize AlgoResult
            json resJson;
            resJson["algorithm"] = result.algorithm;
            resJson["pathFound"] = result.pathFound;
            resJson["error"] = result.error;

            json pathArr = json::array();
            for (const auto& step : result.path) {
                pathArr.push_back({{"row", step.row}, {"col", step.col}});
            }
            resJson["path"] = pathArr;

            json visitedArr = json::array();
            for (const auto& step : result.visitedOrder) {
                visitedArr.push_back({{"row", step.row}, {"col", step.col}, {"order", step.order}});
            }
            resJson["visitedOrder"] = visitedArr;

            resJson["stats"] = {
                {"nodesVisited", result.stats.nodesVisited},
                {"pathLength", result.stats.pathLength},
                {"pathCost", result.stats.pathCost},
                {"timeMs", result.stats.timeMs}
            };

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
