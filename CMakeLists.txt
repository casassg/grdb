cmake_minimum_required(VERSION 3.8)
project(grdb)


set(CMAKE_CXX_STANDARD 11)
include_directories(include)
add_subdirectory(src/db)
add_subdirectory(src/cli)
