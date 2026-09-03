# PathFinder Pro 🧭⚡

<p align="center">
  <img src="https://img.shields.io/badge/C%2B%2B-17-00599C?style=for-the-badge&logo=c%2B%2B&logoColor=white" alt="C++17" />
  <img src="https://img.shields.io/badge/CMake-3.16+-064F8C?style=for-the-badge&logo=cmake&logoColor=white" alt="CMake" />
  <img src="https://img.shields.io/badge/Frontend-HTML5%20%7C%20CSS3%20%7C%20ES6+-E34F26?style=for-the-badge&logo=javascript&logoColor=white" alt="Web Frontend" />
  <img src="https://img.shields.io/badge/Platform-Windows%20%7C%20Linux%20%7C%20macOS-4EAA25?style=for-the-badge" alt="Cross Platform" />
  <img src="https://img.shields.io/badge/Architecture-REST%20API%20%2B%20Web%20Client-blueviolet?style=for-the-badge" alt="REST Architecture" />
  <img src="https://img.shields.io/badge/License-MIT-green.svg?style=for-the-badge" alt="MIT License" />
</p>

<p align="center">
  <strong>An enterprise-grade, interactive graph pathfinding visualizer, procedural maze sandbox, and algorithmic benchmark suite powered by an ultra-fast C++17 REST backend server.</strong>
</p>

---

## 📑 Table of Contents

- [Overview](#-overview)
- [Key Features](#-key-features)
  - [Pathfinding & Graph Traversal Engine](#-pathfinding--graph-traversal-engine)
  - [Procedural Maze & Terrain Generators](#-procedural-maze--terrain-generators)
  - [Interactive Glassmorphic Web Dashboard](#-interactive-glassmorphic-web-dashboard)
  - [Multi-Algorithm Benchmark & Race Mode](#-multi-algorithm-benchmark--race-mode)
  - [Interactive Theory & Pseudocode Inspector](#-interactive-theory--pseudocode-inspector)
  - [Multi-Stop Waypoints & Live Dragging](#-multi-stop-waypoints--live-dragging)
- [Algorithm Comparison Matrix](#-algorithm-comparison-matrix)
- [System Architecture & Engineering Highlights](#-system-architecture--engineering-highlights)
  - [Custom Min-Heap with Decrease-Key](#custom-min-heap-with-decrease-key)
  - [Multi-Weight Terrain & Diagonal Geometry](#multi-weight-terrain--diagonal-geometry)
  - [REST API Server Pipeline](#rest-api-server-pipeline)
- [Project Directory Structure](#-project-directory-structure)
- [Getting Started & Installation](#-getting-started--installation)
  - [Prerequisites](#prerequisites)
  - [Building from Source](#building-from-source)
    - [Windows (MSVC / MinGW / PowerShell)](#windows-msvc--mingw--powershell)
    - [Linux (Ubuntu / Debian / Arch / Fedora)](#linux-ubuntu--debian--arch--fedora)
    - [macOS (Clang / Homebrew)](#macos-clang--homebrew)
  - [Running the Server](#running-the-server)
- [REST API Reference](#-rest-api-reference)
  - [`GET /api/health`](#get-apihealth)
  - [`GET /api/presets`](#get-apipresets)
  - [`POST /api/solve`](#post-apisolve)
  - [`POST /api/generate-maze`](#post-apigenerate-maze)
- [Keyboard Shortcuts & Hotkeys](#-keyboard-shortcuts--hotkeys)
- [Terrain & Tool Palette Guide](#-terrain--tool-palette-guide)
- [Configuration & Extending the Engine](#-configuration--extending-the-engine)
- [Contributing](#-contributing)
- [License](#-license)

---

## 🌟 Overview

**PathFinder Pro** bridges the gap between academic graph theory, high-performance systems programming, and modern visual design.

At its core, a multithreaded **C++17 HTTP/REST engine** executes graph traversal and heuristic pathfinding calculations in sub-millisecond compute times using cache-friendly data structures and custom heap implementations. On the presentation layer, a responsive **Glassmorphism Web Dashboard** renders real-time exploration waves, step-by-step scrubbable execution timelines, dynamic Web Audio synthesizer harmonics, side-by-side algorithm race benchmarks, and procedural terrain environments.

Whether you are studying graph algorithms, preparing for technical interviews, benchmarking heuristic search efficiency, or developing game AI pathing systems, PathFinder Pro delivers an intuitive and comprehensive toolkit.

---

## ✨ Key Features

### 🚀 Pathfinding & Graph Traversal Engine

PathFinder Pro includes 10+ industry-standard and academic pathfinding algorithms across 5 distinct paradigms:

1. **Optimal Weighted Search**:
   - **A\* Search (Manhattan Heuristic)**: Industry gold standard for orthogonal grid navigation using admissible distance estimates ($f(n) = g(n) + h(n)$).
   - **A\* Search (Euclidean Heuristic)**: Straight-line metric optimized for 8-way diagonal movement.
   - **Dijkstra's Algorithm**: Guaranteed shortest-path exploration on weighted graphs powered by a custom min-heap with $O(\log n)$ decrease-key.
   - **Bellman-Ford Algorithm**: Dynamic programming approach capable of handling arbitrary edge weights and detecting **negative-weight cycles**.

2. **Bidirectional Search (Dual Expanding Frontiers)**:
   - **Bidirectional A\***: Simultaneous forward ($S \to E$) and backward ($E \to S$) heuristic search meeting at the optimal intersection node.
   - **Bidirectional Dijkstra**: Dual-ended uniform-cost search halving the search space area.
   - **Bidirectional BFS**: Ultra-fast unweighted dual wave search.

3. **Fast Heuristic & Greedy Search**:
   - **Greedy Best-First Search (GBFS - Manhattan & Euclidean)**: Pure heuristic exploration ($f(n) = h(n)$) prioritizing velocity toward the target over path optimality.

4. **Classic Graph Traversal**:
   - **Breadth-First Search (BFS)**: FIFO queue exploration guaranteeing shortest path on unweighted unit grids.
   - **Depth-First Search (DFS)**: LIFO stack exploration exploring deep labyrinth paths.
   - **Iterative Deepening DFS (IDDFS)**: Combines the space efficiency of DFS ($O(d)$ memory) with the completeness of BFS.

---

### 🗺️ Procedural Maze & Terrain Generators

Generate complex topological challenges on demand:

- **Recursive Division (Chambers)**: Subdivides the grid recursively into walls with randomly punched doorways, forming realistic floorplans.
- **Cellular Automata (Cave Networks)**: Simulates Conway-style rock birth/survival rules to generate organic caverns and dungeon tunnels.
- **DFS Recursive Backtracker**: Generates perfect single-solution mazes with long corridors and no closed loops.
- **Prim's Algorithm (Minimum Spanning Tree)**: Builds sprawling radial labyrinths by picking lowest-cost adjacent frontier walls.
- **Spiral Labyrinth**: Creates a winding inward spiral maze to stress-test heuristic back-tracking.
- **Procedural Height & Terrain Noise**: Generates smooth terrain elevation layers (Expressways, Sand Hills, Mud Forests, Deep Swamps).
- **Random Scatter**: Configurable density obstacle scatter.

---

### 🎨 Interactive Glassmorphic Web Dashboard

- **4 Curated Visual Themes**:
  - 🌌 *Cyber Midnight* (Default neon dark mode)
  - 🌲 *Emerald Matrix* (Phosphor cyber green)
  - 🌅 *Sunset Amber* (Warm glass gradient)
  - ☀️ *Studio Light* (Clean high-contrast daytime mode)
- **Time Scrubber & Playback Timeline**:
  - Pause, resume, jump to start/end, and step forward/backward through every single node expansion.
  - Variable speed playback: `0.5x`, `1.0x`, `3.0x`, `10x Turbo`, and `Instant`.
- **Synthesizer Web Audio Engine**:
  - Real-time procedural audio synthesis adjusting harmonic pitch based on distance from start, exploration wave velocity, and goal acquisition.
- **Grid Scale Scalability**:
  - Switch seamlessly between `Compact (16×32)`, `Standard (20×40)`, and `Dense (24×50)`.
- **JSON Import / Export**:
  - Save custom maze configurations and problem sets to JSON files and reload them at any time.

---

### 🏁 Multi-Algorithm Benchmark & Race Mode

Launch a head-to-head performance simulation comparing all pathfinding algorithms simultaneously on the identical active grid layout:

- **Visual Race Bars**: Real-time progress bars showing node exploration velocity.
- **Side-by-Side Telemetry Table**:
  - Explored Node Count
  - Shortest Path Length (Steps)
  - Total Weighted Movement Cost
  - C++ Backend Execution Time ($\mu\text{s}$ / $\text{ms}$)
  - Mathematical Optimality Guarantees

---

### 📖 Interactive Theory & Pseudocode Inspector

Deepen algorithmic understanding with integrated theory drawers:
- **Synchronized Pseudocode Block**: Highlights corresponding runtime logic steps.
- **Complexity Metrics**: Exact asymptotic time complexity $O(V, E)$, space complexity, admissibility proofs, and recommended real-world use cases (Robotics, Game Dev, Network Routing, GIS).

---

### 📍 Multi-Stop Waypoints & Live Dragging

- **Multi-Stop Checkpoints (Start $\to$ Waypoint $\to$ Target)**: Insert intermediate waypoint checkpoints to solve multi-segment traveling routes seamlessly.
- **⚡ Live Drag Recalculation**: Drag Start, Target, or Waypoint markers around the grid in real-time — the backend and frontend dynamically re-solve and animate the optimal path at 60 FPS without delay.

---

## 📊 Algorithm Comparison Matrix

| Algorithm | Weighted Grid Support | Negative Weights | Shortest Path (Optimal) | Time Complexity | Space Complexity | Frontier Data Structure | Primary Use Case |
| :--- | :---: | :---: | :---: | :---: | :---: | :---: | :---: |
| **A\* Search (Manhattan)** | ✅ Yes | ❌ No | ✅ Yes (Admissible) | $\mathcal{O}((V + E)\log V)$ | $\mathcal{O}(V)$ | Min-Heap (Decrease-Key) | Grid Games, Robotics |
| **A\* Search (Euclidean)** | ✅ Yes | ❌ No | ✅ Yes (Admissible) | $\mathcal{O}((V + E)\log V)$ | $\mathcal{O}(V)$ | Min-Heap (Decrease-Key) | 8-Way Movement, Continuous Maps |
| **Bidirectional A\*** | ✅ Yes | ❌ No | ✅ Yes (Consistent) | $\mathcal{O}((V + E)\log V)$ | $\mathcal{O}(V)$ | Dual Min-Heaps | Large-scale Point-to-Point Routing |
| **Dijkstra's Algorithm** | ✅ Yes | ❌ No | ✅ Guaranteed | $\mathcal{O}((V + E)\log V)$ | $\mathcal{O}(V)$ | Min-Heap (Decrease-Key) | Network Routing (OSPF), GPS |
| **Bidirectional Dijkstra** | ✅ Yes | ❌ No | ✅ Guaranteed | $\mathcal{O}((V + E)\log V)$ | $\mathcal{O}(V)$ | Dual Min-Heaps | Accelerated Weighted Navigation |
| **Breadth-First Search (BFS)** | ❌ No (Unit) | ❌ No | ✅ Yes (Unweighted) | $\mathcal{O}(V + E)$ | $\mathcal{O}(V)$ | FIFO Queue (`std::queue`) | Peer-to-Peer, Unweighted Mazes |
| **Bidirectional BFS** | ❌ No (Unit) | ❌ No | ✅ Yes (Unweighted) | $\mathcal{O}(b^{d/2})$ | $\mathcal{O}(b^{d/2})$ | Dual FIFO Queues | Fast Maze & Social Graph Search |
| **Greedy Best-First Search** | ✅ Yes | ❌ No | ❌ No (Sub-optimal) | $\mathcal{O}(V \log V)$ | $\mathcal{O}(V)$ | Min-Heap | Real-time Game AI, Fast Exploration |
| **Depth-First Search (DFS)** | ❌ No | ❌ No | ❌ No | $\mathcal{O}(V + E)$ | $\mathcal{O}(V)$ | LIFO Stack (`std::vector`) | Maze Generation, Cycle Detection |
| **Iterative Deepening (IDDFS)**| ❌ No (Unit) | ❌ No | ✅ Yes (Unweighted) | $\mathcal{O}(b^d)$ | $\mathcal{O}(d)$ | Call Stack / Iteration | Memory-Constrained Search Trees |
| **Bellman-Ford** | ✅ Yes | ✅ Yes | ✅ Yes (Cycle Detect) | $\mathcal{O}(V \cdot E)$ | $\mathcal{O}(V)$ | Dynamic Array / Relaxation | Forex Arbitrage, Distance Vector (RIP) |

---

## 🏗️ System Architecture & Engineering Highlights

```
┌─────────────────────────────────────────────────────────────────────────┐
│                           Client (Web Browser)                          │
│   ┌───────────────────┬──────────────────────┬──────────────────────┐   │
│   │  Grid Canvas &    │   Timeline Scrubber  │   Web Audio Synth    │   │
│   │  Live Drag Engine │   & Step Controller  │   Sound Generator    │   │
│   └───────────────────┴──────────────────────┴──────────────────────┘   │
│            │                        │                        │          │
└────────────┼────────────────────────┼────────────────────────┼──────────┘
             │ HTTP REST POST (JSON)  │ GET Static Assets      │
             ▼                        ▼                        ▼
┌─────────────────────────────────────────────────────────────────────────┐
│                    PathFinder Pro Server (C++17)                        │
│   ┌─────────────────────────────────────────────────────────────────┐   │
│   │          cpp-httplib Multithreaded REST Controller              │   │
│   │      /api/solve  •  /api/generate-maze  •  /api/presets         │   │
│   └────────────────────────────────┬────────────────────────────────┘   │
│                                    │                                    │
│                    ┌───────────────┴───────────────┐                    │
│                    ▼                               ▼                    │
│   ┌─────────────────────────────────┐ ┌─────────────────────────────┐   │
│   │      Algorithm Subsystem        │ │   Procedural Maze Engine    │   │
│   │  • A* (Manhattan / Euclidean)   │ │  • Recursive Division       │   │
│   │  • Bidirectional A* / Dijkstra  │ │  • Cellular Automata Caves  │   │
│   │  • Dijkstra (Min-Heap)          │ │  • DFS Backtracker          │   │
│   │  • Bellman-Ford (Cycle Check)   │ │  • Prim's Spanning Tree     │   │
│   │  • GBFS, BFS, DFS, IDDFS        │ │  • Heightmap Noise & Spiral │   │
│   └────────────────┬────────────────┘ └──────────────┬──────────────┘   │
│                    │                                 │                  │
│                    ▼                                 ▼                  │
│   ┌─────────────────────────────────────────────────────────────────┐   │
│   │                     Core Graph Engine                           │   │
│   │  • Custom BinaryHeap<Key, Val> with O(log n) decrease-key       │   │
│   │  • 4-Way Orthogonal & 8-Way Diagonal Euclidean Adjacency        │   │
│   │  • Multi-weight Tile Cost Integration (0.5x -> 5.0x)            │   │
│   └─────────────────────────────────────────────────────────────────┘   │
└─────────────────────────────────────────────────────────────────────────┘
```

### Custom Min-Heap with Decrease-Key

Standard C++ library `std::priority_queue` is a max-heap that does not support the **decrease-key** operation. Standard workarounds involve pushing duplicate elements ($O(E)$ memory and extraneous pops).

PathFinder Pro implements a custom, generic `BinaryHeap<Key, Value, Comparator>` located in [`src/utils/BinaryHeap.hpp`](file:///c:/Users/gupta/PathFinder/src/utils/BinaryHeap.hpp) that maintains an internal index hash map:
- **`push(key, value)`**: $O(\log n)$ sift-up.
- **`decreaseKey(value, newKey)`**: $O(\log n)$ direct position lookup and sift-up.
- **`pop()`**: $O(\log n)$ sift-down from root.
- **Memory footprint**: Strict $O(V)$ space invariant.

### Multi-Weight Terrain & Diagonal Geometry

The graph supports both **4-Way Orthogonal** and **8-Way Diagonal** connectivity with Euclidean distance calculations:
- Orthogonal neighbor cost: $\text{edge\_weight} \times 1.0$
- Diagonal neighbor cost: $\text{edge\_weight} \times \sqrt{2} \approx 1.4142$
- Prevents diagonal clipping through impassable corner walls.

---

## 📁 Project Directory Structure

```
PathFinder/
├── CMakeLists.txt              # Cross-platform CMake build configuration
├── README.md                   # Comprehensive project documentation
├── src/
│   ├── main.cpp                # Server entry point & CLI parameter parsing
│   ├── algorithms/             # Graph search & maze implementations
│   │   ├── AlgoResult.hpp      # Shared telemetry & serialization structs
│   │   ├── AStar.hpp / .cpp    # A* algorithm (Manhattan & Euclidean)
│   │   ├── BellmanFord.hpp/.cpp# Bellman-Ford negative cycle solver
│   │   ├── BidirectionalAStar.hpp / .cpp      # Bidirectional A*
│   │   ├── BidirectionalBFS.hpp / .cpp        # Bidirectional BFS
│   │   ├── BidirectionalDijkstra.hpp / .cpp   # Bidirectional Dijkstra
│   │   ├── BFS.hpp / .cpp      # Breadth-First Search
│   │   ├── DFS.hpp / .cpp      # Depth-First Search
│   │   ├── Dijkstra.hpp / .cpp # Dijkstra's Algorithm
│   │   ├── GreedyBestFirst.hpp / .cpp         # Greedy Best-First Search
│   │   ├── IDDFS.hpp / .cpp    # Iterative Deepening DFS
│   │   └── Maze.hpp / .cpp     # 7 Procedural Maze & Terrain Generators
│   ├── graph/                  # Graph data structures
│   │   ├── Node.hpp            # Node, Edge, and Coordinate structures
│   │   └── Graph.hpp / .cpp    # Adjacency grid, 4/8-way connectivity, weights
│   ├── server/                 # HTTP Backend & REST Controller
│   │   ├── Server.hpp / .cpp   # Endpoint handlers & JSON serialization
│   │   └── vendor/             # Header-only dependencies (httplib, json)
│   └── utils/
│       └── BinaryHeap.hpp      # Generic Min-Heap with O(log n) decrease-key
└── web/                        # Web dashboard client
    ├── index.html              # Modern semantic UI markup & modal dialogs
    ├── style.css               # Glassmorphism design system & animations
    └── script.js               # Frontend controller, live drag & audio synth
```

---

## ⚙️ Getting Started & Installation

### Prerequisites

Ensure you have the following installed on your system:
- **C++17 Compatible Compiler**:
  - Windows: MSVC (Visual Studio 2019+ or Build Tools) or MinGW-w64 (GCC 9+)
  - Linux: GCC 9.0+ or Clang 10.0+
  - macOS: Apple Clang 12.0+ or Xcode Command Line Tools
- **CMake**: Version `3.16` or higher (`cmake --version`)
- **Git**: For repository cloning

---

### Building from Source

Clone the repository to your local machine:

```bash
git clone https://github.com/YashCoder-svg/PathFinder.git
cd PathFinder
```

#### Windows (MSVC / MinGW / PowerShell)

```powershell
# Create and navigate to build directory
mkdir build
cd build

# Configure CMake
cmake ..

# Build release binary
cmake --build . --config Release
```

*Note for MSVC*: The resulting binary will be located in `build/Release/pathfinder.exe` or `build/pathfinder.exe`.

#### Linux (Ubuntu / Debian / Arch / Fedora)

```bash
# Install dependencies (Ubuntu/Debian)
sudo apt update
sudo apt install build-essential cmake

# Create build directory
mkdir -p build && cd build

# Configure and compile
cmake ..
make -j$(nproc)
```

#### macOS (Clang / Homebrew)

```bash
# Install CMake if needed
brew install cmake

# Build
mkdir -p build && cd build
cmake ..
make -j$(sysctl -n hw.ncpu)
```

---

### Running the Server

Run the compiled executable from the project root directory:

```bash
# Syntax: ./pathfinder [port] [path_to_web_folder]

# Default: Port 8080, serving ./web
./build/pathfinder 8080 ./web
```

On Windows (MSVC):
```powershell
.\build\Release\pathfinder.exe 8080 .\web
```

Once running, you will see:
```
Starting PathFinder Pro Backend...
🚀 PathFinder Pro Server listening at http://localhost:8080
```

Open your browser and navigate to **`http://localhost:8080`**.

---

## 📡 REST API Reference

The backend exposes a clean REST API that can be consumed by the web UI or external client applications.

### `GET /api/health`
Checks server status and version.

**Response `(200 OK)`**:
```json
{
  "status": "ok",
  "version": "2.0.0"
}
```

---

### `GET /api/presets`
Fetches pre-configured challenge map scenarios (City Grid, Castle Moat, Slalom Challenge, etc.).

**Response `(200 OK)`**:
```json
{
  "presets": [
    {
      "id": "city-grid",
      "name": "🏙️ Manhattan City Grid",
      "description": "Orthogonal avenues and congested diagonal blocks with highways.",
      "walls": [[2, 2], [2, 3]],
      "weights": [{"row": 10, "col": 0, "weight": 0.5}]
    }
  ]
}
```

---

### `POST /api/solve`
Computes the path between start, waypoints, and target coordinates.

**Request Headers**: `Content-Type: application/json`  
**Request Body**:
```json
{
  "rows": 20,
  "cols": 40,
  "mode": "4way",
  "algorithm": "astar-manhattan",
  "start": [5, 5],
  "end": [14, 34],
  "waypoints": [[10, 20]],
  "grid": {
    "walls": [[6, 5], [7, 5], [8, 5]],
    "weights": [
      { "row": 9, "col": 12, "weight": 3.0 }
    ]
  }
}
```

**Response `(200 OK)`**:
```json
{
  "algorithm": "astar-manhattan",
  "noPathFound": false,
  "path": [
    [5, 5], [5, 6], [5, 7], [14, 34]
  ],
  "visitedOrder": [
    { "row": 5, "col": 5, "order": 0, "frontier": 0 },
    { "row": 5, "col": 6, "order": 1, "frontier": 0 }
  ],
  "stats": {
    "nodesVisited": 142,
    "pathLength": 48,
    "pathCost": 52.5,
    "executionTimeMs": 0.38
  }
}
```

---

### `POST /api/generate-maze`
Generates procedural obstacles or weighted terrain maps.

**Request Body**:
```json
{
  "rows": 20,
  "cols": 40,
  "algorithm": "recursive-division"
}
```

Supported algorithms: `recursive-division`, `cellular-automata`, `dfs`, `prims`, `spiral`, `terrain-noise`.

**Response `(200 OK)`**:
```json
{
  "grid": [
    [
      { "isWall": false, "weight": 1.0 },
      { "isWall": true, "weight": 1.0 }
    ]
  ]
}
```

---

## ⌨️ Keyboard Shortcuts & Hotkeys

| Shortcut | Action | Description |
| :---: | :--- | :--- |
| <kbd>Space</kbd> | **Run / Play / Pause** | Executes the active algorithm or toggles the timeline animation |
| <kbd>Esc</kbd> | **Clear / Reset** | Clears the current path, visited wave, or dismisses error banners |
| <kbd>S</kbd> | **Select Start Tool** | Places or relocates the 🚩 Start Node |
| <kbd>E</kbd> | **Select Target Tool**| Places or relocates the 🎯 Target Node |
| <kbd>P</kbd> | **Select Waypoint Tool**| Places or relocates the 🔷 Checkpoint Waypoint |
| <kbd>W</kbd> | **Select Wall Brush** | Paints impenetrable obstacle barrier tiles ($\infty$ cost) |
| <kbd>H</kbd> | **Select Highway Brush**| Paints low-cost Expressway tiles ($0.5\times$ cost) |
| <kbd>M</kbd> | **Select Mud Brush** | Paints heavy terrain Mud tiles ($3.0\times$ cost) |
| <kbd>B</kbd> | **Select Water Brush**| Paints dense Water/Swamp tiles ($5.0\times$ cost) |
| <kbd>X</kbd> | **Select Eraser** | Clears obstacles/weights back to standard grass ($1.0\times$ cost) |
| <kbd>R</kbd> | **Randomize Nodes** | Generates new random valid positions for Start and Target |
| <kbd>C</kbd> | **Algorithm Race** | Opens the side-by-side benchmark comparison modal |
| <kbd>?</kbd> | **Help Guide** | Displays the full shortcut modal and pro usage tips |

---

## 🎨 Terrain & Tool Palette Guide

| Tool | Color / Badge | Weight Cost | Effect on Algorithms |
| :--- | :---: | :---: | :--- |
| **🚩 Start Node** | `Emerald Green` | — | The search origination coordinate. |
| **🎯 Target Node** | `Crimson Red` | — | The destination node goal coordinate. |
| **🔷 Waypoint** | `Royal Blue` | — | Mandatory intermediate stop: routes $S \to W \to E$. |
| **🧱 Wall** | `Charcoal Navy` | $\infty$ | Blocks all traversals; cannot be penetrated. |
| **🛣️ Highway** | `Cyan Blue` | **0.5x** | Fast lane; algorithms preferentially route through here. |
| **🌾 Standard Ground**| `Transparent` | **1.0x** | Baseline movement cost. |
| **🏖️ Sand Hill** | `Golden Yellow` | **2.0x** | Moderate terrain resistance. |
| **🪵 Mud / Forest** | `Amber Brown` | **3.0x** | Heavy terrain resistance; avoided if clear path exists. |
| **🌊 Deep Water** | `Deep Indigo` | **5.0x** | Severe penalty; algorithms only cross if walls block alternate paths. |

---

## 🛠️ Configuration & Extending the Engine

### Adding a New Algorithm
1. Create header and implementation files in `src/algorithms/YourAlgo.hpp` and `src/algorithms/YourAlgo.cpp`.
2. Return an [`AlgoResult`](file:///c:/Users/gupta/PathFinder/src/algorithms/AlgoResult.hpp) struct containing:
   - `visitedOrder`: Exploration sequence for the step-by-step visualizer.
   - `path`: Final shortest path step coordinates.
   - `stats`: Explored count, total cost, and execution duration.
3. Register the algorithm in [`src/server/Server.cpp`](file:///c:/Users/gupta/PathFinder/src/server/Server.cpp) within `runSingleAlgorithm()`.
4. Add the algorithm option in [`web/index.html`](file:///c:/Users/gupta/PathFinder/web/index.html) and theory notes in [`web/script.js`](file:///c:/Users/gupta/PathFinder/web/script.js).

---

## 🤝 Contributing

Contributions are welcome! If you would like to contribute:

1. **Fork the Repository**
2. **Create a Feature Branch** (`git checkout -b feature/AmazingAlgorithm`)
3. **Commit your Changes** (`git commit -m 'Add AmazingAlgorithm support'`)
4. **Push to the Branch** (`git push origin feature/AmazingAlgorithm`)
5. **Open a Pull Request**

---

## 📄 License

This project is licensed under the **MIT License** — see the [LICENSE](LICENSE) file for full details.

---

<p align="center">
  Crafted with ❤️ by <a href="https://github.com/YashCoder-svg"><strong>Yash Gupta</strong></a>
</p>
