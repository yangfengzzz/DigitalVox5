cmake_minimum_required(VERSION 3.12)

project(test.geometry LANGUAGES C CXX)

file(GLOB sources
        ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

add_executable(${PROJECT_NAME} ${sources})

target_include_directories(${PROJECT_NAME} PUBLIC ../arc.geometry
        ${CMAKE_SOURCE_DIR}/third_party/googletest/googlemock/include
        ${CMAKE_SOURCE_DIR}/third_party/googletest/googletest/include)

target_include_directories(${PROJECT_NAME} PUBLIC
        ../third_party
        ../third_party/eigen
        ../third_party/json_cpp/include
        ../third_party/qhull/src
        ../third_party/nanoflann/include)

# Link third party libraries
target_link_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/third_party/googletest/build/lib
        ${CMAKE_SOURCE_DIR}/third_party/qhull/build
        ${CMAKE_SOURCE_DIR}/third_party/json_cpp/build/lib)
target_link_libraries(${PROJECT_NAME} PUBLIC spdlog arc.geometry
        libgmock.a libgmock_main.a libgtest.a libgtest_main.a
        qhullstatic_r qhullcpp jsoncpp)