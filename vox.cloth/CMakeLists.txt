cmake_minimum_required(VERSION 3.12)

project(vox.cloth LANGUAGES C CXX)

file(GLOB sources
        ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

add_library(${PROJECT_NAME} STATIC ${sources})

if (CMAKE_BUILD_TYPE MATCHES "Release")
    target_compile_definitions(${PROJECT_NAME} PUBLIC DISABLE_CUDA_PHYSX PX_PHYSX_CORE_EXPORTS NDEBUG PX_SUPPORT_PVD=0)
ELSEIF (CMAKE_BUILD_TYPE MATCHES "Debug")
    target_compile_definitions(${PROJECT_NAME} PUBLIC DISABLE_CUDA_PHYSX PX_PHYSX_CORE_EXPORTS _DEBUG PX_DEBUG=1 PX_CHECKED=1 PX_NVTX=0 PX_SUPPORT_PVD=1)
endif ()

target_include_directories(${PROJECT_NAME} PUBLIC ../)
