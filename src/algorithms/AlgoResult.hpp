#pragma once

// =============================================================================
// AlgoResult.hpp — Shared return type for all pathfinding algorithms
// =============================================================================
// Every algorithm (BFS, Dijkstra, A*, Bellman-Ford) fills in an AlgoResult
// and returns it to the caller.  The server serialises this to JSON; the
// frontend uses it for animation and the stats panel.
//
// DESIGN NOTE: Using a single concrete struct (rather than a base class with
// virtual methods) keeps things simple and avoids dynamic dispatch overhead.
// The "algorithm name" field is set by the algorithm itself so the caller
// doesn't have to remember which enum value it used.
// =============================================================================

#include <string>
#include <vector>
#include "graph/Node.hpp"   // NodeId

// --------------------------------------------------------------------------
// PathStep — one cell on the final shortest path (in order: start → end).
// --------------------------------------------------------------------------
struct PathStep {
    int row = 0;
    int col = 0;
};

// --------------------------------------------------------------------------
// VisitedStep — one cell in the "exploration" animation order.
//   The frontend animates these one-by-one to show how the algorithm spreads.
// --------------------------------------------------------------------------
struct VisitedStep {
    int row   = 0;
    int col   = 0;
    int order = 0;   // 0-indexed sequence number (useful for colouring by age)
};

// --------------------------------------------------------------------------
// AlgoStats — numerical summary shown in the stats panel.
// --------------------------------------------------------------------------
struct AlgoStats {
    int    nodesVisited = 0;   // How many nodes were popped / expanded
    int    pathLength   = 0;   // Number of cells on the final path (0 = no path)
    double pathCost     = 0.0; // Sum of edge weights along the path
    double timeMs       = 0.0; // Wall-clock execution time (milliseconds)
};

// --------------------------------------------------------------------------
// AlgoResult — the complete return value of one algorithm run.
// --------------------------------------------------------------------------
struct AlgoResult {
    std::string              algorithm;     // e.g. "BFS", "Dijkstra", "A*"
    bool                     pathFound = false;
    std::string              error;         // Non-empty only for Bellman-Ford negative cycle

    std::vector<PathStep>    path;          // Shortest path cells (empty if no path)
    std::vector<VisitedStep> visitedOrder;  // Exploration order for animation

    AlgoStats                stats;
};
