cmake_minimum_required(VERSION 3.10)
project(cpp_ros_su26)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(example example.cpp)
add_executable(print print.cpp)
add_executable(variables variables.cpp)
add_executable(functions functions.cpp)
add_executable(namespaces namespaces.cpp)
add_executable(conditional_compile conditional_compile.cpp)