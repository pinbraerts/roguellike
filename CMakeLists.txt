cmake_minimum_required(VERSION 3.12)
project(roguellike)

set(CMAKE_CXX_STANDARD 17)

add_executable(
        test
        test.cpp
        Action.hpp
        Idle.hpp
        map.txt
        Idle.hpp
        Field.hpp
        Replica.hpp
)
