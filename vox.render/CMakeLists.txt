#  Copyright (c) 2022 Feng Yang
#
#  I am making my contributions/submissions to this project solely in my
#  personal capacity and am not conveying any rights to any intellectual
#  property of any third parties.

cmake_minimum_required(VERSION 3.12)

project(vox.render LANGUAGES C CXX)

set(FRAMEWORK_FILES
        # Header Files
        gui.h
        glsl_compiler.h
        spirv_reflection.h
        gltf_loader.h
        buffer_pool.h
        debug_info.h
        fence_pool.h
        heightmap.h
        semaphore_pool.h
        resource_binding_state.h
        resource_cache.h
        resource_record.h
        resource_replay.h
        vulkan_sample.h
        api_vulkan_sample.h
        timer.h
        camera.h
        # Source Files
        gui.cpp
        glsl_compiler.cpp
        spirv_reflection.cpp
        gltf_loader.cpp
        debug_info.cpp
        buffer_pool.cpp
        fence_pool.cpp
        heightmap.cpp
        semaphore_pool.cpp
        resource_binding_state.cpp
        resource_cache.cpp
        resource_record.cpp
        resource_replay.cpp
        vulkan_sample.cpp
        api_vulkan_sample.cpp
        timer.cpp
        camera.cpp
        )

set(COMMON_FILES
        # Header Files
        singleton.h
        event.h
        event-inl.h
        vk_common.h
        vk_initializers.h
        resource_caching.h
        logging.h
        helpers.h
        error.h
        utils.h
        strings.h
        tags.h
        # Source Files
        error.cpp
        vk_common.cpp
        utils.cpp
        strings.cpp)

set(RENDERING_FILES
        # Header files
        rendering/pipeline_state.h
        #    rendering/postprocessing_pipeline.h
        #    rendering/postprocessing_pass.h
        #    rendering/postprocessing_renderpass.h
        #    rendering/postprocessing_computepass.h
        rendering/render_context.h
        rendering/render_frame.h
        rendering/render_pipeline.h
        rendering/render_target.h
        rendering/subpass.h
        # Source files
        rendering/pipeline_state.cpp
        #    rendering/postprocessing_pipeline.cpp
        #    rendering/postprocessing_pass.cpp
        #    rendering/postprocessing_renderpass.cpp
        #    rendering/postprocessing_computepass.cpp
        rendering/render_context.cpp
        rendering/render_frame.cpp
        rendering/render_pipeline.cpp
        rendering/render_target.cpp
        rendering/subpass.cpp)

set(SCENE_RESOURCE_FILES
        # Header Files
        material.h
        pbr_material.h
        renderer.h
        image_manager.h
        image.h
        image/astc_img.h
        image/ktx_img.h
        image/stb_img.h
        # Source Files
        material.cpp
        pbr_material.cpp
        renderer.cpp
        image_manager.cpp
        image.cpp
        image/astc_img.cpp
        image/ktx_img.cpp
        image/stb_img.cpp)

#set(RENDERING_SUBPASSES_FILES
#    # Header files
#    rendering/subpasses/forward_subpass.h
#    rendering/subpasses/lighting_subpass.h
#    rendering/subpasses/geometry_subpass.h
#    # Source files
#    rendering/subpasses/forward_subpass.cpp
#    rendering/subpasses/lighting_subpass.cpp
#    rendering/subpasses/geometry_subpass.cpp)
#
#set(SCENE_GRAPH_FILES
#    # Header Files
#    scene_graph/component.h
#    scene_graph/node.h
#    scene_graph/scene.h
#    scene_graph/script.h
#    # Source Files
#    scene_graph/component.cpp
#    scene_graph/node.cpp
#    scene_graph/scene.cpp
#    scene_graph/script.cpp)
#
#set(SCENE_GRAPH_COMPONENT_FILES
#    # Header Files
#    scene_graph/components/aabb.h
#    scene_graph/components/camera.h
#    scene_graph/components/perspective_camera.h
#    scene_graph/components/orthographic_camera.h
#    scene_graph/components/image.h
#    scene_graph/components/light.h
#    scene_graph/components/material.h
#    scene_graph/components/mesh.h
#    scene_graph/components/pbr_material.h
#    scene_graph/components/sampler.h
#    scene_graph/components/sub_mesh.h
#    scene_graph/components/texture.h
#    scene_graph/components/transform.h
#    scene_graph/components/image/astc.h
#    scene_graph/components/image/ktx.h
#    scene_graph/components/image/stb.h
#    # Source Files
#    scene_graph/components/aabb.cpp
#    scene_graph/components/camera.cpp
#    scene_graph/components/perspective_camera.cpp
#    scene_graph/components/orthographic_camera.cpp
#    scene_graph/components/image.cpp
#    scene_graph/components/light.cpp
#    scene_graph/components/material.cpp
#    scene_graph/components/mesh.cpp
#    scene_graph/components/pbr_material.cpp
#    scene_graph/components/sampler.cpp
#    scene_graph/components/sub_mesh.cpp
#    scene_graph/components/texture.cpp
#    scene_graph/components/transform.cpp
#    scene_graph/components/image/astc.cpp
#    scene_graph/components/image/ktx.cpp
#    scene_graph/components/image/stb.cpp)
#
#set(SCENE_GRAPH_SCRIPTS_FILES
#    # Header Files
#    scene_graph/scripts/free_camera.h
#    scene_graph/scripts/node_animation.h
#    scene_graph/scripts/animation.h
#    # Source Files
#    scene_graph/scripts/free_camera.cpp
#    scene_graph/scripts/node_animation.cpp
#    scene_graph/scripts/animation.cpp)

set(STATS_FILES
        # Header Files
        stats/stats.h
        stats/stats_common.h
        stats/stats_provider.h
        stats/frame_time_stats_provider.h
        stats/hwcpipe_stats_provider.h
        stats/vulkan_stats_provider.h

        # Source Files
        stats/stats.cpp
        stats/stats_provider.cpp
        stats/frame_time_stats_provider.cpp
        stats/hwcpipe_stats_provider.cpp
        stats/vulkan_stats_provider.cpp)

set(CORE_FILES
        # Header Files
        core/instance.h
        core/physical_device.h
        core/device.h
        core/debug.h
        core/shader_module.h
        core/pipeline_layout.h
        core/pipeline.h
        core/descriptor_set_layout.h
        core/descriptor_pool.h
        core/descriptor_set.h
        core/queue.h
        core/command_pool.h
        core/swapchain.h
        core/command_buffer.h
        core/buffer.h
        core/image.h
        core/image_view.h
        core/sampled_image.h
        core/instance.h
        core/sampler.h
        core/framebuffer.h
        core/render_pass.h
        core/query_pool.h
        core/scratch_buffer.h
        core/acceleration_structure.h
        core/shader_binding_table.h
        core/vulkan_resource.h
        # Source Files
        core/instance.cpp
        core/physical_device.cpp
        core/device.cpp
        core/debug.cpp
        core/image.cpp
        core/shader_module.cpp
        core/pipeline_layout.cpp
        core/pipeline.cpp
        core/descriptor_set_layout.cpp
        core/descriptor_pool.cpp
        core/descriptor_set.cpp
        core/queue.cpp
        core/command_pool.cpp
        core/swapchain.cpp
        core/command_buffer.cpp
        core/buffer.cpp
        core/image.cpp
        core/image_view.cpp
        core/sampled_image.cpp
        core/instance.cpp
        core/sampler.cpp
        core/framebuffer.cpp
        core/render_pass.cpp
        core/query_pool.cpp
        core/scratch_buffer.cpp
        core/acceleration_structure.cpp
        core/shader_binding_table.cpp
        core/vulkan_resource.cpp)

set(PLATFORM_FILES
        # Header Files
        platform/application.h
        platform/platform.h
        platform/window.h
        platform/filesystem.h
        platform/input_events.h
        platform/configuration.h
        platform/parser.h
        platform/headless_window.h
        platform/plugins/plugin.h
        platform/plugins/plugin_base.h

        # Source Files
        platform/headless_window.cpp
        platform/application.cpp
        platform/platform.cpp
        platform/window.cpp
        platform/filesystem.cpp
        platform/input_events.cpp
        platform/configuration.cpp
        platform/parser.cpp
        platform/plugins/plugin.cpp)

set(GLFW_FILES
        # Header Files
        platform/glfw_window.h
        # Source Files
        platform/glfw_window.cpp
        )

set(PARSER_FILES
        # Header Files
        platform/parsers/CLI11.h
        platform/parsers/help_formatter.h
        # Source Files
        platform/parsers/CLI11.cpp
        platform/parsers/help_formatter.cpp
        )

set(UI_FILES ui)

set(SCENE_FILES
        background.h
        layer.h
        update_flag.h
        update_flag.cpp
        update_flag_manager.h
        update_flag_manager.cpp
        vobject.h
        inspector_item.h
        inspector_item.cpp
        scene_forward.h
        scene_manager.h
        scene_manager.cpp
        scene.h
        scene.cpp
        entity.h
        entity.cpp
        component.h
        component.cpp
        components_manager.h
        components_manager.cpp
        transform.h
        transform.cpp
        camera.h
        camera.cpp
        script.h
        script.cpp
        )

set(CONTROL_FILES controls)
set(PHYSX_FILES physics)
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

set(MESH_FILES mesh)
set(SHADER_FILES shader)

#set(GRAPHING_FILES
#    # Header Files
#    graphing/graph.h
#    graphing/graph_node.h
#    graphing/scene_graph.h
#    graphing/framework_graph.h
#
#    # Source Files
#    graphing/graph.cpp
#    graphing/graph_node.cpp
#    graphing/scene_graph.cpp
#    graphing/framework_graph.cpp)

set(ANDROID_FILES
        # Header Files
        platform/android/android_platform.h
        platform/android/android_window.h
        # Source Files
        platform/android/android_platform.cpp
        platform/android/android_window.cpp)

set(WINDOWS_FILES
        # Header Files
        platform/windows/windows_platform.h
        # Source Files
        platform/windows/windows_platform.cpp)

set(UNIX_FILES
        # Header Files
        platform/unix/unix_platform.h
        # Source Files
        platform/unix/unix_platform.cpp)

set(LINUX_D2D_FILES
        # Header Files
        platform/unix/unix_d2d_platform.h
        platform/unix/direct_window.h
        # Source Files
        platform/unix/unix_d2d_platform.cpp
        platform/unix/direct_window.cpp)

source_group("\\" FILES ${FRAMEWORK_FILES})
source_group("common\\" FILES ${COMMON_FILES})
source_group("platform\\" FILES ${PLATFORM_FILES})
source_group("platform\\" FILES ${GLFW_FILES})
source_group("platform\\parsers" FILES ${PARSER_FILES})
source_group("platform\\windows" FILES ${WINDOWS_FILES})
source_group("platform\\android" FILES ${ANDROID_FILES})
source_group("platform\\unix" FILES ${UNIX_FILES})
source_group("platform\\unix" FILES ${LINUX_D2D_FILES})
source_group("core\\" FILES ${CORE_FILES})
#source_group("geometry\\" FILES ${GEOMETRY_FILES})
source_group("rendering\\" FILES ${RENDERING_FILES})
#source_group("rendering\\subpasses" FILES ${RENDERING_SUBPASSES_FILES})
source_group("\\" FILES ${SCENE_RESOURCE_FILES})
#source_group("scene_graph\\components\\" FILES ${SCENE_GRAPH_COMPONENT_FILES})
#source_group("scene_graph\\scripts\\" FILES ${SCENE_GRAPH_SCRIPTS_FILES})
source_group("stats\\" FILES ${STATS_FILES})
#source_group("graphing\\" FILES ${GRAPHING_FILES})

set(PROJECT_FILES
        ${PLATFORM_FILES}
        ${PARSER_FILES}
        ${COMMON_FILES}
        ${FRAMEWORK_FILES}
        ${CORE_FILES}
        #    ${GEOMETRY_FILES}
        ${RENDERING_FILES}
        #    ${RENDERING_SUBPASSES_FILES}
        ${SCENE_RESOURCE_FILES}
        #    ${SCENE_GRAPH_COMPONENT_FILES}
        #    ${SCENE_GRAPH_SCRIPTS_FILES}
        ${STATS_FILES}
        ${UI_FILES}
        ${SCENE_FILES}
        ${CONTROL_FILES}
        ${PHYSX_FILES}
        ${MESH_FILES}
        ${SHADER_FILES}
        #    ${GRAPHING_FILES}
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

if (${VKB_WARNINGS_AS_ERRORS})
    message(STATUS "Warnings as Errors Enabled")
    if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
        target_compile_options(${PROJECT_NAME} PRIVATE -Werror)
    elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
        target_compile_options(${PROJECT_NAME} PRIVATE /W3 /WX)
    endif ()
endif ()

target_include_directories(${PROJECT_NAME} PUBLIC ../vox.math ${CMAKE_CURRENT_SOURCE_DIR} ${PHYSX_SOURCE})

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
