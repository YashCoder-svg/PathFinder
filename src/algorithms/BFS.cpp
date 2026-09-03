#include "BFS.hpp"
#include <queue>
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

AlgoResult BFS::run(Graph& graph, NodeId start, NodeId end) {
    AlgoResult result;
    result.algorithm = "BFS";

    graph.resetState();

    auto t0 = std::chrono::high_resolution_clock::now();

    std::queue<NodeId> q;

    Node& startNode  = graph.nodeAt(start);
    startNode.visited = true;
    startNode.dist    = 0.0;
    startNode.parent  = INVALID_NODE;
    q.push(start);

    int visitSeq = 0;
    result.visitedOrder.push_back({startNode.row, startNode.col, visitSeq++});

    bool found = false;

    while (!q.empty()) {
        NodeId uid = q.front();
        q.pop();

        result.stats.nodesVisited++;

        if (uid == end) {
            found = true;
            break;
        }

        for (const Edge& e : graph.neighbors(uid)) {
            NodeId vid = e.to;
            Node&  v   = graph.nodeAt(vid);

            if (!v.visited) {
                v.visited = true;
                v.parent  = uid;
                v.dist    = graph.nodeAt(uid).dist + 1.0;
                q.push(vid);
                result.visitedOrder.push_back({v.row, v.col, visitSeq++});
            }
        }
    }

    auto t1 = std::chrono::high_resolution_clock::now();
    result.stats.timeMs = std::chrono::duration<double, std::milli>(t1 - t0).count();

    if (found) {
        result.pathFound       = true;
        result.path            = reconstructPath(graph, start, end);
        result.stats.pathLength = static_cast<int>(result.path.size());

        result.stats.pathCost  = graph.nodeAt(end).dist;
    }

    return result;
}
