#pragma once

// =============================================================================
// Maze.hpp — Maze Generation Algorithms
// =============================================================================
// Provides maze generation using DFS (Recursive Backtracker) and Prim's Algorithm.
// Returns a 2D grid descriptor where walls are marked true.
// =============================================================================

#include <vector>
#include <string>
#include "graph/Graph.hpp"

class MazeGenerator {
public:
    static std::vector<std::vector<Graph::CellDesc>> generateDFS(int rows, int cols);
    static std::vector<std::vector<Graph::CellDesc>> generatePrims(int rows, int cols);
};
