cmake_minimum_required(VERSION 3.15)
project(Borders)
find_package(OpenCV REQUIRED)
find_package(OpenCV REQUIRED COMPONENTS xfeatures2d)

set(CMAKE_CXX_STANDARD 23)

add_executable(Borders main.cpp)
target_link_libraries(Borders ${OpenCV_LIBS})