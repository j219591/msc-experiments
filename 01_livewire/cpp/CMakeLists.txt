cmake_minimum_required(VERSION 3.15)
project(Livewire)
find_package(OpenCV REQUIRED)
set(CMAKE_CXX_STANDARD 14)

add_executable(Livewire livewire.cpp)
target_link_libraries(Livewire ${OpenCV_LIBS})