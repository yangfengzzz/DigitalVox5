cmake_minimum_required(VERSION 3.12)

project(vox.base LANGUAGES C CXX)

file(GLOB sources
        ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/*.c)

add_library(${PROJECT_NAME} STATIC ${sources})

target_compile_definitions(${PROJECT_NAME} PRIVATE OPEN3D_CXX_STANDARD="${CMAKE_CXX_STANDARD}")
target_compile_definitions(${PROJECT_NAME} PRIVATE OPEN3D_CXX_COMPILER_ID="${CMAKE_CXX_COMPILER_ID}")
target_compile_definitions(${PROJECT_NAME} PRIVATE OPEN3D_CXX_COMPILER_VERSION="${CMAKE_CXX_COMPILER_VERSION}")
target_compile_definitions(${PROJECT_NAME} PRIVATE OPEN3D_CUDA_COMPILER_ID="${CMAKE_CUDA_COMPILER_ID}")
target_compile_definitions(${PROJECT_NAME} PRIVATE OPEN3D_CUDA_COMPILER_VERSION="${CMAKE_CUDA_COMPILER_VERSION}")

# OpenMP
find_package(OpenMP)
if (OpenMP_CXX_FOUND)
    target_link_libraries(${PROJECT_NAME} PUBLIC OpenMP::OpenMP_CXX ${OpenCV_LIBS})
endif ()

target_include_directories(${PROJECT_NAME} PUBLIC
        ../
        ../third_party/eigen
        ../third_party/json_cpp/include
        ../third_party/zlib/contrib/
        ../third_party/zlib/contrib/minizip
        /usr/local/Cellar/openssl@3/3.0.2/include)

# Link third party libraries
target_link_libraries(${PROJECT_NAME} PUBLIC spdlog)