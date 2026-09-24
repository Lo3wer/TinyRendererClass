cmake_minimum_required(VERSION 3.10)
project(TinyRenderer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenMP REQUIRED)

add_executable(renderer main.cpp tgaimage.cpp model.cpp sgl.cpp)
target_link_libraries(renderer PRIVATE OpenMP::OpenMP_CXX)
