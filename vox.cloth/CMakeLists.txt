cmake_minimum_required(VERSION 3.12)

project(vox.cloth LANGUAGES C CXX)

file(GLOB sources
        ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/ps/unix/PsUnixAtomic.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/NvCloth/ps/*.cpp)

add_library(${PROJECT_NAME} STATIC ${sources})

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

if (CMAKE_BUILD_TYPE MATCHES "Release")
    target_compile_definitions(${PROJECT_NAME} PUBLIC DISABLE_CUDA_PHYSX PX_PHYSX_CORE_EXPORTS NDEBUG PX_SUPPORT_PVD=0)
ELSEIF (CMAKE_BUILD_TYPE MATCHES "Debug")
    target_compile_definitions(${PROJECT_NAME} PUBLIC DISABLE_CUDA_PHYSX PX_PHYSX_CORE_EXPORTS _DEBUG PX_DEBUG=1 PX_CHECKED=1 PX_NVTX=0 PX_SUPPORT_PVD=1)
endif ()

target_include_directories(${PROJECT_NAME} PUBLIC ../
        ${CMAKE_CURRENT_SOURCE_DIR} ${PHYSX_SOURCE}
        ../third_party/nlohmann/include)

# Link third party libraries
target_link_libraries(${PROJECT_NAME} PUBLIC
        volk
        ktx
        stb
        astc
        imgui
        glslang
        SPIRV
        vma
        hwcpipe
        spirv-cross-glsl
        glslang-default-resource-limits
        spdlog
        ctpl
        CLI11::CLI11)