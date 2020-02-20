cmake_minimum_required(VERSION 3.15)
project(untitled)

set(CMAKE_CXX_STANDARD 17)

find_library(GLEW lib/libGLEW.a REQUIRED)
find_library(GLFW lib/libglfw3.a REQUIRED)

link_directories(include)
link_libraries(GLEW GLFW)

add_executable(${PROJECT_NAME} main.cpp)