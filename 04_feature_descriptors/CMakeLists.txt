cmake_minimum_required(VERSION 3.15)
project(SURFDetector)
find_package(OpenCV REQUIRED)
find_package(OpenCV REQUIRED COMPONENTS xfeatures2d)

set(CMAKE_CXX_STANDARD 14)

add_executable(SURFDetector surf.cpp)
target_link_libraries(SURFDetector ${OpenCV_LIBS})