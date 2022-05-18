cmake_minimum_required(VERSION 3.12)

project(arc.core LANGUAGES C CXX)

file(GLOB sources
        ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/hashmap/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/hashmap/cpu/*.cpp)

add_library(${PROJECT_NAME} STATIC ${sources})

target_include_directories(${PROJECT_NAME} PUBLIC
        ../arc.base
        ../arc.core
        ../third_party
        ../third_party/eigen
        ../third_party/json_cpp/include)

# OpenMP
find_package(OpenMP)
if (OpenMP_CXX_FOUND)
    target_link_libraries(${PROJECT_NAME} PUBLIC OpenMP::OpenMP_CXX ${OpenCV_LIBS})
endif ()

# Link third party libraries
target_link_libraries(${PROJECT_NAME} PUBLIC spdlog)