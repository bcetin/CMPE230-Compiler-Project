cmake_minimum_required(VERSION 2.8.12.2)
project(project3_merve_ilik_2015400006_master)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(SOURCE_FILES
        comp.cpp
        )

add_executable(project3 ${SOURCE_FILES})