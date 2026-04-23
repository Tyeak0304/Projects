cmake_minimum_required(VERSION 3.16)
project(CyberPlague CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Optimized for 8GB RAM / 2 CPU hardware constraint
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -march=native -DNDEBUG")
set(CMAKE_CXX_FLAGS_DEBUG   "-g -fsanitize=address,undefined")

include_directories(include)

file(GLOB_RECURSE SOURCES "src/*.cpp")
add_executable(cyber-plague ${SOURCES})
target_compile_options(cyber-plague PRIVATE -Wall -Wextra -Wpedantic)
