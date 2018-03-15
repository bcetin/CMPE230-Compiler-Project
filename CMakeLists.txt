cmake_minimum_required(VERSION 2.8.12.2)
project(comp)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(SOURCE_FILES
        comp.cpp
        )

add_executable(comp ${SOURCE_FILES})
