cmake_minimum_required(VERSION 3.19)
project(Traffic)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(Traffic
        Condition.h
        Environment.h
        main.cpp
        Profiler.h
        Stoplight.h
        Stream.h)
