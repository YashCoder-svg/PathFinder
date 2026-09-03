#include "GreedyBestFirst.hpp"
#include <queue>
#include <chrono>
#include <vector>
#include <algorithm>

struct GBFSEntry {
    NodeId id;
    double h;

    bool operator>(const GBFSEntry& other) const {
        return h > other.h;
    }
};

static double calcH(const Node& a, const Node& b, HeuristicType hType) {
    if (hType == HeuristicType::Euclidean) {
        return a.euclideanTo(b);
    }
    return static_cast<double>(a.manhattanTo(b));
}

AlgoResult GreedyBestFirst::run(Graph& graph, NodeId start, NodeId end,
                                HeuristicType heuristic) {
    AlgoResult result;
    result.algorithm = "Greedy Best-First Search";

    graph.resetState();
    auto t0 = std::chrono::high_resolution_clock::now();

    const Node& sNode = graph.nodeAt(start);
    const Node& eNode = graph.nodeAt(end);

    int N = graph.numNodes();
    std::vector<bool> visited(N, false);
    std::vector<NodeId> parent(N, INVALID_NODE);

    std::priority_queue<GBFSEntry, std::vector<GBFSEntry>, std::greater<GBFSEntry>> pq;

    pq.push({start, calcH(sNode, eNode, heuristic)});
    visited[start] = true;

    int visitSeq = 0;
    result.visitedOrder.push_back({sNode.row, sNode.col, visitSeq++, 0});

    bool found = false;
    size_t peakFrontier = 1;

    while (!pq.empty()) {
        peakFrontier = std::max(peakFrontier, pq.size());
        auto top = pq.top();
        pq.pop();
        NodeId u = top.id;

        result.stats.nodesVisited++;

        if (u == end) {
            found = true;
            break;
        }

        for (const Edge& e : graph.neighbors(u)) {
            NodeId v = e.to;
            if (!visited[v]) {
                visited[v] = true;
                parent[v] = u;
                const Node& vn = graph.nodeAt(v);
                pq.push({v, calcH(vn, eNode, heuristic)});
                result.visitedOrder.push_back({vn.row, vn.col, visitSeq++, 0});
            }
        }
    }

    auto t1 = std::chrono::high_resolution_clock::now();
    result.stats.timeMs = std::chrono::duration<double, std::milli>(t1 - t0).count();
    result.stats.maxFrontier = static_cast<int>(peakFrontier);

    if (found) {
        result.pathFound = true;
        std::vector<PathStep> path;
        NodeId cur = end;
        while (cur != INVALID_NODE) {
            const Node& n = graph.nodeAt(cur);
            path.push_back({n.row, n.col});
            if (cur == start) break;
            cur = parent[cur];
        }
        std::reverse(path.begin(), path.end());
        result.path = std::move(path);
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
