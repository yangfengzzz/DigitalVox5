#  Copyright (c) 2022 Feng Yang
#
#  I am making my contributions/submissions to this project solely in my
#  personal capacity and am not conveying any rights to any intellectual
#  property of any third parties.

cmake_minimum_required(VERSION 3.12)

project(vox.editor LANGUAGES C CXX)

set(UI_FILES
        ui/menu_bar.cpp
        ui/project_settings.cpp
        ui/tool_bar.cpp
        ui/asset_browser.cpp
        ui/material_editor.cpp
        ui/hierarchy.cpp
        ui/inspector.cpp
        ui/asset_properties.cpp
        size_converter.cpp
        editor_utils.cpp
        editor_actions.cpp
        editor_resources.cpp
        panels_manager.cpp
        entity_creation_menu.cpp
        editor_application.cpp)

set(VIEW_FILES
        view/view.cpp
        view/view.h
        view/game_view.cpp
        view/game_view.h
        view/asset_view.cpp
        view/asset_view.h
        view/scene_view.cpp
        view/scene_view.h)

set(IMGUI_FILES
        imgui/crude_json.cpp
        imgui/crude_json.h
        imgui/imgui_bezier_math.h
        imgui/imgui_bezier_math.inl
        imgui/imgui_canvas.cpp
        imgui/imgui_canvas.h
        imgui/imgui_curve.cpp
        imgui/imgui_curve.h
        imgui/imgui_extra_math.h
        imgui/imgui_extra_math.inl
        imgui/imgui_node_editor.cpp
        imgui/imgui_node_editor.h
        imgui/imgui_node_editor_api.cpp
        imgui/imgui_node_editor_internal.h
        imgui/imgui_node_editor_internal.inl
        imgui/imgui_shapes.cpp
        imgui/imgui_shapes.h
        imgui/imgui_zmo.cpp
        imgui/imgui_zmo.h)

set(PROFILING
        profiling/profiler.cpp
        profiling/profiler.h
        profiling/profiler_report.h
        profiling/profiler_spy.cpp
        profiling/profiler_spy.h)

set(FRAMEWORK_FILES
        ini_file.cpp
        ${UI_FILES}
        ${VIEW_FILES}
        ${IMGUI_FILES}
        ${PROFILING}
        )

set(LUA_SOURCE
        ../third_party/sol2/single/include
        /usr/local/include/lua
        )

set(PHYSX_SOURCE
        ../third_party/physx/physx/source/common/src/mac
        ../third_party/physx/physx/include
        ../third_party/physx/physx/source/common/include
        ../third_party/physx/physx/source/common/src
        ../third_party/physx/physx/source/geomutils/include
        ../third_party/physx/physx/source/geomutils/src
        ../third_party/physx/physx/source/geomutils/src/contact
        ../third_party/physx/physx/source/geomutils/src/common
        ../third_party/physx/physx/source/geomutils/src/convex
        ../third_party/physx/physx/source/geomutils/src/distance
        ../third_party/physx/physx/source/geomutils/src/sweep
        ../third_party/physx/physx/source/geomutils/src/gjk
        ../third_party/physx/physx/source/geomutils/src/intersection
        ../third_party/physx/physx/source/geomutils/src/mesh
        ../third_party/physx/physx/source/geomutils/src/hf
        ../third_party/physx/physx/source/geomutils/src/pcm
        ../third_party/physx/physx/source/geomutils/src/ccd
        ../third_party/physx/physx/source/physxgpu/include
        ../third_party/physx/physx/../pxshared/include
        ../third_party/physx/physx/source/foundation/include)

set(LOADER_SOURCE ../third_party/assimp/include
        ../third_party/assimp/build_release/include)

set(PROJECT_FILES
        ${FRAMEWORK_FILES}
        )

# No need for explict casts from vk::HandleType to VkHandleType on ANDROID
if (ANDROID)
    add_definitions(-DVULKAN_HPP_TYPESAFE_CONVERSION=1)
endif ()

# Add files based on platform
if (ANDROID)
    list(APPEND PROJECT_FILES ${ANDROID_FILES})
    message(STATUS "Android platform detected")
else ()
    if (DIRECT_TO_DISPLAY)
        list(APPEND PROJECT_FILES ${LINUX_D2D_FILES})
        message(STATUS "Unix d2d platform detected")
    else ()
        list(APPEND PROJECT_FILES ${GLFW_FILES})
        if (WIN32)
            list(APPEND PROJECT_FILES ${WINDOWS_FILES})
            message(STATUS "Windows platform detected")
        else ()
            list(APPEND PROJECT_FILES ${UNIX_FILES})
            message(STATUS "Unix platform detected")
        endif ()
    endif ()
endif ()

# Pre compiled headers
vulkan_samples_pch(PROJECT_FILES pch.cpp)

add_library(${PROJECT_NAME} STATIC ${PROJECT_FILES})

# compiler flags based on compiler type
if (NOT MSVC)
    target_compile_options(${PROJECT_NAME} PUBLIC -fexceptions)
endif ()

if (MSVC)
    target_compile_options(${PROJECT_NAME} PUBLIC /MP)
endif ()

if (${VKB_VALIDATION_LAYERS})
    target_compile_definitions(${PROJECT_NAME} PUBLIC VKB_VALIDATION_LAYERS)
endif ()

# GPU assisted validation layers are not available on macOS.
if (${VKB_VALIDATION_LAYERS_GPU_ASSISTED})
    if (APPLE)
        message(WARNING "GPU assisted validation layers are not currently available on macOS.")
    else ()
        target_compile_definitions(${PROJECT_NAME} PUBLIC VKB_VALIDATION_LAYERS_GPU_ASSISTED)
    endif ()
endif ()

if (${VKB_VULKAN_DEBUG})
    target_compile_definitions(${PROJECT_NAME} PUBLIC VKB_VULKAN_DEBUG)
endif ()

if (CMAKE_BUILD_TYPE MATCHES "Release")
    target_compile_definitions(${PROJECT_NAME} PUBLIC DISABLE_CUDA_PHYSX PX_PHYSX_CORE_EXPORTS NDEBUG PX_SUPPORT_PVD=0)
ELSEIF (CMAKE_BUILD_TYPE MATCHES "Debug")
    target_compile_definitions(${PROJECT_NAME} PUBLIC DISABLE_CUDA_PHYSX PX_PHYSX_CORE_EXPORTS _DEBUG PX_DEBUG=1 PX_CHECKED=1 PX_NVTX=0 PX_SUPPORT_PVD=1)
endif ()

if (${VKB_WARNINGS_AS_ERRORS})
    message(STATUS "Warnings as Errors Enabled")
    if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
        target_compile_options(${PROJECT_NAME} PRIVATE -Werror)
    elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
        target_compile_options(${PROJECT_NAME} PRIVATE /W3 /WX)
    endif ()
endif ()

target_include_directories(${PROJECT_NAME} PUBLIC ../vox.math ../vox.render ./vox.editor ${CMAKE_CURRENT_SOURCE_DIR} ${PHYSX_SOURCE} ${LOADER_SOURCE} ${LUA_SOURCE})

# Link third party libraries
target_link_libraries(${PROJECT_NAME} PUBLIC
        volk
        ktx
        stb
        astc
        imgui
        tinygltf
        glslang
        SPIRV
        vma
        hwcpipe
        spirv-cross-glsl
        glslang-default-resource-limits
        spdlog
        ctpl
        CLI11::CLI11)

if (${NEED_LINK_ATOMIC})
    target_link_libraries(${PROJECT_NAME} PUBLIC atomic)
endif ()

# Link platform specific libraries
if (ANDROID)
    target_link_libraries(${PROJECT_NAME} PUBLIC log android native_app_glue)
else ()
    if (DIRECT_TO_DISPLAY)
        target_link_libraries(${PROJECT_NAME} PRIVATE dl)
    else ()
        target_link_libraries(${PROJECT_NAME} PRIVATE glfw)
    endif ()
endif ()
