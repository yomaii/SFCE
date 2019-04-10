cmake_minimum_required(VERSION 3.7)

project (SFCE)

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

add_definitions(-std=c++11)
add_executable(SFCE.out main.cpp famicom.cpp cpu.cpp 6502.cpp 2d.cpp)

target_link_libraries(SFCE.out ${SDL2_LIBRARIES})


