# PathFinder Pro 🧭

PathFinder Pro is an interactive, web-based graph pathfinding visualizer powered by a high-performance C++ HTTP backend server.

## Features 🚀

- **Multiple Pathfinding Algorithms**:
  - **Breadth-First Search (BFS)** (Unweighted shortest path)
  - **Dijkstra's Algorithm** (Weighted shortest path)
  - **A* Search** (Heuristic-guided search)
  - **Bellman-Ford Algorithm** (Supports negative weights / cycle detection)
- **Maze Generation**: Recursive backtracking maze generator.
- **Interactive Web Dashboard**:
  - Real-time visualization with speed controls
  - Custom start/end nodes, wall drawing, and weight additions
  - Responsive dark & glassmorphism theme with smooth animations
- **C++ Backend**:
  - Multithreaded REST API server built with C++17 (`cpp-httplib` & `nlohmann/json`)
  - Static file serving for web UI assets

## Tech Stack 🛠️

- **Backend**: C++17, CMake, `cpp-httplib`, `nlohmann/json`, WinSock2 / pthreads
- **Frontend**: HTML5, CSS3 (Vanilla CSS, custom themes), JavaScript (ES6+)

## Getting Started ⚙️

### Prerequisites

- C++17 compatible compiler (GCC, Clang, or MSVC)
- CMake 3.16 or higher

### Building the Project

```bash
# Create build directory
mkdir build
cd build

# Generate build files and compile
cmake ..
cmake --build .
```

### Running the Application

```bash
# Run executable from project root
./build/pathfinder 8080 ./web
```

Open your browser and navigate to `http://localhost:8080`.

## Project Structure 📁

```
PathFinder/
├── CMakeLists.txt
├── src/
│   ├── algorithms/      # BFS, Dijkstra, A*, Bellman-Ford, Maze
│   ├── graph/           # Graph data structures
│   ├── server/          # REST API & Static File Server
│   ├── utils/           # Helper data structures (Binary Heap, etc.)
│   └── vendor/          # Header-only dependencies
└── web/                 # Web interface (HTML, CSS, JS)
```

## License 📜

MIT License
