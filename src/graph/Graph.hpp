#pragma once

#include "Node.hpp"
#include <vector>
#include <stdexcept>
#include <string>

struct Edge {
    NodeId to;
    double weight;
};

enum class ConnectivityMode {
    FourWay,
    EightWay
};

class Graph {
public:

    Graph(int rows, int cols,
          ConnectivityMode mode = ConnectivityMode::FourWay);

    [[nodiscard]] int rows()    const { return rows_; }
    [[nodiscard]] int cols()    const { return cols_; }
    [[nodiscard]] int numNodes() const { return rows_ * cols_; }

    [[nodiscard]]       Node& nodeAt(NodeId id);
    [[nodiscard]] const Node& nodeAt(NodeId id) const;

    [[nodiscard]]       Node& nodeAt(int row, int col);
    [[nodiscard]] const Node& nodeAt(int row, int col) const;

    [[nodiscard]] NodeId idOf(int row, int col) const;

    [[nodiscard]] const std::vector<Edge>& neighbors(NodeId id) const;

    void buildAdjacencyList();

    void setWall(int row, int col, bool wall);

    void setWeight(int row, int col, double weight);

    void setStart(int row, int col);
    void setEnd(int row, int col);

    void resetState();

    struct CellDesc {
        double weight = 1.0;
        bool isWall  = false;
        bool isStart = false;
        bool isEnd   = false;
    };
    void loadFromGrid(const std::vector<std::vector<CellDesc>>& cells);

private:

    void assertBounds(int row, int col) const;

    void buildEdgesFor(NodeId id);

    static const int kDir4[4][2];
    static const int kDir8[8][2];

    int rows_;
    int cols_;
    ConnectivityMode mode_;

    std::vector<Node> nodes_;

    std::vector<std::vector<Edge>> edges_;

    NodeId startId_ = INVALID_NODE;
    NodeId endId_   = INVALID_NODE;
};
