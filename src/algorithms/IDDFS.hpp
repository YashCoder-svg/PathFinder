#pragma once

#include "AlgoResult.hpp"
#include "graph/Graph.hpp"

class IDDFS {
public:
    static AlgoResult run(Graph& graph, NodeId start, NodeId end, int maxDepth = 1000);
};
