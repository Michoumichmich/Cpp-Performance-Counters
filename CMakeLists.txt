cmake_minimum_required(VERSION 3.0)
project(Cpp-Performance-Counters)

set(CMAKE_CXX_STANDARD 20)

set(PROFILER_INCLUDES include/internal/cache.hpp include/internal/memory_profiler.hpp include/internal/branch_profiler.hpp include/profiler.hpp include/internal/config.hpp)


set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Ofast -march=native -mtune=native")
add_executable(ex1 examples/ex1.cpp ${PROFILES_INCLUDES})
add_executable(ex2 examples/ex2.cpp ${PROFILES_INCLUDES})

