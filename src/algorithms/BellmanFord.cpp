#include "BellmanFord.hpp"
#include <chrono>
#include <vector>
#include <algorithm>

static std::vector<PathStep> reconstructPath(const Graph& graph, NodeId start, NodeId end) {
    std::vector<PathStep> path;
    NodeId cur = end;
    while (cur != INVALID_NODE && cur != start) {
        const Node& n = graph.nodeAt(cur);
        path.push_back({n.row, n.col});
        cur = n.parent;
    }
    if (cur == start) {
        const Node& s = graph.nodeAt(start);
        path.push_back({s.row, s.col});
    }
    std::reverse(path.begin(), path.end());
    return path;
}

AlgoResult BellmanFord::run(Graph& graph, NodeId start, NodeId end) {
    AlgoResult result;
    result.algorithm = "Bellman-Ford";

    graph.resetState();

    auto t0 = std::chrono::high_resolution_clock::now();

    int V = graph.numNodes();
    Node& src = graph.nodeAt(start);
    src.dist = 0.0;
    src.parent = INVALID_NODE;
    src.visited = true;

    int visitSeq = 0;
    result.visitedOrder.push_back({src.row, src.col, visitSeq++});
    result.stats.nodesVisited = 1;

    struct ExtEdge {
        NodeId u;
        NodeId v;
        double w;
    };
    std::vector<ExtEdge> allEdges;
    allEdges.reserve(V * 4);

    for (NodeId u = 0; u < V; ++u) {
        for (const Edge& e : graph.neighbors(u)) {
            allEdges.push_back({u, e.to, e.weight});
        }
    }

    for (int i = 0; i < V - 1; ++i) {
        bool updated = false;
        for (const auto& edge : allEdges) {
            const Node& u = graph.nodeAt(edge.u);
            if (u.dist == INF) continue;

            Node& v = graph.nodeAt(edge.v);
            if (u.dist + edge.w < v.dist) {
                v.dist = u.dist + edge.w;
                v.parent = edge.u;
                updated = true;

                if (!v.visited) {
                    v.visited = true;
                    result.stats.nodesVisited++;
                    result.visitedOrder.push_back({v.row, v.col, visitSeq++});
                }
            }
        }
        if (!updated) break;
    }

    bool hasNegativeCycle = false;
    for (const auto& edge : allEdges) {
        const Node& u = graph.nodeAt(edge.u);
        if (u.dist == INF) continue;

        const Node& v = graph.nodeAt(edge.v);
        if (u.dist + edge.w < v.dist) {
            hasNegativeCycle = true;
            break;
        }
    }

    auto t1 = std::chrono::high_resolution_clock::now();
    result.stats.timeMs = std::chrono::duration<double, std::milli>(t1 - t0).count();

    if (hasNegativeCycle) {
        result.pathFound = false;
        result.error = "Negative weight cycle detected!";
        return result;
    }

    const Node& endNode = graph.nodeAt(end);
    if (endNode.dist != INF) {
        result.pathFound = true;
        result.path = reconstructPath(graph, start, end);
        result.stats.pathLength = static_cast<int>(result.path.size());
        result.stats.pathCost = endNode.dist;
    }

    return result;
}
