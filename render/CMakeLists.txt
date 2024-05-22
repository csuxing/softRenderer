cmake_minimum_required(VERSION 3.16)
project(render LANGUAGES C CXX)

# set core files
set(CORE_FILES 
    core/error.h
    core/error.cpp
    core/helpers.h
    core/instance.h
    core/instance.cpp
    core/device.h
    core/device.cpp
    core/physical_device.h
    core/physical_device.cpp
    core/vulkan_resource.h
    core/vulkan_resource.cpp
    core/queue.h
    core/queue.cpp
    core/command_pool.h
    core/command_pool.cpp
    core/fence_pool.h
    core/fence_pool.cpp
    core/command_buffer.h
    core/command_buffer.cpp
    core/render_frame.h
    core/render_frame.cpp
    core/vulkan_vma.cpp
)

set(RENDERING_FILES
    rendering/render_context.h
    rendering/render_context.cpp
    rendering/render_pass.h
    rendering/render_pass.cpp
    rendering/framebuffer.h
    rendering/framebuffer.cpp
    rendering/swapchain.h
    rendering/swapchain.cpp
    rendering/render_frame.h
    rendering/render_frame.cpp
)

# find other files
file(GLOB SOURCES *.h *.cpp)

set(PROJECT_FILES
    ${CORE_FILES}
    ${RENDERING_FILES}
    ${SOURCES}
)

source_group("\\" FILES ${SOURCES})
source_group("core\\" FILES ${CORE_FILES})
source_group("rendering\\" FILES ${RENDERING_FILES})

# generate lib, not exe
add_library(${PROJECT_NAME} STATIC ${PROJECT_FILES})

# include header files
include_directories(${UTILS_DIR})
include_directories(${CMAKE_SOURCE_DIR}/thirdParty/vma/include)

# link library
target_link_libraries(${PROJECT_NAME} PRIVATE glfw)
target_link_libraries(${PROJECT_NAME} PRIVATE spdlog::spdlog)
target_link_libraries(${PROJECT_NAME} PRIVATE utils)
target_link_libraries(${PROJECT_NAME} PRIVATE volk)
