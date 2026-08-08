#pragma once

// =============================================================================
// Graph.hpp — Grid-Based Adjacency-List Graph
// =============================================================================
// REPRESENTATION CHOICE: Adjacency List vs Matrix
//   For a sparse grid (most cells have ≤ 8 neighbors) an adjacency list is
//   far more memory-efficient than an adjacency matrix.
//     Matrix:  O(V²) space — not viable for a 100×100 grid (10 000 nodes → 100M entries)
//     List:    O(V + E) space — a 100×100 4-connected grid has at most 4V edges → 40K entries
//
// GRID LAYOUT:
//   Nodes are stored in a flat 1-D vector.  The node at (row, col) lives at
//   index  row * cols_ + col.  This gives O(1) lookup and is cache-friendly
//   because a whole row fits in consecutive memory.
//
// EDGE WEIGHT:
//   Each directed edge (u → v) carries the weight of the *destination* node v.
//   This represents the cost of entering v (terrain cost).  For unweighted
//   traversal (BFS) the weight is simply ignored.
//
// NEIGHBORS:
//   Supports 4-connectivity (N/S/E/W) or 8-connectivity (+diagonals).
//   Diagonal moves cost √2 × weight in weighted mode (not yet rounded).
// =============================================================================

#include "Node.hpp"
#include <vector>
#include <stdexcept>
#include <string>

// --------------------------------------------------------------------------
// Edge — represents one directed edge in the adjacency list.
// --------------------------------------------------------------------------
struct Edge {
    NodeId to;      // Destination node id (flat index)
    double weight;  // Cost of traversing this edge (= destination node's weight)
};

// --------------------------------------------------------------------------
// ConnectivityMode — how many neighbors each non-wall cell connects to.
// --------------------------------------------------------------------------
enum class ConnectivityMode {
    FourWay,   // Up / Down / Left / Right  (default for most grid pathfinding)
    EightWay   // FourWay + 4 diagonal directions
};

// =============================================================================
// Graph
// =============================================================================
class Graph {
public:
    // -------------------------------------------------------------------------
    // Constructor — builds an empty rows×cols grid with no walls.
    // Call buildAdjacencyList() after setting walls to populate edges.
    // -------------------------------------------------------------------------
    Graph(int rows, int cols,
          ConnectivityMode mode = ConnectivityMode::FourWay);

    // -------------------------------------------------------------------------
    // Accessors
    // -------------------------------------------------------------------------
    [[nodiscard]] int rows()    const { return rows_; }
    [[nodiscard]] int cols()    const { return cols_; }
    [[nodiscard]] int numNodes() const { return rows_ * cols_; }

    // Flat-index → Node reference
    [[nodiscard]]       Node& nodeAt(NodeId id);
    [[nodiscard]] const Node& nodeAt(NodeId id) const;

    // (row, col) → Node reference (bounds-checked)
    [[nodiscard]]       Node& nodeAt(int row, int col);
    [[nodiscard]] const Node& nodeAt(int row, int col) const;

    // Convert (row, col) to flat id
    [[nodiscard]] NodeId idOf(int row, int col) const;

    // -------------------------------------------------------------------------
    // Adjacency list access — edges_ is populated by buildAdjacencyList().
    // -------------------------------------------------------------------------
    [[nodiscard]] const std::vector<Edge>& neighbors(NodeId id) const;

    // -------------------------------------------------------------------------
    // Graph building
    // -------------------------------------------------------------------------

    // (Re)build the entire adjacency list from the current Node data.
    // Must be called after any wall / terrain changes before running a search.
    void buildAdjacencyList();

    // Mark a single cell as a wall and rebuild edges that touch it.
    // For bulk changes prefer setting nodes then calling buildAdjacencyList().
    void setWall(int row, int col, bool wall);

    // Set terrain weight for a cell (1–5).
    void setWeight(int row, int col, int weight);

    // Set start / end markers (only one of each at a time).
    void setStart(int row, int col);
    void setEnd(int row, int col);

    // Reset all per-run algorithm state on every node.
    // Call this before each search to avoid stale distances / parents.
    void resetState();

    // -------------------------------------------------------------------------
    // Serialisation helpers (used by the REST server)
    // -------------------------------------------------------------------------

    // Load the grid from a 2-D JSON-like flat array of cell descriptors.
    // cells[row][col] = { weight, isWall, isStart, isEnd }
    struct CellDesc {
        int  weight  = 1;
        bool isWall  = false;
        bool isStart = false;
        bool isEnd   = false;
    };
    void loadFromGrid(const std::vector<std::vector<CellDesc>>& cells);

private:
    // =========================================================================
    // Private helpers
    // =========================================================================

    // Check that (row, col) is within grid bounds.
    void assertBounds(int row, int col) const;

    // Build adjacency edges for one specific node (called during full rebuild).
    void buildEdgesFor(NodeId id);

    // Offsets for 4-way and 8-way connectivity.
    // Stored as (dRow, dCol) pairs.
    static const int kDir4[4][2];   // N S E W
    static const int kDir8[8][2];   // N S E W + diagonals

    // =========================================================================
    // Data members
    // =========================================================================
    int rows_;
    int cols_;
    ConnectivityMode mode_;

    // Flat node storage — index = row * cols_ + col
    std::vector<Node> nodes_;

    // Adjacency list — edges_[i] holds all outgoing edges from node i.
    // Rebuilt whenever topology changes (walls / terrain).
    std::vector<std::vector<Edge>> edges_;

    // Track start/end ids so we can clear the old marker before setting a new one.
    NodeId startId_ = INVALID_NODE;
    NodeId endId_   = INVALID_NODE;
};
