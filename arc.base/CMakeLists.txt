cmake_minimum_required(VERSION 3.12)

project(arc.base LANGUAGES C CXX)

file(GLOB sources
        ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

add_library(${PROJECT_NAME} STATIC ${sources})

target_compile_definitions(${PROJECT_NAME} PRIVATE OPEN3D_CXX_STANDARD="${CMAKE_CXX_STANDARD}")
target_compile_definitions(${PROJECT_NAME} PRIVATE OPEN3D_CXX_COMPILER_ID="${CMAKE_CXX_COMPILER_ID}")
target_compile_definitions(${PROJECT_NAME} PRIVATE OPEN3D_CXX_COMPILER_VERSION="${CMAKE_CXX_COMPILER_VERSION}")
target_compile_definitions(${PROJECT_NAME} PRIVATE OPEN3D_CUDA_COMPILER_ID="${CMAKE_CUDA_COMPILER_ID}")
target_compile_definitions(${PROJECT_NAME} PRIVATE OPEN3D_CUDA_COMPILER_VERSION="${CMAKE_CUDA_COMPILER_VERSION}")

target_include_directories(${PROJECT_NAME} PUBLIC
        ../arc.base
        ../third_party/eigen
        ../third_party/json_cpp/include
        ../third_party/zlib/contrib/)

# Link third party libraries
target_link_libraries(${PROJECT_NAME} PUBLIC spdlog)