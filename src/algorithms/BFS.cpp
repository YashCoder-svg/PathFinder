// =============================================================================
// BFS.cpp — Breadth-First Search implementation
// =============================================================================

#include "BFS.hpp"
#include <queue>
#include <chrono>
#include <vector>
#include <algorithm>  // std::reverse

// ---------------------------------------------------------------------------
// Helper: reconstruct path by following parent pointers end → start,
// then reversing so the result is start → end.
// ---------------------------------------------------------------------------
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

// =============================================================================
// BFS::run
// =============================================================================
AlgoResult BFS::run(Graph& graph, NodeId start, NodeId end) {
    AlgoResult result;
    result.algorithm = "BFS";

    // Reset per-run state so stale dist/visited/parent don't interfere
    graph.resetState();

    auto t0 = std::chrono::high_resolution_clock::now();

    // -------------------------------------------------------------------------
    // ALGORITHM:
    //   1. Enqueue the start node; mark it visited (dist = 0 for BFS).
    //   2. While the queue is non-empty:
    //        a. Dequeue the front node u.
    //        b. If u == end, we're done.
    //        c. For each neighbour v of u (from the adjacency list):
    //             If v hasn't been visited, mark it (set parent = u) and enqueue.
    //
    // KEY INSIGHT: Because a FIFO queue expands nodes in the order they were
    // discovered, every node is first reached via the shortest-hop path.
    // The first time we reach the end node is therefore optimal.
    // -------------------------------------------------------------------------

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
                v.dist    = graph.nodeAt(uid).dist + 1.0;  // BFS: unit steps
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
        // BFS path cost = path length - 1 edges (unit weight)
        result.stats.pathCost  = graph.nodeAt(end).dist;
    }

    return result;
}
