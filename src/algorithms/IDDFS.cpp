#include "IDDFS.hpp"
#include <chrono>
#include <vector>
#include <algorithm>

static bool dls(Graph& graph, NodeId u, NodeId target, int depth,
                const Node& targetNode,
                std::vector<int>& maxRemainingDepth, std::vector<NodeId>& parent,
                std::vector<VisitedStep>& visitedOrder, int& visitSeq,
                int& nodesVisited) {
    if (depth <= maxRemainingDepth[u]) return false;
    maxRemainingDepth[u] = depth;

    const Node& un = graph.nodeAt(u);
    if (depth < un.manhattanTo(targetNode)) return false;

    nodesVisited++;
    visitedOrder.push_back({un.row, un.col, visitSeq++, 0});

    if (u == target) return true;
    if (depth <= 0) return false;

    struct Cand {
        NodeId v;
        int h;
    };
    std::vector<Cand> cands;
    for (const Edge& e : graph.neighbors(u)) {
        const Node& vn = graph.nodeAt(e.to);
        cands.push_back({e.to, vn.manhattanTo(targetNode)});
    }
    std::sort(cands.begin(), cands.end(), [](const Cand& a, const Cand& b) {
        return a.h < b.h;
    });

    for (const auto& cand : cands) {
        NodeId v = cand.v;
        parent[v] = u;
        if (dls(graph, v, target, depth - 1, targetNode, maxRemainingDepth, parent, visitedOrder, visitSeq, nodesVisited)) {
            return true;
        }
    }

    return false;
}

AlgoResult IDDFS::run(Graph& graph, NodeId start, NodeId end, int maxDepth) {
    AlgoResult result;
    result.algorithm = "Iterative Deepening DFS (IDDFS)";

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

    const Node& targetNode = graph.nodeAt(end);
    int N = graph.numNodes();
    std::vector<NodeId> parent(N, INVALID_NODE);
    std::vector<int> maxRemainingDepth(N, -1);

    int visitSeq = 0;
    int nodesVisited = 0;
    bool found = false;

    int effectiveMaxDepth = std::min(maxDepth, graph.rows() * graph.cols());

    for (int limit = 1; limit <= effectiveMaxDepth; ++limit) {
        std::fill(parent.begin(), parent.end(), INVALID_NODE);
        std::fill(maxRemainingDepth.begin(), maxRemainingDepth.end(), -1);

        if (dls(graph, start, end, limit, targetNode, maxRemainingDepth, parent, result.visitedOrder, visitSeq, nodesVisited)) {
            found = true;
            break;
        }
    }

    auto t1 = std::chrono::high_resolution_clock::now();
    result.stats.timeMs = std::chrono::duration<double, std::milli>(t1 - t0).count();
    result.stats.nodesVisited = nodesVisited;
    result.stats.maxFrontier = effectiveMaxDepth;

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

