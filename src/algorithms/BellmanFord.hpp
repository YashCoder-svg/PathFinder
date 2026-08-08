#pragma once

// =============================================================================
// BellmanFord.hpp — Bellman-Ford Algorithm
// =============================================================================
// Bellman-Ford computes single-source shortest paths in graphs that can have
// negative edge weights (unlike Dijkstra).
//
// COMPLEXITY:
//   Time:  O(V * E)
//   Space: O(V)
//
// NEGATIVE CYCLE DETECTION:
//   Runs |V| - 1 relaxation iterations. If any edge can still be relaxed on
//   the |V|-th iteration, a negative-weight cycle exists.
// =============================================================================

#include "AlgoResult.hpp"
#include "graph/Graph.hpp"

class BellmanFord {
public:
    static AlgoResult run(Graph& graph, NodeId start, NodeId end);
};
