cmake_minimum_required(VERSION 3.16)
project(framework LANGUAGES C CXX)

# add macro
if(USE_RENDER_DOC)
    add_definitions(-DRENDER_DOC)
endif()
  
# set core files
set(CORE_FILES 
    core/error.h
    core/error.cpp
    core/debug.h
    core/debug.cpp
    core/helpers.h
    core/helpers.cpp
    core/vulkan_resource.h
    core/vulkan_resource.cpp
    core/command_pool.h
    core/command_pool.cpp
    core/fence_pool.h
    core/fence_pool.cpp
    core/command_buffer.h
    core/command_buffer.cpp
    core/render_frame.h
    core/render_frame.cpp
    core/vulkan_vma.cpp
    core/buffer.h
    core/buffer.cpp
    core/image.h
    core/image.cpp
    core/image_view.h
    core/image_view.cpp
)
 
set(RENDERING_FILES
    rendering/render_context.h
    rendering/render_context.cpp
    rendering/render_pass.h
    rendering/render_pass.cpp
    rendering/framebuffer.h
    rendering/framebuffer.cpp
    rendering/render_frame.h
    rendering/render_frame.cpp
)

set(APP_FILES
    app/device_manager.h
    app/device_manager.cpp
    app/vk_device_manager.h
    app/vk_device_manager.cpp
)

set(SCENE_FILES
    scene/scene.h
    scene/scene.cpp
    scene/node.h
    scene/node.cpp
    scene/component.h
    scene/component.cpp
    scene/gltf_loader.h
    scene/gltf_loader.cpp
    scene/submesh.h
    scene/submesh.cpp
    scene/import_image.h
    scene/import_image.cpp
)

# find other files
file(GLOB SOURCES *.h *.cpp *.hpp)

set(PROJECT_FILES
    ${CORE_FILES}
    ${RENDERING_FILES}
    ${APP_FILES}
    ${SCENE_FILES}
    ${SOURCES}
)

set(CORE_INCLUDE_PATH ${CMAKE_SOURCE_DIR}/framework/core)
set(RENDERING_INCLUDE_PATH ${CMAKE_SOURCE_DIR}/framework/rendering)
set(RENDER_PATH ${CMAKE_SOURCE_DIR}/framework)
set(PLATFORM_INCLUDE_PATH ${CMAKE_SOURCE_DIR}/platform)
set(RENDER_DOC ${CMAKE_SOURCE_DIR}/thirdParty/renderdoc)

source_group("\\" FILES ${SOURCES})
source_group("core\\" FILES ${CORE_FILES})
source_group("rendering\\" FILES ${RENDERING_FILES})
source_group("app\\" FILES ${APP_FILES})
source_group("scene\\" FILES ${SCENE_FILES})

# generate lib, not exe
add_library(${PROJECT_NAME} STATIC ${PROJECT_FILES})

# include header files
include_directories(${UTILS_DIR})
include_directories(${CMAKE_SOURCE_DIR}/thirdParty/vma/include)
include_directories(${RENDER_PATH})
include_directories(${PLATFORM_INCLUDE_PATH})
include_directories(${RENDER_DOC})

# link library
target_link_libraries(${PROJECT_NAME} PRIVATE glfw)
target_link_libraries(${PROJECT_NAME} PRIVATE spdlog::spdlog)
target_link_libraries(${PROJECT_NAME} PRIVATE utils)
target_link_libraries(${PROJECT_NAME} PRIVATE volk)
target_link_libraries(${PROJECT_NAME} PRIVATE glm)
target_link_libraries(${PROJECT_NAME} PUBLIC tinygltf)