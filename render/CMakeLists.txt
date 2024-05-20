cmake_minimum_required(VERSION 3.16)
project(render LANGUAGES C CXX)

# set core files
set(CORE_FILES 
    core/error.h
    core/helpers.h
    core/instance.h
    core/instance.cpp
    core/device.h
    core/device.cpp
    core/physical_device.h
    core/physical_device.cpp
    core/vulkan_resource.h
    core/vulkan_resource.cpp
)

# find other files
file(GLOB SOURCES *.h *.cpp)

set(PROJECT_FILES
    ${CORE_FILES}
    ${SOURCES}
)

source_group("\\" FILES ${SOURCES})
source_group("core\\" FILES ${CORE_FILES})

# generate lib, not exe
add_library(${PROJECT_NAME} ${PROJECT_FILES})

# include header files
include_directories(${UTILS_DIR})

# link library
target_link_libraries(${PROJECT_NAME} PRIVATE glfw)
target_link_libraries(${PROJECT_NAME} PRIVATE spdlog::spdlog)
target_link_libraries(${PROJECT_NAME} PRIVATE utils)
target_link_libraries(${PROJECT_NAME} PRIVATE volk)