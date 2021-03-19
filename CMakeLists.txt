cmake_minimum_required(VERSION 3.16)
project(hw005)

set(CMAKE_CXX_STANDARD 17)

add_executable(hw005 main.cpp host_info.cpp host_info.h op.h op.cpp vm_info.cpp vm_info.h tools.h typedef.h  engine.h engine.cpp engine_base.cpp)