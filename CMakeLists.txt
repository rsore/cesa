cmake_minimum_required(VERSION 3.20)
project(cesa)

set(CMAKE_CXX_STANDARD 20)

add_executable(cesa
        main.cpp
        include/cesa/vector.hpp
)

target_include_directories(cesa PUBLIC include)
