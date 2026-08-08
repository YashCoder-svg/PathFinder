// =============================================================================
// Maze.cpp — Maze Generation Algorithms Implementation
// =============================================================================

#include "Maze.hpp"
#include <random>
#include <stack>
#include <algorithm>

std::vector<std::vector<Graph::CellDesc>> MazeGenerator::generateDFS(int rows, int cols) {
    // Initialise grid with all walls
    std::vector<std::vector<Graph::CellDesc>> grid(rows, std::vector<Graph::CellDesc>(cols));
    for (int r = 0; r < rows; ++r) {
        for (int c = 0; c < cols; ++c) {
            grid[r][c].isWall = true;
        }
    }

    std::random_device rd;
    std::mt19937 rng(rd());

    std::stack<std::pair<int, int>> st;
    grid[0][0].isWall = false;
    st.push({0, 0});

    const int dr[4] = {-2, 2, 0, 0};
    const int dc[4] = {0, 0, -2, 2};

    while (!st.empty()) {
        auto [r, c] = st.top();

        std::vector<int> dirs = {0, 1, 2, 3};
        std::shuffle(dirs.begin(), dirs.end(), rng);

        bool carved = false;
        for (int d : dirs) {
            int nr = r + dr[d];
            int nc = c + dc[d];

            if (nr >= 0 && nr < rows && nc >= 0 && nc < cols && grid[nr][nc].isWall) {
                // Carve passage through intermediate wall
                int mr = r + dr[d] / 2;
                int mc = c + dc[d] / 2;
                grid[mr][mc].isWall = false;
                grid[nr][nc].isWall = false;

                st.push({nr, nc});
                carved = true;
                break;
            }
        }

        if (!carved) {
            st.pop();
        }
    }

    return grid;
}

std::vector<std::vector<Graph::CellDesc>> MazeGenerator::generatePrims(int rows, int cols) {
    std::vector<std::vector<Graph::CellDesc>> grid(rows, std::vector<Graph::CellDesc>(cols));
    for (int r = 0; r < rows; ++r) {
        for (int c = 0; c < cols; ++c) {
            grid[r][c].isWall = true;
        }
    }

    std::random_device rd;
    std::mt19937 rng(rd());

    struct WallEdge {
        int r1, c1;
        int r2, c2;
    };
    std::vector<WallEdge> frontier;

    grid[0][0].isWall = false;
    auto addWalls = [&](int r, int c) {
        const int dr[4] = {-2, 2, 0, 0};
        const int dc[4] = {0, 0, -2, 2};
        for (int i = 0; i < 4; ++i) {
            int nr = r + dr[i];
            int nc = c + dc[i];
            if (nr >= 0 && nr < rows && nc >= 0 && nc < cols && grid[nr][nc].isWall) {
                frontier.push_back({r, c, nr, nc});
            }
        }
    };

    addWalls(0, 0);

    while (!frontier.empty()) {
        std::uniform_int_distribution<std::size_t> dist(0, frontier.size() - 1);
        std::size_t idx = dist(rng);
        WallEdge edge = frontier[idx];
        frontier.erase(frontier.begin() + idx);

        if (grid[edge.r2][edge.c2].isWall) {
            grid[edge.r2][edge.c2].isWall = false;
            int mr = (edge.r1 + edge.r2) / 2;
            int mc = (edge.c1 + edge.c2) / 2;
            grid[mr][mc].isWall = false;

            addWalls(edge.r2, edge.c2);
        }
    }

    return grid;
}
