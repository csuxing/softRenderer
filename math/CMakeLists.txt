cmake_minimum_required(VERSION 3.16)
project(math LANGUAGES C CXX)
file(GLOB SOURCES *.h *.cpp)
add_library(${PROJECT_NAME} ${SOURCES}) 
