#pragma once

#include "AlgoResult.hpp"
#include "graph/Graph.hpp"
#include "AStar.hpp"

class GreedyBestFirst {
public:
    static AlgoResult run(Graph& graph, NodeId start, NodeId end,
                          HeuristicType heuristic = HeuristicType::Manhattan);
};
