cmake_minimum_required(VERSION 3.16)
project(PathfinderPro CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if (MSVC)
    add_compile_options(/W4 /EHsc)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

add_executable(pathfinder
    src/main.cpp
    src/graph/Graph.cpp
    src/algorithms/BFS.cpp
    src/algorithms/Dijkstra.cpp
    src/algorithms/AStar.cpp
    src/algorithms/BellmanFord.cpp
    src/algorithms/Maze.cpp
    src/server/Server.cpp
)

target_include_directories(pathfinder PRIVATE src)

find_package(Threads REQUIRED)
target_link_libraries(pathfinder PRIVATE Threads::Threads)

if (WIN32)
    target_link_libraries(pathfinder PRIVATE ws2_32 wsock32)
endif()
