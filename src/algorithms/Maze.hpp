#pragma once

#include <vector>
#include <string>
#include "graph/Graph.hpp"

class MazeGenerator {
public:
    static std::vector<std::vector<Graph::CellDesc>> generateDFS(int rows, int cols);
    static std::vector<std::vector<Graph::CellDesc>> generatePrims(int rows, int cols);
    static std::vector<std::vector<Graph::CellDesc>> generateRecursiveDivision(int rows, int cols);
    static std::vector<std::vector<Graph::CellDesc>> generateCellularAutomata(int rows, int cols);
    static std::vector<std::vector<Graph::CellDesc>> generateSpiral(int rows, int cols);
    static std::vector<std::vector<Graph::CellDesc>> generateTerrainNoise(int rows, int cols);
};
