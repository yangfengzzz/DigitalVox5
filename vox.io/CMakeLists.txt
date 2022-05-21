cmake_minimum_required(VERSION 3.12)

project(vox.io LANGUAGES C CXX)

file(GLOB sources
        ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/file_format/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/file_format/*.c)

add_library(${PROJECT_NAME} STATIC ${sources})

target_compile_definitions(${PROJECT_NAME} PRIVATE OPEN3D_CXX_STANDARD="${CMAKE_CXX_STANDARD}")
target_compile_definitions(${PROJECT_NAME} PRIVATE OPEN3D_CXX_COMPILER_ID="${CMAKE_CXX_COMPILER_ID}")
target_compile_definitions(${PROJECT_NAME} PRIVATE OPEN3D_CXX_COMPILER_VERSION="${CMAKE_CXX_COMPILER_VERSION}")
target_compile_definitions(${PROJECT_NAME} PRIVATE OPEN3D_CUDA_COMPILER_ID="${CMAKE_CUDA_COMPILER_ID}")
target_compile_definitions(${PROJECT_NAME} PRIVATE OPEN3D_CUDA_COMPILER_VERSION="${CMAKE_CUDA_COMPILER_VERSION}")

# OpenMP
find_package(OpenMP)
if(OpenMP_CXX_FOUND)
    target_link_libraries(${PROJECT_NAME} PUBLIC OpenMP::OpenMP_CXX ${OpenCV_LIBS})
endif()


target_include_directories(${PROJECT_NAME} PUBLIC
        ../vox.base
        ../vox.geometry
        ../third_party/eigen
        ../third_party/json_cpp/include
        ../third_party/zlib/contrib/
        ../third_party/zlib/contrib/minizip
        /usr/local/Cellar/openssl@3/3.0.2/include
        ../third_party/assimp/include
        ../third_party/assimp/build_release/include
        ../third_party/tinygltf
        ../third_party/tinyobj
        ../third_party/libjpeg
        ../third_party/libjpeg/build
        ../third_party/libpng
        ../third_party/libpng/build)

# Link third party libraries
target_link_libraries(${PROJECT_NAME} PUBLIC spdlog)