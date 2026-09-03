#pragma once

#include "AlgoResult.hpp"
#include "graph/Graph.hpp"

class DFS {
public:
    static AlgoResult run(Graph& graph, NodeId start, NodeId end);
};
