#include "BidirectionalAStar.hpp"
#include <queue>
#include <chrono>
#include <vector>
#include <algorithm>
#include <cmath>

struct AStarEntry {
    NodeId id;
    double f;
    double g;

    bool operator>(const AStarEntry& other) const {
        return f > other.f;
    }
};

static double calcH(const Node& a, const Node& b, HeuristicType hType) {
    if (hType == HeuristicType::Euclidean) {
        return a.euclideanTo(b);
    }
    return static_cast<double>(a.manhattanTo(b));
}

AlgoResult BidirectionalAStar::run(Graph& graph, NodeId start, NodeId end,
                                   HeuristicType heuristic) {
    AlgoResult result;
    result.algorithm = (heuristic == HeuristicType::Euclidean)
                       ? "Bidirectional A* (Euclidean)"
                       : "Bidirectional A* (Manhattan)";

    graph.resetState();
    auto t0 = std::chrono::high_resolution_clock::now();

    if (start == end) {
        result.pathFound = true;
        const Node& s = graph.nodeAt(start);
        result.path.push_back({s.row, s.col});
        result.stats.nodesVisited = 1;
        result.stats.pathLength = 1;
        result.stats.pathCost = 0.0;
        return result;
    }

    const Node& sNode = graph.nodeAt(start);
    const Node& eNode = graph.nodeAt(end);

    int N = graph.numNodes();
    std::vector<double> gF(N, INF);
    std::vector<double> gB(N, INF);
    std::vector<bool> closedF(N, false);
    std::vector<bool> closedB(N, false);
    std::vector<NodeId> parentF(N, INVALID_NODE);
    std::vector<NodeId> parentB(N, INVALID_NODE);

    std::priority_queue<AStarEntry, std::vector<AStarEntry>, std::greater<AStarEntry>> pqF;
    std::priority_queue<AStarEntry, std::vector<AStarEntry>, std::greater<AStarEntry>> pqB;

    gF[start] = 0.0;
    pqF.push({start, calcH(sNode, eNode, heuristic), 0.0});

    gB[end] = 0.0;
    pqB.push({end, calcH(eNode, sNode, heuristic), 0.0});

    int visitSeq = 0;
    double mu = INF;
    NodeId bestMeet = INVALID_NODE;
    size_t peakFrontier = 0;

    while (!pqF.empty() && !pqB.empty()) {
        peakFrontier = std::max(peakFrontier, pqF.size() + pqB.size());

        if (pqF.top().f >= mu && pqB.top().f >= mu) {
            break;
        }

        if (pqF.top().f <= pqB.top().f) {
            auto topF = pqF.top();
            pqF.pop();
            NodeId u = topF.id;

            if (closedF[u]) continue;
            closedF[u] = true;
            result.stats.nodesVisited++;

            const Node& un = graph.nodeAt(u);
            result.visitedOrder.push_back({un.row, un.col, visitSeq++, 0});

            if (closedB[u] || gB[u] < INF) {
                if (gF[u] + gB[u] < mu) {
                    mu = gF[u] + gB[u];
                    bestMeet = u;
                }
            }

            for (const Edge& e : graph.neighbors(u)) {
                NodeId v = e.to;
                if (closedF[v]) continue;

                double tentativeG = gF[u] + e.weight;
                if (tentativeG < gF[v]) {
                    gF[v] = tentativeG;
                    parentF[v] = u;
                    const Node& vn = graph.nodeAt(v);
                    double fScore = tentativeG + calcH(vn, eNode, heuristic);
                    pqF.push({v, fScore, tentativeG});

                    if (gB[v] < INF && tentativeG + gB[v] < mu) {
                        mu = tentativeG + gB[v];
                        bestMeet = v;
                    }
                }
            }
        } else {
            auto topB = pqB.top();
            pqB.pop();
            NodeId u = topB.id;

            if (closedB[u]) continue;
            closedB[u] = true;
            result.stats.nodesVisited++;

            const Node& un = graph.nodeAt(u);
            result.visitedOrder.push_back({un.row, un.col, visitSeq++, 1});

            if (closedF[u] || gF[u] < INF) {
                if (gF[u] + gB[u] < mu) {
                    mu = gF[u] + gB[u];
                    bestMeet = u;
                }
            }

            for (const Edge& e : graph.neighbors(u)) {
                NodeId v = e.to;
                if (closedB[v]) continue;

                double tentativeG = gB[u] + e.weight;
                if (tentativeG < gB[v]) {
                    gB[v] = tentativeG;
                    parentB[v] = u;
                    const Node& vn = graph.nodeAt(v);
                    double fScore = tentativeG + calcH(vn, sNode, heuristic);
                    pqB.push({v, fScore, tentativeG});

                    if (gF[v] < INF && gF[v] + tentativeG < mu) {
                        mu = gF[v] + tentativeG;
                        bestMeet = v;
                    }
                }
            }
        }
    }

    auto t1 = std::chrono::high_resolution_clock::now();
    result.stats.timeMs = std::chrono::duration<double, std::milli>(t1 - t0).count();
    result.stats.maxFrontier = static_cast<int>(peakFrontier);

    if (bestMeet != INVALID_NODE && mu < INF) {
        result.pathFound = true;

        std::vector<PathStep> forwardPath;
        NodeId cur = bestMeet;
        while (cur != INVALID_NODE) {
            const Node& n = graph.nodeAt(cur);
            forwardPath.push_back({n.row, n.col});
            if (cur == start) break;
            cur = parentF[cur];
        }
        std::reverse(forwardPath.begin(), forwardPath.end());

        std::vector<PathStep> backwardPath;
        cur = parentB[bestMeet];
        while (cur != INVALID_NODE) {
            const Node& n = graph.nodeAt(cur);
            backwardPath.push_back({n.row, n.col});
            if (cur == end) break;
            cur = parentB[cur];
        }

        result.path = std::move(forwardPath);
        for (const auto& step : backwardPath) {
            result.path.push_back(step);
        }

        result.stats.pathLength = static_cast<int>(result.path.size());
        double totalCost = 0.0;
        for (size_t i = 1; i < result.path.size(); ++i) {
            const Node& n = graph.nodeAt(result.path[i].row, result.path[i].col);
            totalCost += n.weight;
        }
        result.stats.pathCost = totalCost;
    }

    return result;
}
