#pragma once

#include <cmath>
#include <limits>
#include <cstdint>

using NodeId = int;

constexpr NodeId INVALID_NODE = -1;

constexpr double INF = std::numeric_limits<double>::infinity();

enum class TerrainType {
    Normal,
    Road,
    Mud,
    Water
};

inline double getTerrainCost(TerrainType type) {
    switch (type) {
        case TerrainType::Road:   return 0.5;
        case TerrainType::Normal: return 1.0;
        case TerrainType::Mud:    return 3.0;
        case TerrainType::Water:  return 5.0;
        default:                  return 1.0;
    }
}

struct Node {

    int row = 0;
    int col = 0;

    double weight = 1.0;
    TerrainType terrain = TerrainType::Normal;

    bool isWall  = false;
    bool isStart = false;
    bool isEnd   = false;

    double dist = INF;

    NodeId parent = INVALID_NODE;

    bool visited = false;

    [[nodiscard]] NodeId id(int gridCols) const {
        return row * gridCols + col;
    }

    [[nodiscard]] double euclideanTo(const Node& other) const {
        double dr = static_cast<double>(row - other.row);
        double dc = static_cast<double>(col - other.col);
        return std::sqrt(dr * dr + dc * dc);
    }

    [[nodiscard]] int manhattanTo(const Node& other) const {
        return std::abs(row - other.row) + std::abs(col - other.col);
    }

    void resetState() {
        dist    = INF;
        parent  = INVALID_NODE;
        visited = false;
    }
};
