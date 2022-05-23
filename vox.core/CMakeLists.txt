cmake_minimum_required(VERSION 3.12)

project(vox.core LANGUAGES C CXX)

file(GLOB sources
        ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/linalg/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/kernel/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/hashmap/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/hashmap/cpu/*.cpp)

add_library(${PROJECT_NAME} STATIC ${sources})

target_compile_definitions(${PROJECT_NAME} PUBLIC MKL_ILP64)
target_compile_options(${PROJECT_NAME} PUBLIC $<TARGET_PROPERTY:MKL::MKL,INTERFACE_COMPILE_OPTIONS>)
target_include_directories(${PROJECT_NAME} PUBLIC $<TARGET_PROPERTY:MKL::MKL,INTERFACE_INCLUDE_DIRECTORIES>)
target_link_libraries(${PROJECT_NAME} PUBLIC $<LINK_ONLY:MKL::MKL>)

target_include_directories(${PROJECT_NAME} PUBLIC
        ../
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