cmake_minimum_required(VERSION 3.10)
project(MyProject LANGUAGES CXX)

set(CMAKE_CXX_STANDART 17)
set(CMAKE_CXX_STANDART_REQUIRED ON)

add_executable(MyExecutable main.cpp)