#include "BidirectionalDijkstra.hpp"
#include <queue>
#include <chrono>
#include <vector>
#include <algorithm>

struct PQEntry {
    NodeId id;
    double dist;

    bool operator>(const PQEntry& other) const {
        return dist > other.dist;
    }
};

AlgoResult BidirectionalDijkstra::run(Graph& graph, NodeId start, NodeId end) {
    AlgoResult result;
    result.algorithm = "Bidirectional Dijkstra";

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

    int N = graph.numNodes();
    std::vector<double> distF(N, INF);
    std::vector<double> distB(N, INF);
    std::vector<bool> visitedF(N, false);
    std::vector<bool> visitedB(N, false);
    std::vector<NodeId> parentF(N, INVALID_NODE);
    std::vector<NodeId> parentB(N, INVALID_NODE);

    std::priority_queue<PQEntry, std::vector<PQEntry>, std::greater<PQEntry>> pqF;
    std::priority_queue<PQEntry, std::vector<PQEntry>, std::greater<PQEntry>> pqB;

    distF[start] = 0.0;
    pqF.push({start, 0.0});

    distB[end] = 0.0;
    pqB.push({end, 0.0});

    int visitSeq = 0;
    double mu = INF;
    NodeId bestMeet = INVALID_NODE;
    size_t peakFrontier = 0;

    while (!pqF.empty() && !pqB.empty()) {
        peakFrontier = std::max(peakFrontier, pqF.size() + pqB.size());

        if (pqF.top().dist + pqB.top().dist >= mu) {
            break;
        }

        if (pqF.top().dist <= pqB.top().dist) {
            auto topF = pqF.top();
            pqF.pop();
            NodeId u = topF.id;

            if (visitedF[u]) continue;
            visitedF[u] = true;
            result.stats.nodesVisited++;

            const Node& un = graph.nodeAt(u);
            result.visitedOrder.push_back({un.row, un.col, visitSeq++, 0});

            for (const Edge& e : graph.neighbors(u)) {
                NodeId v = e.to;
                double newDist = distF[u] + e.weight;
                if (newDist < distF[v]) {
                    distF[v] = newDist;
                    parentF[v] = u;
                    pqF.push({v, newDist});
                }
                if (visitedB[v] && distF[u] + e.weight + distB[v] < mu) {
                    mu = distF[u] + e.weight + distB[v];
                    bestMeet = v;
                    parentF[v] = u;
                }
            }
        } else {
            auto topB = pqB.top();
            pqB.pop();
            NodeId u = topB.id;

            if (visitedB[u]) continue;
            visitedB[u] = true;
            result.stats.nodesVisited++;

            const Node& un = graph.nodeAt(u);
            result.visitedOrder.push_back({un.row, un.col, visitSeq++, 1});

            for (const Edge& e : graph.neighbors(u)) {
                NodeId v = e.to;

                double newDist = distB[u] + e.weight;
                if (newDist < distB[v]) {
                    distB[v] = newDist;
                    parentB[v] = u;
                    pqB.push({v, newDist});
                }
                if (visitedF[v] && distB[u] + e.weight + distF[v] < mu) {
                    mu = distB[u] + e.weight + distF[v];
                    bestMeet = v;
                    parentB[v] = u;
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
