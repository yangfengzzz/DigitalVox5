cmake_minimum_required(VERSION 3.12)

project(arc.geometry LANGUAGES C CXX)

file(GLOB sources
        ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

add_library(${PROJECT_NAME} STATIC ${sources})

target_include_directories(${PROJECT_NAME} PUBLIC
        ../third_party
        ../third_party/eigen
        ../third_party/json_cpp/include
        ../third_party/qhull/src
        ../third_party/nanoflann/include)

# Link third party libraries
target_link_libraries(${PROJECT_NAME} PUBLIC spdlog)