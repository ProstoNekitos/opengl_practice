cmake_minimum_required(VERSION 3.15)
project(untitled)

set(CMAKE_CXX_STANDARD 17)

include_directories(include)
link_directories(include)

aux_source_directory(src SRC)

find_library(GLEW
        NAMES libGLEW.a
        HINTS lib)
find_library(GLFW
        NAMES libglfw3.a
        HINTS lib)

add_executable(${PROJECT_NAME} main.cpp ${SRC} include/Shader.h include/stb_image.h src/stb_image.cpp include/Camera.h include/Scene.h include/App.h include/Object.h)
target_link_libraries(${PROJECT_NAME} ${GLEW} ${GLFW} -ldl -lgtest -pthread)