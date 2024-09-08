cmake_minimum_required(VERSION 3.10)
project(VirtualHardware)


set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_PREFIX_PATH "/opt/homebrew/Cellar/qt/6.6.1/lib/cmake")
find_package(Qt6Widgets REQUIRED)
find_package(OpenCV REQUIRED)
add_executable(simulator virhwm.cpp gui.cpp)
target_link_libraries(simulator ${OpenCV_LIBS})
target_link_libraries(simulator Qt6::Widgets)