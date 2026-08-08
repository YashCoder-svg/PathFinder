// =============================================================================
// Dijkstra.cpp — Dijkstra's algorithm implementation
// =============================================================================

#include "Dijkstra.hpp"
#include "utils/BinaryHeap.hpp"
#include <chrono>
#include <vector>
#include <algorithm>  // std::reverse

// ---------------------------------------------------------------------------
// Helper: reconstruct path end → start via parent pointers, then reverse.
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
// Dijkstra::run
// =============================================================================
AlgoResult Dijkstra::run(Graph& graph, NodeId start, NodeId end) {
    AlgoResult result;
    result.algorithm = "Dijkstra";

    graph.resetState();

    auto t0 = std::chrono::high_resolution_clock::now();

    // -------------------------------------------------------------------------
    // ALGORITHM:
    //   1. Initialise all distances to ∞; set dist[start] = 0.
    //   2. Push (0, start) onto the min-heap.
    //   3. While the heap is non-empty:
    //        a. Pop (d, u) — the node u with the smallest known distance d.
    //        b. If u is already finalised (visited), skip it.
    //           NOTE: with our decrease-key heap we never insert duplicates,
    //           but this guard is kept for safety.
    //        c. Mark u as finalised.
    //        d. If u == end, stop.
    //        e. For each neighbour v with edge weight w:
    //             If d + w < dist[v]:
    //               Update dist[v] = d + w, parent[v] = u.
    //               If v is in the heap → decreaseKey(v, d+w).
    //               Else              → push(d+w, v).
    //
    // CORRECTNESS ARGUMENT (Greedy choice):
    //   When u is popped it has the globally smallest tentative distance.
    //   Since all edge weights ≥ 0, no future path can improve on d.
    //   Therefore d is the true shortest distance — we can finalise u.
    // -------------------------------------------------------------------------

    int N = graph.numNodes();
    BinaryHeap<double, NodeId> pq;

    // Initialise start node
    Node& src  = graph.nodeAt(start);
    src.dist   = 0.0;
    src.parent = INVALID_NODE;
    pq.push(0.0, start);

    int visitSeq = 0;

    while (!pq.isEmpty()) {
        auto [d, uid] = pq.pop();

        Node& u = graph.nodeAt(uid);
        if (u.visited) continue;   // Already finalised
        u.visited = true;

        result.stats.nodesVisited++;
        result.visitedOrder.push_back({u.row, u.col, visitSeq++});

        if (uid == end) break;    // Optimal path found

        for (const Edge& e : graph.neighbors(uid)) {
            NodeId vid  = e.to;
            Node&  v    = graph.nodeAt(vid);
            if (v.visited) continue;   // Finalised — can't improve

            double newDist = d + e.weight;
            if (newDist < v.dist) {
                v.dist   = newDist;
                v.parent = uid;

                if (pq.contains(vid)) {
                    pq.decreaseKey(vid, newDist);
                } else {
                    pq.push(newDist, vid);
                }
            }
        }
    }

    auto t1 = std::chrono::high_resolution_clock::now();
    result.stats.timeMs = std::chrono::duration<double, std::milli>(t1 - t0).count();

    const Node& endNode = graph.nodeAt(end);
    if (endNode.visited) {
        result.pathFound        = true;
        result.path             = reconstructPath(graph, start, end);
        result.stats.pathLength = static_cast<int>(result.path.size());
        result.stats.pathCost   = endNode.dist;
    }

    return result;
}
