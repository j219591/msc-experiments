cmake_minimum_required(VERSION 3.15)
project(RegionGrowing)
find_package(OpenCV REQUIRED)
set(CMAKE_CXX_STANDARD 20)

add_executable(RegionGrowing region_growing.cpp)
add_executable(RegionGrowingB region_growing_b.cpp)
target_link_libraries(RegionGrowing ${OpenCV_LIBS})
target_link_libraries(RegionGrowingB ${OpenCV_LIBS})