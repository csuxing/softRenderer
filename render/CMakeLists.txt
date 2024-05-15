cmake_minimum_required(VERSION 3.16)
project(render LANGUAGES C CXX)
file(GLOB SOURCES *.h *.cpp)
add_library(${PROJECT_NAME} ${SOURCES})

include_directories(${UTILS_DIR})

target_link_libraries(${PROJECT_NAME} PRIVATE glfw)
target_link_libraries(${PROJECT_NAME} PRIVATE spdlog::spdlog)
target_link_libraries(${PROJECT_NAME} PRIVATE utils)