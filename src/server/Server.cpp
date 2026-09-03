#include "Server.hpp"
#include "vendor/httplib.h"
#include "vendor/json.hpp"

#include "graph/Graph.hpp"
#include "algorithms/BFS.hpp"
#include "algorithms/Dijkstra.hpp"
#include "algorithms/AStar.hpp"
#include "algorithms/BellmanFord.hpp"
#include "algorithms/BidirectionalBFS.hpp"
#include "algorithms/BidirectionalDijkstra.hpp"
#include "algorithms/BidirectionalAStar.hpp"
#include "algorithms/GreedyBestFirst.hpp"
#include "algorithms/DFS.hpp"
#include "algorithms/IDDFS.hpp"
#include "algorithms/Maze.hpp"

#include <iostream>
#include <vector>

using json = nlohmann::json;

Server::Server(int port, const std::string& staticDir)
    : port_(port), staticDir_(staticDir) {}

static AlgoResult runSingleAlgorithm(Graph& graph, NodeId startId, NodeId endId, const std::string& algo) {
    if (algo == "bfs") {
        return BFS::run(graph, startId, endId);
    } else if (algo == "dijkstra") {
        return Dijkstra::run(graph, startId, endId);
    } else if (algo == "astar" || algo == "astar-manhattan") {
        return AStar::run(graph, startId, endId, HeuristicType::Manhattan);
    } else if (algo == "astar-euclidean") {
        return AStar::run(graph, startId, endId, HeuristicType::Euclidean);
    } else if (algo == "bellman-ford") {
        return BellmanFord::run(graph, startId, endId);
    } else if (algo == "bidirectional-bfs" || algo == "bibfs") {
        return BidirectionalBFS::run(graph, startId, endId);
    } else if (algo == "bidirectional-dijkstra" || algo == "bidijkstra") {
        return BidirectionalDijkstra::run(graph, startId, endId);
    } else if (algo == "bidirectional-astar" || algo == "biastar" || algo == "bidirectional-astar-manhattan") {
        return BidirectionalAStar::run(graph, startId, endId, HeuristicType::Manhattan);
    } else if (algo == "bidirectional-astar-euclidean") {
        return BidirectionalAStar::run(graph, startId, endId, HeuristicType::Euclidean);
    } else if (algo == "greedy-best-first" || algo == "gbfs") {
        return GreedyBestFirst::run(graph, startId, endId, HeuristicType::Manhattan);
    } else if (algo == "gbfs-euclidean") {
        return GreedyBestFirst::run(graph, startId, endId, HeuristicType::Euclidean);
    } else if (algo == "dfs") {
        return DFS::run(graph, startId, endId);
    } else if (algo == "iddfs") {
        return IDDFS::run(graph, startId, endId);
    }
    throw std::runtime_error("Unknown algorithm: " + algo);
}

void Server::start() {
    httplib::Server svr;

    auto ret = svr.set_mount_point("/", staticDir_.c_str());
    if (!ret) {
        std::cerr << "[Warning] Could not mount static directory: " << staticDir_ << "\n";
    }

    svr.Get("/api/health", [](const httplib::Request&, httplib::Response& res) {
        json reply = {{"status", "ok"}, {"version", "2.0.0"}};
        res.set_content(reply.dump(), "application/json");
    });

    svr.Get("/api/presets", [](const httplib::Request&, httplib::Response& res) {
        json presets = json::array();

        {
            json p;
            p["id"] = "city-grid";
            p["name"] = "🏙️ Manhattan City Grid";
            p["description"] = "Orthogonal avenues and congested diagonal blocks with highways.";
            p["walls"] = json::array();
            p["weights"] = json::array();

            for (int r = 2; r < 18; r += 3) {
                for (int c = 2; c < 38; ++c) {
                    if (c % 4 != 0) p["walls"].push_back({r, c});
                }
            }

            for (int c = 0; c < 40; ++c) {
                p["weights"].push_back({{"row", 10}, {"col", c}, {"weight", 0.5}});
            }
            presets.push_back(p);
        }

        {
            json p;
            p["id"] = "castle-moat";
            p["name"] = "🏰 Castle Fortress & Moat";
            p["description"] = "A protected fortress surrounded by deep water and gate bridges.";
            p["walls"] = json::array();
            p["weights"] = json::array();

            for (int r = 3; r <= 16; ++r) {
                for (int c = 8; c <= 31; ++c) {
                    if (r == 3 || r == 16 || c == 8 || c == 31) {
                        if (c != 20 && r != 10) {
                            p["weights"].push_back({{"row", r}, {"col", c}, {"weight", 5.0}});
                        }
                    }
                }
            }

            for (int r = 6; r <= 13; ++r) {
                for (int c = 12; c <= 27; ++c) {
                    if (r == 6 || r == 13 || c == 12 || c == 27) {
                        if (c != 20) {
                            p["walls"].push_back({r, c});
                        }
                    }
                }
            }
            presets.push_back(p);
        }

        {
            json p;
            p["id"] = "slalom-barriers";
            p["name"] = "🎿 Slalom Zig-Zag Challenge";
            p["description"] = "Alternating obstacle pylons testing heuristic search divergence.";
            p["walls"] = json::array();
            for (int col = 6; col < 36; col += 6) {
                bool fromTop = ((col / 6) % 2 == 1);
                int rStart = fromTop ? 0 : 6;
                int rEnd = fromTop ? 14 : 20;
                for (int r = rStart; r < rEnd; ++r) {
                    p["walls"].push_back({r, col});
                }
            }
            presets.push_back(p);
        }

        {
            json p;
            p["id"] = "spiral-labyrinth";
            p["name"] = "🌀 Spiral Labyrinth";
            p["description"] = "A winding inward spiral testing heuristic back-tracking efficiency.";
            p["walls"] = json::array();
            p["weights"] = json::array();
            int top = 1, bottom = 18, left = 1, right = 38;
            while (top + 2 <= bottom && left + 2 <= right) {
                for (int c = left; c <= right; ++c) {
                    p["walls"].push_back({top, c});
                    p["walls"].push_back({bottom, c});
                }
                for (int r = top; r <= bottom; ++r) {
                    p["walls"].push_back({r, left});
                    p["walls"].push_back({r, right});
                }
                if (top + 1 < bottom) {
                    for (auto it = p["walls"].begin(); it != p["walls"].end(); ) {
                        if ((*it)[0] == top + 1 && (*it)[1] == left) {
                            it = p["walls"].erase(it);
                        } else {
                            ++it;
                        }
                    }
                }
                top += 2; bottom -= 2; left += 2; right -= 2;
            }
            presets.push_back(p);
        }

        {
            json p;
            p["id"] = "cave-dungeon";
            p["name"] = "⛰️ Cavern Dungeon";
            p["description"] = "Underground cavern halls and dense mud chambers.";
            p["walls"] = json::array();
            p["weights"] = json::array();
            for (int r = 2; r < 18; ++r) {
                for (int c = 2; c < 38; ++c) {
                    if ((r == 5 || r == 14) && (c < 12 || c > 26)) {
                        p["walls"].push_back({r, c});
                    }
                    if ((c == 14 || c == 24) && (r < 7 || r > 11)) {
                        p["walls"].push_back({r, c});
                    }
                    if (r >= 7 && r <= 11 && c >= 16 && c <= 22) {
                        p["weights"].push_back({{"row", r}, {"col", c}, {"weight", 3.0}});
                    }
                }
            }
            presets.push_back(p);
        }

        {
            json p;
            p["id"] = "weighted-valleys";
            p["name"] = "🏞️ Weighted River Valleys";
            p["description"] = "Layered expressways, sand dunes, and deep rivers.";
            p["walls"] = json::array();
            p["weights"] = json::array();
            for (int c = 0; c < 40; ++c) {
                p["weights"].push_back({{"row", 4}, {"col", c}, {"weight", 0.5}});
                p["weights"].push_back({{"row", 8}, {"col", c}, {"weight", 2.0}});
                p["weights"].push_back({{"row", 12}, {"col", c}, {"weight", 5.0}});
                p["weights"].push_back({{"row", 16}, {"col", c}, {"weight", 3.0}});
            }
            for (int r = 0; r < 20; ++r) {
                if (r != 4 && r != 12) {
                    p["walls"].push_back({r, 10});
                    p["walls"].push_back({r, 28});
                }
            }
            presets.push_back(p);
        }

        res.set_content(json({{"presets", presets}}).dump(), "application/json");
    });

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
                                double wt = item["weight"].is_number() ? item["weight"].get<double>() : 1.0;
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
                            cells[r][c].weight = cell.value("weight", 1.0);
                        }
                    }
                    graph.loadFromGrid(cells);
                }
            }

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

            if (startRow < 0 || startRow >= rows || startCol < 0 || startCol >= cols) {
                res.status = 400;
                res.set_content(json({{"error", "start coordinates out of bounds"}}).dump(), "application/json");
                return;
            }
            if (endRow < 0 || endRow >= rows || endCol < 0 || endCol >= cols) {
                res.status = 400;
                res.set_content(json({{"error", "end coordinates out of bounds"}}).dump(), "application/json");
                return;
            }
            if (startRow == endRow && startCol == endCol) {
                res.status = 400;
                res.set_content(json({{"error", "start and end cannot be the same"}}).dump(), "application/json");
                return;
            }
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

            std::string algo = body.value("algorithm", "dijkstra");

            std::vector<std::pair<int, int>> legPoints;
            legPoints.push_back({startRow, startCol});

            if (body.contains("waypoints") && body["waypoints"].is_array()) {
                for (const auto& wp : body["waypoints"]) {
                    int wr = -1, wc = -1;
                    if (parseCoord(wp, wr, wc)) {
                        if (wr >= 0 && wr < rows && wc >= 0 && wc < cols && !graph.nodeAt(wr, wc).isWall) {
                            legPoints.push_back({wr, wc});
                        }
                    }
                }
            }
            legPoints.push_back({endRow, endCol});

            AlgoResult combinedResult;
            combinedResult.algorithm = algo;
            combinedResult.pathFound = true;

            int totalVisited = 0;
            double totalTime = 0.0;
            double totalCost = 0.0;

            for (size_t leg = 0; leg + 1 < legPoints.size(); ++leg) {
                NodeId legStart = graph.idOf(legPoints[leg].first, legPoints[leg].second);
                NodeId legEnd   = graph.idOf(legPoints[leg + 1].first, legPoints[leg + 1].second);

                AlgoResult legRes = runSingleAlgorithm(graph, legStart, legEnd, algo);
                if (!legRes.error.empty()) {
                    res.status = 400;
                    res.set_content(json({{"error", legRes.error}}).dump(), "application/json");
                    return;
                }
                totalVisited += legRes.stats.nodesVisited;
                totalTime += legRes.stats.timeMs;

                if (!legRes.pathFound) {
                    combinedResult.pathFound = false;
                    combinedResult.visitedOrder.insert(combinedResult.visitedOrder.end(),
                                                       legRes.visitedOrder.begin(), legRes.visitedOrder.end());
                    break;
                }

                combinedResult.visitedOrder.insert(combinedResult.visitedOrder.end(),
                                                   legRes.visitedOrder.begin(), legRes.visitedOrder.end());

                if (combinedResult.path.empty()) {
                    combinedResult.path = legRes.path;
                } else {
                    for (size_t i = 1; i < legRes.path.size(); ++i) {
                        combinedResult.path.push_back(legRes.path[i]);
                    }
                }
                totalCost += legRes.stats.pathCost;
            }

            combinedResult.stats.nodesVisited = totalVisited;
            combinedResult.stats.timeMs = totalTime;
            combinedResult.stats.pathCost = totalCost;
            combinedResult.stats.pathLength = static_cast<int>(combinedResult.path.size());

            json resJson;
            json pathArr = json::array();
            for (const auto& step : combinedResult.path) {
                pathArr.push_back({step.row, step.col});
            }
            resJson["path"] = pathArr;

            json visitedArr = json::array();
            for (const auto& step : combinedResult.visitedOrder) {
                visitedArr.push_back({
                    {"row", step.row},
                    {"col", step.col},
                    {"order", step.order},
                    {"frontier", step.frontier}
                });
            }
            resJson["visitedOrder"] = visitedArr;
            resJson["noPathFound"] = !combinedResult.pathFound;
            resJson["algorithm"] = combinedResult.algorithm;

            resJson["stats"] = {
                {"nodesVisited", combinedResult.stats.nodesVisited},
                {"pathLength", combinedResult.pathFound ? combinedResult.stats.pathLength : 0},
                {"pathCost", combinedResult.pathFound ? combinedResult.stats.pathCost : 0.0},
                {"executionTimeMs", combinedResult.stats.timeMs}
            };

            res.status = 200;
            res.set_content(resJson.dump(), "application/json");

        } catch (const std::exception& ex) {
            res.status = 400;
            res.set_content(json({{"error", ex.what()}}).dump(), "application/json");
        }
    });

    svr.Post("/api/generate-maze", [](const httplib::Request& req, httplib::Response& res) {
        try {
            auto body = json::parse(req.body);
            int rows = body.at("rows").get<int>();
            int cols = body.at("cols").get<int>();
            std::string algo = body.value("algorithm", "dfs");

            std::vector<std::vector<Graph::CellDesc>> grid;
            if (algo == "prims") {
                grid = MazeGenerator::generatePrims(rows, cols);
            } else if (algo == "recursive-division") {
                grid = MazeGenerator::generateRecursiveDivision(rows, cols);
            } else if (algo == "cellular-automata" || algo == "caves") {
                grid = MazeGenerator::generateCellularAutomata(rows, cols);
            } else if (algo == "spiral") {
                grid = MazeGenerator::generateSpiral(rows, cols);
            } else if (algo == "terrain-noise" || algo == "noise") {
                grid = MazeGenerator::generateTerrainNoise(rows, cols);
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
