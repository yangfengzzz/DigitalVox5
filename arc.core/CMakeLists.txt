cmake_minimum_required(VERSION 3.12)

project(arc.core LANGUAGES C CXX)

file(GLOB sources
        ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

add_library(${PROJECT_NAME} STATIC ${sources})

target_include_directories(${PROJECT_NAME} PUBLIC
        ../arc.base
        ../third_party
        ../third_party/eigen
        ../third_party/json_cpp/include)

# Link third party libraries
target_link_libraries(${PROJECT_NAME} PUBLIC spdlog)