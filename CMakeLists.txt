cmake_minimum_required(VERSION 3.16)
project(SFML_Simulation LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Шукаємо системну бібліотеку SFML 2.x
find_package(SFML 2.5 COMPONENTS graphics window system REQUIRED)

add_executable(SFML_Simulation main.cpp)

target_link_libraries(SFML_Simulation PRIVATE
    sfml-graphics
    sfml-window
    sfml-system
)