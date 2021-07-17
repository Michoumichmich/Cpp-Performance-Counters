cmake_minimum_required(VERSION 3.0)
project(Cpp-Performance-Counters)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Ofast")
add_executable(Cpp-Performance-Counters main.cpp include/internal/cache.hpp include/internal/memory_profiler.hpp include/internal/branch_profiler.hpp include/profiler.hpp)

