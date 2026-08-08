// =============================================================================
// AStar.cpp — A* Search Algorithm Implementation
// =============================================================================

#include "AStar.hpp"
#include "utils/BinaryHeap.hpp"
#include <chrono>
#include <vector>
#include <algorithm>
#include <cmath>

static std::vector<PathStep> reconstructPath(const Graph& graph, NodeId start, NodeId end) {
    std::vector<PathStep> path;
    NodeId cur = end;
    while (cur != INVALID_NODE && cur != start) {
        const Node& n = graph.nodeAt(cur);
        path.push_back({n.row, n.col});
        cur = n.parent;
    }
    if (cur == start) {
        const Node& s = graph.nodeAt(start);
        path.push_back({s.row, s.col});
    }
    std::reverse(path.begin(), path.end());
    return path;
}

static double calculateHeuristic(const Node& a, const Node& b, HeuristicType type) {
    if (type == HeuristicType::Manhattan) {
        return static_cast<double>(a.manhattanTo(b));
    } else {
        return a.euclideanTo(b);
    }
}

AlgoResult AStar::run(Graph& graph, NodeId start, NodeId end, HeuristicType heuristic) {
    AlgoResult result;
    result.algorithm = (heuristic == HeuristicType::Manhattan) ? "A* (Manhattan)" : "A* (Euclidean)";

    graph.resetState();

    auto t0 = std::chrono::high_resolution_clock::now();

    const Node& targetNode = graph.nodeAt(end);
    BinaryHeap<double, NodeId> pq;

    Node& src = graph.nodeAt(start);
    src.dist = 0.0; // g(n) = 0
    src.parent = INVALID_NODE;

    double initialF = calculateHeuristic(src, targetNode, heuristic);
    pq.push(initialF, start);

    int visitSeq = 0;

    while (!pq.isEmpty()) {
        auto [f, uid] = pq.pop();

        Node& u = graph.nodeAt(uid);
        if (u.visited) continue;
        u.visited = true;

        result.stats.nodesVisited++;
        result.visitedOrder.push_back({u.row, u.col, visitSeq++});

        if (uid == end) break;

        for (const Edge& e : graph.neighbors(uid)) {
            NodeId vid = e.to;
            Node& v = graph.nodeAt(vid);
            if (v.visited) continue;

            double tentativeG = u.dist + e.weight;
            if (tentativeG < v.dist) {
                v.dist = tentativeG;
                v.parent = uid;
                double h = calculateHeuristic(v, targetNode, heuristic);
                double fScore = tentativeG + h;

                if (pq.contains(vid)) {
                    pq.decreaseKey(vid, fScore);
                } else {
                    pq.push(fScore, vid);
                }
            }
        }
    }

    auto t1 = std::chrono::high_resolution_clock::now();
    result.stats.timeMs = std::chrono::duration<double, std::milli>(t1 - t0).count();

    const Node& endNode = graph.nodeAt(end);
    if (endNode.visited) {
        result.pathFound = true;
        result.path = reconstructPath(graph, start, end);
        result.stats.pathLength = static_cast<int>(result.path.size());
        result.stats.pathCost = endNode.dist;
    }

    return result;
}
