#  Copyright (c) 2022 Feng Yang
#
#  I am making my contributions/submissions to this project solely in my
#  personal capacity and am not conveying any rights to any intellectual
#  property of any third parties.#

cmake_minimum_required(VERSION 3.12)
include(android_package)

# create sample app project
project(app LANGUAGES C CXX)

add_subdirectory(plugins)

set(SRC
        primitive_app.cpp
        physx_app.cpp
        physx_dynamic_app.cpp
        physx_joint_app.cpp
        skybox_app.cpp
        assimp_app.cpp
        gui_app.cpp
        gui_custom_app.cpp
        pbr_app.cpp
        irradiance_app.cpp
        ibl_app.cpp
        shadowmap_app.cpp
        cascade_shadowmap_app.cpp
        atomic_compute_app.cpp
        cluster_forward_app.cpp
        multi_light_app.cpp
        particle_app.cpp
        framebuffer_picker_app.cpp
        main.cpp
        )

source_group("\\" FILES ${SRC})

# select target type based on platform
if (ANDROID)
    if (CMAKE_VS_NsightTegra_VERSION)
        list(APPEND SRC ${CMAKE_CURRENT_SOURCE_DIR}/android/AndroidManifest.xml)
    endif ()

    add_library(${PROJECT_NAME} SHARED ${SRC})
else ()
    add_executable(${PROJECT_NAME} WIN32 ${SRC})
endif ()

target_include_directories(${PROJECT_NAME} PUBLIC
        ../
        ../third_party/nlohmann/include
        ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(${PROJECT_NAME} PRIVATE vox.base vox.render vox.math plugins spdlog)

if (CMAKE_BUILD_TYPE MATCHES "Release")
    target_compile_definitions(${PROJECT_NAME} PUBLIC DISABLE_CUDA_PHYSX PX_PHYSX_CORE_EXPORTS NDEBUG PX_SUPPORT_PVD=0)
    target_link_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/third_party/physX/physx/bin/linux.clang/release
            ${CMAKE_SOURCE_DIR}/third_party/assimp/build_release/bin)
ELSEIF (CMAKE_BUILD_TYPE MATCHES "Debug")
    target_compile_definitions(${PROJECT_NAME} PUBLIC DISABLE_CUDA_PHYSX PX_PHYSX_CORE_EXPORTS _DEBUG PX_DEBUG=1 PX_CHECKED=1 PX_NVTX=0 PX_SUPPORT_PVD=1)
    target_link_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/third_party/physX/physx/bin/linux.clang/debug
            ${CMAKE_SOURCE_DIR}/third_party/assimp/build_release/bin)
endif ()

target_link_libraries(${PROJECT_NAME} PUBLIC
        # PhysX
        libPhysX_static_64.a
        libPhysXCharacterKinematic_static_64.a
        libPhysXCommon_static_64.a
        libPhysXCooking_static_64.a
        libPhysXExtensions_static_64.a
        libPhysXFoundation_static_64.a
        libPhysXPvdSDK_static_64.a
        libPhysXVehicle_static_64.a
        # Assimp
        assimp)

# Create android project
if (ANDROID)
    set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "-u ANativeActivity_onCreate")

    if (CMAKE_VS_NsightTegra_VERSION)
        set_property(TARGET ${PROJECT_NAME} PROPERTY ANDROID_GUI ON)
        set_property(TARGET ${PROJECT_NAME} PROPERTY ANDROID_ASSETS_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}/../assets)
        set_property(TARGET ${PROJECT_NAME} PROPERTY ANDROID_JAVA_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../bldsys/android/java)
    endif ()

    # Add packaging project only if not using CMake's toolchain
    if (CMAKE_SYSTEM_VERSION GREATER 1)
        add_android_package_project(
                NAME ${PROJECT_NAME}_package
                DEPENDS ${PROJECT_NAME}
                ASSET_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/../assets
                JAVA_DIR ${CMAKE_CURRENT_SOURCE_DIR}/android/java
                RES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/android/res
                MANIFEST_FILE ${CMAKE_CURRENT_SOURCE_DIR}/android/AndroidManifest.xml)
    endif ()

    # Sync assets and shaders
    android_sync_folder(PATH ${CMAKE_CURRENT_SOURCE_DIR}/../assets)
    android_sync_folder(PATH ${CMAKE_CURRENT_SOURCE_DIR}/../shaders)
endif ()

# Create MSVC project
if (MSVC)
    #Set the working directory to the source of the project so developer dont have to
    set_property(TARGET ${PROJECT_NAME} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")

    #Configure output paths
    foreach (CONFIG_TYPE ${CMAKE_CONFIGURATION_TYPES})
        string(TOUPPER ${CONFIG_TYPE} SUFFIX)
        string(TOLOWER ${CONFIG_TYPE} CONFIG_DIR)
        set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_${SUFFIX} ${CMAKE_CURRENT_BINARY_DIR}/bin/${CONFIG_DIR}/${TARGET_ARCH})
        set_target_properties(${PROJECT_NAME} PROPERTIES LIBRARY_OUTPUT_DIRECTORY_${SUFFIX} ${CMAKE_CURRENT_BINARY_DIR}/lib/${CONFIG_DIR}/${TARGET_ARCH})
        set_target_properties(${PROJECT_NAME} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY_${SUFFIX} ${CMAKE_CURRENT_BINARY_DIR}/lib/${CONFIG_DIR}/${TARGET_ARCH})
    endforeach ()
endif ()
