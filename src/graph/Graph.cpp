// =============================================================================
// Graph.cpp — Implementation of the grid-based adjacency-list graph
// =============================================================================

#include "Graph.hpp"
#include <stdexcept>
#include <string>
#include <cmath>  // std::sqrt

// ---------------------------------------------------------------------------
// Direction tables
// ---------------------------------------------------------------------------
// 4-way: North (row-1), South (row+1), West (col-1), East (col+1)
const int Graph::kDir4[4][2] = {
    {-1,  0},  // North
    { 1,  0},  // South
    { 0, -1},  // West
    { 0,  1},  // East
};

// 8-way: 4-way + 4 diagonals
const int Graph::kDir8[8][2] = {
    {-1,  0},  // North
    { 1,  0},  // South
    { 0, -1},  // West
    { 0,  1},  // East
    {-1, -1},  // North-West
    {-1,  1},  // North-East
    { 1, -1},  // South-West
    { 1,  1},  // South-East
};

// =============================================================================
// Constructor
// =============================================================================
Graph::Graph(int rows, int cols, ConnectivityMode mode)
    : rows_(rows), cols_(cols), mode_(mode)
{
    if (rows <= 0 || cols <= 0)
        throw std::invalid_argument("Graph: rows and cols must be positive");

    // Initialise the flat node array
    nodes_.resize(rows * cols);
    for (int r = 0; r < rows; ++r) {
        for (int c = 0; c < cols; ++c) {
            NodeId id = r * cols + c;
            nodes_[id].row = r;
            nodes_[id].col = c;
        }
    }

    // Allocate adjacency list slots (filled by buildAdjacencyList)
    edges_.resize(rows * cols);
    buildAdjacencyList();
}

// =============================================================================
// Bounds checking helper
// =============================================================================
void Graph::assertBounds(int row, int col) const {
    if (row < 0 || row >= rows_ || col < 0 || col >= cols_) {
        throw std::out_of_range(
            "Graph: (" + std::to_string(row) + "," + std::to_string(col) +
            ") is out of range for a " + std::to_string(rows_) +
            "×" + std::to_string(cols_) + " grid"
        );
    }
}

// =============================================================================
// Accessors
// =============================================================================
NodeId Graph::idOf(int row, int col) const {
    assertBounds(row, col);
    return row * cols_ + col;
}

Node& Graph::nodeAt(NodeId id) {
    return nodes_.at(static_cast<std::size_t>(id));
}
const Node& Graph::nodeAt(NodeId id) const {
    return nodes_.at(static_cast<std::size_t>(id));
}

Node& Graph::nodeAt(int row, int col) {
    return nodes_[idOf(row, col)];
}
const Node& Graph::nodeAt(int row, int col) const {
    return nodes_[idOf(row, col)];
}

const std::vector<Edge>& Graph::neighbors(NodeId id) const {
    return edges_.at(static_cast<std::size_t>(id));
}

// =============================================================================
// buildEdgesFor — compute outgoing edges for a single node
// =============================================================================
// DESIGN NOTE: We store edges only for non-wall nodes and only to non-wall
// neighbours.  This keeps the adjacency list lean and means wall checks don't
// need to happen inside the algorithm hot-loops — they're already filtered here.
//
// Edge weight = destination node's terrain weight.
//   • For cardinal (N/S/E/W) edges   → weight = dest.weight
//   • For diagonal edges             → weight = dest.weight * √2
//     (a diagonal step covers √2 cells in Euclidean space)
// =============================================================================
void Graph::buildEdgesFor(NodeId id) {
    edges_[id].clear();

    const Node& src = nodes_[id];
    if (src.isWall) return;  // Wall nodes have no outgoing edges

    const int (*dirs)[2]  = (mode_ == ConnectivityMode::FourWay) ? kDir4 : kDir8;
    int numDirs            = (mode_ == ConnectivityMode::FourWay) ? 4     : 8;

    for (int d = 0; d < numDirs; ++d) {
        int nr = src.row + dirs[d][0];
        int nc = src.col + dirs[d][1];

        // Skip out-of-bounds
        if (nr < 0 || nr >= rows_ || nc < 0 || nc >= cols_) continue;

        const Node& dst = nodes_[nr * cols_ + nc];
        if (dst.isWall) continue;  // Skip impassable neighbours

        // Diagonal edges cost √2 times the destination weight
        bool isDiagonal = (dirs[d][0] != 0 && dirs[d][1] != 0);
        double edgeWeight = isDiagonal
            ? static_cast<double>(dst.weight) * std::sqrt(2.0)
            : static_cast<double>(dst.weight);

        edges_[id].push_back({dst.id(cols_), edgeWeight});
    }
}

// =============================================================================
// buildAdjacencyList — rebuild all edges
// =============================================================================
// Called after any bulk topology change (loading a new grid, toggling many
// walls).  For a V-node, E-edge graph this is O(V + E) — acceptable since
// the grid is typically small (≤ 200×200 = 40 000 nodes).
void Graph::buildAdjacencyList() {
    for (NodeId id = 0; id < static_cast<NodeId>(nodes_.size()); ++id) {
        buildEdgesFor(id);
    }
}

// =============================================================================
// setWall — toggle a single cell and rebuild its and its neighbours' edges
// =============================================================================
void Graph::setWall(int row, int col, bool wall) {
    assertBounds(row, col);
    NodeId id        = idOf(row, col);
    nodes_[id].isWall = wall;

    // Rebuild this cell's edges
    buildEdgesFor(id);

    // Also rebuild the edges of all neighbours, because a wall change here
    // may add or remove edges pointing *to* this cell.
    const int (*dirs)[2] = (mode_ == ConnectivityMode::FourWay) ? kDir4 : kDir8;
    int numDirs           = (mode_ == ConnectivityMode::FourWay) ? 4     : 8;

    for (int d = 0; d < numDirs; ++d) {
        int nr = row + dirs[d][0];
        int nc = col + dirs[d][1];
        if (nr < 0 || nr >= rows_ || nc < 0 || nc >= cols_) continue;
        buildEdgesFor(idOf(nr, nc));
    }
}

// =============================================================================
// setWeight / setStart / setEnd
// =============================================================================
void Graph::setWeight(int row, int col, int weight) {
    assertBounds(row, col);
    nodes_[idOf(row, col)].weight = weight;
    // Rebuilding edges of neighbours is necessary because edge weights
    // depend on the *destination* node's weight.
    setWall(row, col, nodes_[idOf(row, col)].isWall); // reuse neighbour-rebuild logic
}

void Graph::setStart(int row, int col) {
    if (startId_ != INVALID_NODE) nodes_[startId_].isStart = false;
    startId_ = idOf(row, col);
    nodes_[startId_].isStart = true;
}

void Graph::setEnd(int row, int col) {
    if (endId_ != INVALID_NODE) nodes_[endId_].isEnd = false;
    endId_ = idOf(row, col);
    nodes_[endId_].isEnd = true;
}

// =============================================================================
// resetState — clear per-run algorithm data
// =============================================================================
// Always call this before starting a new search on the same Graph object.
void Graph::resetState() {
    for (Node& n : nodes_) n.resetState();
}

// =============================================================================
// loadFromGrid — batch-load cell descriptors
// =============================================================================
void Graph::loadFromGrid(const std::vector<std::vector<CellDesc>>& cells) {
    if (static_cast<int>(cells.size()) != rows_)
        throw std::invalid_argument("loadFromGrid: row count mismatch");

    startId_ = INVALID_NODE;
    endId_   = INVALID_NODE;

    for (int r = 0; r < rows_; ++r) {
        if (static_cast<int>(cells[r].size()) != cols_)
            throw std::invalid_argument("loadFromGrid: col count mismatch at row "
                                        + std::to_string(r));
        for (int c = 0; c < cols_; ++c) {
            Node& n        = nodes_[r * cols_ + c];
            const CellDesc& d = cells[r][c];
            n.weight  = d.weight;
            n.isWall  = d.isWall;
            n.isStart = d.isStart;
            n.isEnd   = d.isEnd;

            if (d.isStart) startId_ = n.id(cols_);
            if (d.isEnd)   endId_   = n.id(cols_);
        }
    }

    buildAdjacencyList();
}
