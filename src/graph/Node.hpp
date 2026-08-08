#pragma once

// =============================================================================
// Node.hpp — Grid Cell Representation
// =============================================================================
// Each cell in the pathfinding grid is a Node. We store both the *static*
// properties of the cell (its position, cost to enter, whether it is a wall)
// and the *dynamic* pathfinding state (tentative distance, parent pointer)
// directly on the node.
//
// DSA Note: Mixing "graph topology" data with "algorithm state" is common in
// competitive-programming templates for clarity and cache locality.  In a
// production system you would separate them (algorithm-specific data lives in
// a separate array indexed by node id) so multiple algorithms can run
// concurrently without stepping on each other's state.
// =============================================================================

#include <cmath>    // std::sqrt, std::abs
#include <limits>   // std::numeric_limits
#include <cstdint>  // uint8_t

// --------------------------------------------------------------------------
// NodeId — a compact 1-D identifier derived from (row, col) coordinates.
// Using a flat integer lets us use arrays instead of maps for O(1) lookup.
// --------------------------------------------------------------------------
using NodeId = int;

// Sentinel: "no node" / "not yet discovered"
constexpr NodeId INVALID_NODE = -1;

// Infinity for distance initialisation (must fit in a double)
constexpr double INF = std::numeric_limits<double>::infinity();

// =============================================================================
// Node
// =============================================================================
struct Node {
    // -------------------------------------------------------------------------
    // Static geometry & terrain
    // -------------------------------------------------------------------------
    int row = 0;   // Row index in the grid (y-axis, 0 = top)
    int col = 0;   // Column index in the grid (x-axis, 0 = left)

    // Terrain weight — the cost to *enter* this cell.
    // BFS ignores this (treats every edge as weight 1).
    // Dijkstra / A* use it directly.
    // Valid range: 1 (normal) … 5 (heavy terrain).  Walls set weight = 0 by
    // convention but are filtered out before any weight is read.
    int weight = 1;

    // -------------------------------------------------------------------------
    // Cell type flags — mutually exclusive in practice.
    // -------------------------------------------------------------------------
    bool isWall  = false;  // Impassable; ignored during graph traversal
    bool isStart = false;  // Source of the shortest-path search
    bool isEnd   = false;  // Destination / target

    // -------------------------------------------------------------------------
    // Per-run algorithm state — reset before each search.
    // -------------------------------------------------------------------------
    // Tentative shortest distance from the source node discovered so far.
    // Initialised to +∞; set to 0 for the source.
    double dist = INF;

    // Parent node id along the best known path.  Stored so we can reconstruct
    // the path by following parent pointers from end → start and reversing.
    NodeId parent = INVALID_NODE;

    // Whether this node has been "finalised" (popped from the priority queue).
    // In Dijkstra / A* a finalised node's distance is optimal and will never
    // be relaxed again.
    bool visited = false;

    // -------------------------------------------------------------------------
    // Computed properties
    // -------------------------------------------------------------------------

    // Flat 1-D id given the grid width. Used as the canonical identifier when
    // storing nodes in arrays / heap position tables.
    [[nodiscard]] NodeId id(int gridCols) const {
        return row * gridCols + col;
    }

    // Euclidean distance to another node — useful for Euclidean heuristic in A*.
    [[nodiscard]] double euclideanTo(const Node& other) const {
        double dr = static_cast<double>(row - other.row);
        double dc = static_cast<double>(col - other.col);
        return std::sqrt(dr * dr + dc * dc);
    }

    // Manhattan distance to another node — the standard A* heuristic on a
    // 4-connected grid.  It is *admissible* (never over-estimates) because
    // every step costs at least 1, and Manhattan counts the minimum steps.
    [[nodiscard]] int manhattanTo(const Node& other) const {
        return std::abs(row - other.row) + std::abs(col - other.col);
    }

    // Reset mutable algorithm state so the same Node objects can be reused
    // across multiple runs without reallocating.
    void resetState() {
        dist    = INF;
        parent  = INVALID_NODE;
        visited = false;
    }
};
