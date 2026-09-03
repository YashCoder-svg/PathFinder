#pragma once

#include "AlgoResult.hpp"
#include "graph/Graph.hpp"

enum class HeuristicType {
    Manhattan,
    Euclidean
};

class AStar {
public:

    static AlgoResult run(Graph& graph, NodeId start, NodeId end, HeuristicType heuristic = HeuristicType::Manhattan);
};
