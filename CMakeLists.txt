cmake_minimum_required(VERSION 3.25)
project(FileSystem)

set(CMAKE_CXX_STANDARD 17)

add_executable(FileSystem main.cpp src/entity/DiskDriver.cpp src/entity/DiskDriver.h)
