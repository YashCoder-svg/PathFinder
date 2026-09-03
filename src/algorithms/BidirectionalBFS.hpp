#pragma once

#include "AlgoResult.hpp"
#include "graph/Graph.hpp"

class BidirectionalBFS {
public:
    static AlgoResult run(Graph& graph, NodeId start, NodeId end);
};
