#pragma once

// =============================================================================
// Dijkstra.hpp — Dijkstra's Shortest-Path Algorithm
// =============================================================================
// Dijkstra finds the shortest path in a graph with NON-NEGATIVE edge weights.
// It is a generalisation of BFS: instead of a FIFO queue, it uses a min-heap
// (priority queue) keyed by tentative distance, so it always expands the
// currently cheapest reachable node.
//
// COMPLEXITY (with a binary heap + decrease-key):
//   Time:  O((V + E) log V)   — each node/edge processed once; heap ops are log V
//   Space: O(V)               — heap + distance array
//
// WHY CUSTOM HEAP?
//   std::priority_queue has no decrease-key.  The custom BinaryHeap<double,NodeId>
//   supports O(log V) decrease-key via a position map, giving the theoretical
//   optimum.  The alternative (lazy insertion of duplicates) uses O(E) space
//   and does unnecessary work — fine in practice but wrong for a portfolio.
// =============================================================================

#include "AlgoResult.hpp"
#include "graph/Graph.hpp"

class Dijkstra {
public:
    // -------------------------------------------------------------------------
    // run() — execute Dijkstra from start to end on the given weighted graph.
    //
    // Edge weights come from Graph::neighbors(), which already encodes terrain
    // cost (1–5) and diagonal penalties (×√2).  No weight adjustment needed here.
    // -------------------------------------------------------------------------
    static AlgoResult run(Graph& graph, NodeId start, NodeId end);
};
