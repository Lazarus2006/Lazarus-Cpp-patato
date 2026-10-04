cmake_minimum_required(VERSION 3.16)
project(DijkstraVisualizer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5 REQUIRED COMPONENTS Widgets OpenGL)

add_executable(dijkstra_visualizer dijkstra7.cpp)

target_link_libraries(dijkstra_visualizer PRIVATE 
    Qt5::Widgets 
    Qt5::OpenGL
    opengl32
)
