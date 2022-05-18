cmake_minimum_required(VERSION 3.12)

project(test.base LANGUAGES C CXX)

file(GLOB sources
        ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

add_executable(${PROJECT_NAME} ${sources})

target_include_directories(${PROJECT_NAME} PUBLIC ../arc.base
        ${CMAKE_SOURCE_DIR}/third_party/googletest/googlemock/include
        ${CMAKE_SOURCE_DIR}/third_party/googletest/googletest/include)

target_include_directories(${PROJECT_NAME} PUBLIC
        ../third_party/eigen
        ../third_party/json_cpp/include
        ../third_party/zlib/contrib/
        /usr/local/Cellar/openssl@3/3.0.2/include)

# OpenMP
find_package(OpenMP)
if(OpenMP_CXX_FOUND)
    target_link_libraries(${PROJECT_NAME} PUBLIC OpenMP::OpenMP_CXX ${OpenCV_LIBS})
endif()


# Link third party libraries
target_link_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/third_party/googletest/build/lib
        ${CMAKE_SOURCE_DIR}/third_party/json_cpp/build/lib
        /usr/local/Cellar/openssl@3/3.0.2/lib
        ${CMAKE_SOURCE_DIR}/third_party/zlib)
target_link_libraries(${PROJECT_NAME} PUBLIC spdlog arc.base
        libgmock.a libgmock_main.a libgtest.a libgtest_main.a
        jsoncpp libcrypto.a curl libz.a)