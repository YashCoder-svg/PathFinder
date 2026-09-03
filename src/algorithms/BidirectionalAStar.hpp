#pragma once

#include "AlgoResult.hpp"
#include "graph/Graph.hpp"
#include "AStar.hpp"

class BidirectionalAStar {
public:
    static AlgoResult run(Graph& graph, NodeId start, NodeId end,
                          HeuristicType heuristic = HeuristicType::Manhattan);
};
