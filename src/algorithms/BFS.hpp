#pragma once

// =============================================================================
// BFS.hpp — Breadth-First Search on a grid graph
// =============================================================================
// BFS is the simplest correct pathfinding algorithm for UNWEIGHTED graphs.
// It finds the path with the fewest edges (hops), which on a unit-weight grid
// is also the shortest physical path.
//
// COMPLEXITY:
//   Time:  O(V + E)  — every node and edge is visited at most once
//   Space: O(V)      — the queue holds at most O(V) nodes
//
// WHY BFS BEFORE DIJKSTRA?
//   BFS is the unweighted special case of Dijkstra.  Understanding BFS first
//   makes it easier to see what Dijkstra adds (a priority queue instead of a
//   FIFO queue, so it handles non-uniform edge weights).
// =============================================================================

#include "AlgoResult.hpp"
#include "graph/Graph.hpp"

class BFS {
public:
    // -------------------------------------------------------------------------
    // run() — execute BFS from start to end on the given graph.
    //
    // Parameters:
    //   graph — the grid graph (adjacency list, walls already encoded as missing
    //            edges, so the algorithm never needs to check isWall itself)
    //   start — flat NodeId of the source cell
    //   end   — flat NodeId of the destination cell
    //
    // Returns an AlgoResult with:
    //   • path         — cells from start → end (empty if no path exists)
    //   • visitedOrder — cells in the order BFS expanded them (for animation)
    //   • stats        — nodesVisited, pathLength, pathCost, timeMs
    // -------------------------------------------------------------------------
    static AlgoResult run(Graph& graph, NodeId start, NodeId end);
};
