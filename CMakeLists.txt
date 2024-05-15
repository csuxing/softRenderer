cmake_minimum_required (VERSION 3.1)
project(softRenderer)

set(UTILS_DIR "${CMAKE_SOURCE_DIR}/utils")

file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}//output")
 
# use folder
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenMP)
if(OPENMP_FOUND)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Dubug)
endif()

add_subdirectory(thirdParty)
add_subdirectory(math)
add_subdirectory(utils)
add_subdirectory(platform)
add_subdirectory(render)

file(GLOB SOURCES *.h *.cpp)
 
add_executable(${PROJECT_NAME} ${SOURCES}) 
target_link_libraries(${PROJECT_NAME} PRIVATE spdlog::spdlog)
target_link_libraries(${PROJECT_NAME} PRIVATE glfw)
target_link_libraries(${PROJECT_NAME} PRIVATE math)
target_link_libraries(${PROJECT_NAME} PRIVATE utils)
target_link_libraries(${PROJECT_NAME} PRIVATE platform)
target_link_libraries(${PROJECT_NAME} PRIVATE render)