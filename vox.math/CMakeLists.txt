cmake_minimum_required(VERSION 3.12)

project(vox.math LANGUAGES C CXX)

set(MATH_FILES
        macros.h
        constants.h
        parallel.h
        parallel-inl.h
        parallel.cpp
        type_helpers.h
        functors.h
        functors-inl.h
        math_utils.h
        math_utils-inl.h
        algorithm.h
        random.h
        random.cpp)

add_library(${PROJECT_NAME} STATIC ${MATH_FILES})
