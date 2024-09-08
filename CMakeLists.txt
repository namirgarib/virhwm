cmake_minimum_required(VERSION 3.12)

project(VIRHWM VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)

include_directories(include)

add_executable(virtual_hardware_m src/virhwm.cpp
        src/gui.cpp)