cmake_minimum_required(VERSION 3.15)
project(Coursera)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -funroll-loops -funroll-all-loops -march=native")
add_executable(Coursera main.cpp test_runner.hpp stl_container_ostream.hpp visitor.hpp json_printer.cpp json_printer.hpp type_checking.hpp)
