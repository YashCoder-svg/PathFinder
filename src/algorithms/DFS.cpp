#include "DFS.hpp"
#include <stack>
#include <chrono>
#include <vector>
#include <algorithm>

AlgoResult DFS::run(Graph& graph, NodeId start, NodeId end) {
    AlgoResult result;
    result.algorithm = "Depth-First Search (DFS)";

    graph.resetState();
    auto t0 = std::chrono::high_resolution_clock::now();

    int N = graph.numNodes();
    std::vector<bool> visited(N, false);
    std::vector<NodeId> parent(N, INVALID_NODE);

    std::stack<NodeId> st;
    st.push(start);
    visited[start] = true;

    int visitSeq = 0;
    const Node& sNode = graph.nodeAt(start);
    result.visitedOrder.push_back({sNode.row, sNode.col, visitSeq++, 0});

    bool found = false;
    size_t peakFrontier = 1;

    while (!st.empty()) {
        peakFrontier = std::max(peakFrontier, st.size());
        NodeId u = st.top();
        st.pop();

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
                st.push(v);
                const Node& vn = graph.nodeAt(v);
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
