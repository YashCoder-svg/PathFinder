#pragma once

#include <string>
#include <vector>
#include "graph/Node.hpp"

struct PathStep {
    int row = 0;
    int col = 0;
};

struct VisitedStep {
    int row      = 0;
    int col      = 0;
    int order    = 0;
    int frontier = 0;
};

struct AlgoStats {
    int    nodesVisited = 0;
    int    pathLength   = 0;
    double pathCost     = 0.0;
    double timeMs       = 0.0;
    int    maxFrontier  = 0;
};

struct AlgoResult {
    std::string              algorithm;
    bool                     pathFound = false;
    std::string              error;

    std::vector<PathStep>    path;
    std::vector<VisitedStep> visitedOrder;

    AlgoStats                stats;
};
