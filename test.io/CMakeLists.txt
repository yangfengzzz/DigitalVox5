cmake_minimum_required(VERSION 3.12)

project(test.io LANGUAGES C CXX)

file(GLOB sources
        ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/file_format/*.cpp)

add_executable(${PROJECT_NAME} ${sources})

target_include_directories(${PROJECT_NAME} PUBLIC ../
        ${CMAKE_SOURCE_DIR}/third_party/googletest/googlemock/include
        ${CMAKE_SOURCE_DIR}/third_party/googletest/googletest/include)

target_include_directories(${PROJECT_NAME} PUBLIC
        ../third_party
        ../third_party/eigen
        ../third_party/json_cpp/include
        ../third_party/qhull/src
        ../third_party/nanoflann/include)

# OpenMP
find_package(OpenMP)
if (OpenMP_CXX_FOUND)
    target_link_libraries(${PROJECT_NAME} PUBLIC OpenMP::OpenMP_CXX ${OpenCV_LIBS})
endif ()

# Link third party libraries
target_link_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/third_party/googletest/build/lib
        ${CMAKE_SOURCE_DIR}/third_party/json_cpp/build/lib
        /usr/local/Cellar/openssl@3/3.0.2/lib
        ${CMAKE_SOURCE_DIR}/third_party/zlib
        ${CMAKE_SOURCE_DIR}/third_party/qhull/build
        ${CMAKE_SOURCE_DIR}/third_party/libjpeg/build
        ${CMAKE_SOURCE_DIR}/third_party/libpng/build
        ${CMAKE_SOURCE_DIR}/third_party/assimp/build_release/bin
        )
target_link_libraries(${PROJECT_NAME} PUBLIC spdlog vox.geometry vox.base vox.io
        libgmock.a libgmock_main.a libgtest.a libgtest_main.a
        jsoncpp libcrypto.a curl libz.a # base
        qhullstatic_r qhullcpp # geometry
        assimp turbojpeg png16)