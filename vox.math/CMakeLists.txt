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
        random.cpp
        bounding_frustum.h
        bounding_frustum.cpp
        collision_utils.h
        collision_utils.cpp
        color.h
        color.cpp
        spherical_harmonics3.h
        spherical_harmonics3.cpp)

add_library(${PROJECT_NAME} STATIC ${MATH_FILES})
