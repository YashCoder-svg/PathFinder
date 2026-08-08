#pragma once

// =============================================================================
// AStar.hpp — A* Search Algorithm
// =============================================================================
// A* is an informed search algorithm that uses a heuristic function h(n)
// to guide search towards the target.
// Priority f(n) = g(n) + h(n):
//   g(n) = exact cost from start to node n
//   h(n) = estimated cost from node n to end
//
// HEURISTIC SELECTION:
//   - Manhattan: Standard for 4-directional grid movement.
//   - Euclidean: Standard for 8-directional or arbitrary geometry.
//
// ADMISSIBILITY:
//   h(n) must never overestimate the true cost to reach the goal.
//   Manhattan is admissible on 4-way unit grids; Euclidean is admissible on any grid.
// =============================================================================

#include "AlgoResult.hpp"
#include "graph/Graph.hpp"

enum class HeuristicType {
    Manhattan,
    Euclidean
};

class AStar {
public:
    // -------------------------------------------------------------------------
    // run() — execute A* search from start to end with chosen heuristic.
    // -------------------------------------------------------------------------
    static AlgoResult run(Graph& graph, NodeId start, NodeId end, HeuristicType heuristic = HeuristicType::Manhattan);
};
