cmake_minimum_required(VERSION 3.15)
project(untitled)

set(CMAKE_CXX_STANDARD 17)

include_directories(include)
link_directories(include)

include_directories(include/thirdparty)
link_directories(include/thirdparty)

aux_source_directory(${PROJECT_SOURCE_DIR}/src SRC)
aux_source_directory(${PROJECT_SOURCE_DIR}/src/thirdparty THIRDPARTY)

find_library(GLEW
        NAMES libGLEW.a
        HINTS lib)

find_library(GLFW
        NAMES libglfw3.a
        HINTS lib)

add_executable(${PROJECT_NAME} main.cpp ${SRC} ${THIRDPARTY} include/Water.h)
target_link_libraries(${PROJECT_NAME} ${GLEW} ${GLFW} -ldl -lgtest -pthread -lassimp)