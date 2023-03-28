cmake_minimum_required(VERSION 3.15)
project(Borders)

set(CMAKE_CXX_STANDARD 23)

find_package(OpenMP REQUIRED)
find_package(OpenCV REQUIRED)
find_package(OpenCV REQUIRED COMPONENTS xfeatures2d)

add_executable(Borders main.cpp)
target_compile_options(Borders PUBLIC ${OpenMP_CXX_FLAGS})
target_link_libraries(Borders PUBLIC ${OpenCV_LIBS} ${OpenMP_CXX_LIBRARIES})