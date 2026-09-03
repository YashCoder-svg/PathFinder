#include "Maze.hpp"
#include <random>
#include <stack>
#include <algorithm>
#include <cmath>

std::vector<std::vector<Graph::CellDesc>> MazeGenerator::generateDFS(int rows, int cols) {
    std::vector<std::vector<Graph::CellDesc>> grid(rows, std::vector<Graph::CellDesc>(cols));
    for (int r = 0; r < rows; ++r) {
        for (int c = 0; c < cols; ++c) {
            grid[r][c].isWall = true;
            grid[r][c].weight = 1.0;
        }
    }

    std::random_device rd;
    std::mt19937 rng(rd());

    std::stack<std::pair<int, int>> st;
    grid[1 < rows ? 1 : 0][1 < cols ? 1 : 0].isWall = false;
    st.push({1 < rows ? 1 : 0, 1 < cols ? 1 : 0});

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

    // Connect borders if even dimensions left outer boundary untouched
    for (int r = 0; r < rows; ++r) {
        if (cols > 2 && grid[r][cols - 2].isWall == false && (rng() % 3 == 0)) {
            grid[r][cols - 1].isWall = false;
        }
    }
    for (int c = 0; c < cols; ++c) {
        if (rows > 2 && grid[rows - 2][c].isWall == false && (rng() % 3 == 0)) {
            grid[rows - 1][c].isWall = false;
        }
    }

    return grid;
}

std::vector<std::vector<Graph::CellDesc>> MazeGenerator::generatePrims(int rows, int cols) {
    std::vector<std::vector<Graph::CellDesc>> grid(rows, std::vector<Graph::CellDesc>(cols));
    for (int r = 0; r < rows; ++r) {
        for (int c = 0; c < cols; ++c) {
            grid[r][c].isWall = true;
            grid[r][c].weight = 1.0;
        }
    }

    std::random_device rd;
    std::mt19937 rng(rd());

    struct WallEdge {
        int r1, c1;
        int r2, c2;
    };
    std::vector<WallEdge> frontier;

    int startR = (rows > 1) ? 1 : 0;
    int startC = (cols > 1) ? 1 : 0;
    grid[startR][startC].isWall = false;

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

    addWalls(startR, startC);

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

    // Connect borders if even dimensions left outer boundary untouched
    for (int r = 0; r < rows; ++r) {
        if (cols > 2 && grid[r][cols - 2].isWall == false && (rng() % 3 == 0)) {
            grid[r][cols - 1].isWall = false;
        }
    }
    for (int c = 0; c < cols; ++c) {
        if (rows > 2 && grid[rows - 2][c].isWall == false && (rng() % 3 == 0)) {
            grid[rows - 1][c].isWall = false;
        }
    }

    return grid;
}

static void divideChamber(std::vector<std::vector<Graph::CellDesc>>& grid,
                          int r1, int r2, int c1, int c2, std::mt19937& rng) {
    int height = r2 - r1 + 1;
    int width  = c2 - c1 + 1;

    if (height < 3 || width < 3) return;

    bool horizontal = (height > width) ? true : (width > height ? false : (rng() % 2 == 0));

    if (horizontal) {

        std::vector<int> possibleRows;
        for (int r = r1 + 1; r < r2; r += 1) {
            if (r % 2 == 0) possibleRows.push_back(r);
        }
        if (possibleRows.empty()) return;

        int wallRow = possibleRows[rng() % possibleRows.size()];
        int passageCol = c1 + (rng() % width);

        for (int c = c1; c <= c2; ++c) {
            if (c != passageCol) {
                grid[wallRow][c].isWall = true;
            }
        }

        divideChamber(grid, r1, wallRow - 1, c1, c2, rng);
        divideChamber(grid, wallRow + 1, r2, c1, c2, rng);
    } else {

        std::vector<int> possibleCols;
        for (int c = c1 + 1; c < c2; c += 1) {
            if (c % 2 == 0) possibleCols.push_back(c);
        }
        if (possibleCols.empty()) return;

        int wallCol = possibleCols[rng() % possibleCols.size()];
        int passageRow = r1 + (rng() % height);

        for (int r = r1; r <= r2; ++r) {
            if (r != passageRow) {
                grid[r][wallCol].isWall = true;
            }
        }

        divideChamber(grid, r1, r2, c1, wallCol - 1, rng);
        divideChamber(grid, r1, r2, wallCol + 1, c2, rng);
    }
}

std::vector<std::vector<Graph::CellDesc>> MazeGenerator::generateRecursiveDivision(int rows, int cols) {
    std::vector<std::vector<Graph::CellDesc>> grid(rows, std::vector<Graph::CellDesc>(cols));
    for (int r = 0; r < rows; ++r) {
        for (int c = 0; c < cols; ++c) {
            grid[r][c].isWall = (r == 0 || r == rows - 1 || c == 0 || c == cols - 1);
            grid[r][c].weight = 1.0;
        }
    }

    std::random_device rd;
    std::mt19937 rng(rd());

    divideChamber(grid, 1, rows - 2, 1, cols - 2, rng);
    return grid;
}

std::vector<std::vector<Graph::CellDesc>> MazeGenerator::generateCellularAutomata(int rows, int cols) {
    std::vector<std::vector<Graph::CellDesc>> grid(rows, std::vector<Graph::CellDesc>(cols));
    std::random_device rd;
    std::mt19937 rng(rd());
    std::uniform_real_distribution<double> dist(0.0, 1.0);

    for (int r = 0; r < rows; ++r) {
        for (int c = 0; c < cols; ++c) {
            grid[r][c].weight = 1.0;
            if (r == 0 || r == rows - 1 || c == 0 || c == cols - 1) {
                grid[r][c].isWall = true;
            } else {
                grid[r][c].isWall = (dist(rng) < 0.42);
            }
        }
    }

    for (int iter = 0; iter < 4; ++iter) {
        auto nextGrid = grid;
        for (int r = 1; r < rows - 1; ++r) {
            for (int c = 1; c < cols - 1; ++c) {
                int wallCount = 0;
                for (int dr = -1; dr <= 1; ++dr) {
                    for (int dc = -1; dc <= 1; ++dc) {
                        if (dr == 0 && dc == 0) continue;
                        if (grid[r + dr][c + dc].isWall) wallCount++;
                    }
                }
                if (wallCount >= 5) {
                    nextGrid[r][c].isWall = true;
                } else if (wallCount <= 3) {
                    nextGrid[r][c].isWall = false;
                }
            }
        }
        grid = nextGrid;
    }

    return grid;
}

std::vector<std::vector<Graph::CellDesc>> MazeGenerator::generateSpiral(int rows, int cols) {
    std::vector<std::vector<Graph::CellDesc>> grid(rows, std::vector<Graph::CellDesc>(cols));
    for (int r = 0; r < rows; ++r) {
        for (int c = 0; c < cols; ++c) {
            grid[r][c].isWall = false;
            grid[r][c].weight = 1.0;
        }
    }

    int top = 0, bottom = rows - 1;
    int left = 0, right = cols - 1;

    while (top + 2 <= bottom && left + 2 <= right) {

        for (int c = left; c <= right; ++c) {
            grid[top][c].isWall = true;
            grid[bottom][c].isWall = true;
        }
        for (int r = top; r <= bottom; ++r) {
            grid[r][left].isWall = true;
            grid[r][right].isWall = true;
        }

        if (top + 1 < bottom) {
            grid[top + 1][left].isWall = false;
        }

        top += 2;
        bottom -= 2;
        left += 2;
        right -= 2;
    }

    return grid;
}

std::vector<std::vector<Graph::CellDesc>> MazeGenerator::generateTerrainNoise(int rows, int cols) {
    std::vector<std::vector<Graph::CellDesc>> grid(rows, std::vector<Graph::CellDesc>(cols));
    std::random_device rd;
    std::mt19937 rng(rd());
    std::uniform_real_distribution<double> dist(0.0, 100.0);

    double offX = dist(rng);
    double offY = dist(rng);

    for (int r = 0; r < rows; ++r) {
        for (int c = 0; c < cols; ++c) {
            double nx = (c + offX) * 0.15;
            double ny = (r + offY) * 0.15;

            double v = std::sin(nx) * std::cos(ny)
                     + 0.5 * std::sin(2.0 * nx + 1.2) * std::cos(2.0 * ny + 0.8)
                     + 0.25 * std::sin(4.0 * nx);

            double norm = (v + 1.75) / 3.5;
            if (norm < 0.0) norm = 0.0;
            if (norm > 1.0) norm = 1.0;

            grid[r][c].isWall = false;

            if (norm > 0.82) {
                grid[r][c].isWall = true;
                grid[r][c].weight = 1.0;
            } else if (norm > 0.65) {
                grid[r][c].weight = 5.0;
            } else if (norm > 0.50) {
                grid[r][c].weight = 3.0;
            } else if (norm > 0.35) {
                grid[r][c].weight = 2.0;
            } else if (norm < 0.15) {
                grid[r][c].weight = 0.5;
            } else {
                grid[r][c].weight = 1.0;
            }
        }
    }

    return grid;
}
