cmake_minimum_required(VERSION 3.16)
project(utils LANGUAGES C CXX)
file(GLOB SOURCES *.h *.cpp) 
add_library(${PROJECT_NAME} ${SOURCES})
 
target_link_libraries(${PROJECT_NAME} PRIVATE spdlog::spdlog)