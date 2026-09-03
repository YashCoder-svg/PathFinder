#include "BidirectionalBFS.hpp"
#include <queue>
#include <chrono>
#include <vector>
#include <algorithm>

AlgoResult BidirectionalBFS::run(Graph& graph, NodeId start, NodeId end) {
    AlgoResult result;
    result.algorithm = "Bidirectional BFS";

    graph.resetState();
    auto t0 = std::chrono::high_resolution_clock::now();

    if (start == end) {
        result.pathFound = true;
        const Node& s = graph.nodeAt(start);
        result.path.push_back({s.row, s.col});
        result.stats.nodesVisited = 1;
        result.stats.pathLength = 1;
        result.stats.pathCost = 0.0;
        return result;
    }

    int N = graph.numNodes();
    std::vector<bool> visitedF(N, false);
    std::vector<bool> visitedB(N, false);
    std::vector<NodeId> parentF(N, INVALID_NODE);
    std::vector<NodeId> parentB(N, INVALID_NODE);

    std::queue<NodeId> qF;
    std::queue<NodeId> qB;

    visitedF[start] = true;
    qF.push(start);

    visitedB[end] = true;
    qB.push(end);

    int visitSeq = 0;
    const Node& sNode = graph.nodeAt(start);
    const Node& eNode = graph.nodeAt(end);
    result.visitedOrder.push_back({sNode.row, sNode.col, visitSeq++, 0});
    result.visitedOrder.push_back({eNode.row, eNode.col, visitSeq++, 1});

    NodeId meetNode = INVALID_NODE;
    bool found = false;

    while (!qF.empty() && !qB.empty()) {

        if (!qF.empty()) {
            NodeId u = qF.front();
            qF.pop();
            result.stats.nodesVisited++;

            for (const Edge& e : graph.neighbors(u)) {
                NodeId v = e.to;
                if (!visitedF[v]) {
                    visitedF[v] = true;
                    parentF[v] = u;
                    qF.push(v);
                    const Node& vn = graph.nodeAt(v);
                    result.visitedOrder.push_back({vn.row, vn.col, visitSeq++, 0});

                    if (visitedB[v]) {
                        meetNode = v;
                        found = true;
                        break;
                    }
                }
            }
        }
        if (found) break;

        if (!qB.empty()) {
            NodeId u = qB.front();
            qB.pop();
            result.stats.nodesVisited++;

            for (const Edge& e : graph.neighbors(u)) {
                NodeId v = e.to;
                if (!visitedB[v]) {
                    visitedB[v] = true;
                    parentB[v] = u;
                    qB.push(v);
                    const Node& vn = graph.nodeAt(v);
                    result.visitedOrder.push_back({vn.row, vn.col, visitSeq++, 1});

                    if (visitedF[v]) {
                        meetNode = v;
                        found = true;
                        break;
                    }
                }
            }
        }
        if (found) break;
    }

    auto t1 = std::chrono::high_resolution_clock::now();
    result.stats.timeMs = std::chrono::duration<double, std::milli>(t1 - t0).count();
    result.stats.maxFrontier = static_cast<int>(qF.size() + qB.size());

    if (found && meetNode != INVALID_NODE) {
        result.pathFound = true;

        std::vector<PathStep> forwardPath;
        NodeId cur = meetNode;
        while (cur != INVALID_NODE) {
            const Node& n = graph.nodeAt(cur);
            forwardPath.push_back({n.row, n.col});
            if (cur == start) break;
            cur = parentF[cur];
        }
        std::reverse(forwardPath.begin(), forwardPath.end());

        std::vector<PathStep> backwardPath;
        cur = parentB[meetNode];
        while (cur != INVALID_NODE) {
            const Node& n = graph.nodeAt(cur);
            backwardPath.push_back({n.row, n.col});
            if (cur == end) break;
            cur = parentB[cur];
        }

        result.path = std::move(forwardPath);
        for (const auto& step : backwardPath) {
            result.path.push_back(step);
        }

        result.stats.pathLength = static_cast<int>(result.path.size());

        double totalCost = 0.0;
        for (size_t i = 1; i < result.path.size(); ++i) {
            const Node& n = graph.nodeAt(result.path[i].row, result.path[i].col);
            totalCost += n.weight;
        }
        result.stats.pathCost = totalCost;
    }

    return result;
}
