//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.base/logging.h"
#include "vox.core/dtype.h"

/// Call a numerical templated function based on Dtype. Wrap the function to
/// a lambda function to use DISPATCH_DTYPE_TO_TEMPLATE.
///
/// Before:
///     if (dtype == core::Float32) {
///         func<float>(args);
///     } else if (dtype == core::Float64) {
///         func<double>(args);
///     } else ...
///
/// Now:
///     DISPATCH_DTYPE_TO_TEMPLATE(dtype, [&]() {
///        func<scalar_t>(args);
///     });
///
/// Inspired by:
///     https://github.com/pytorch/pytorch/blob/master/aten/src/ATen/Dispatch.h
#define DISPATCH_DTYPE_TO_TEMPLATE(DTYPE, ...)    \
    [&] {                                         \
        if (DTYPE == vox::core::Float32) {        \
            using scalar_t = float;               \
            return __VA_ARGS__();                 \
        } else if (DTYPE == vox::core::Float64) { \
            using scalar_t = double;              \
            return __VA_ARGS__();                 \
        } else if (DTYPE == vox::core::Int8) {    \
            using scalar_t = int8_t;              \
            return __VA_ARGS__();                 \
        } else if (DTYPE == vox::core::Int16) {   \
            using scalar_t = int16_t;             \
            return __VA_ARGS__();                 \
        } else if (DTYPE == vox::core::Int32) {   \
            using scalar_t = int32_t;             \
            return __VA_ARGS__();                 \
        } else if (DTYPE == vox::core::Int64) {   \
            using scalar_t = int64_t;             \
            return __VA_ARGS__();                 \
        } else if (DTYPE == vox::core::UInt8) {   \
            using scalar_t = uint8_t;             \
            return __VA_ARGS__();                 \
        } else if (DTYPE == vox::core::UInt16) {  \
            using scalar_t = uint16_t;            \
            return __VA_ARGS__();                 \
        } else if (DTYPE == vox::core::UInt32) {  \
            using scalar_t = uint32_t;            \
            return __VA_ARGS__();                 \
        } else if (DTYPE == vox::core::UInt64) {  \
            using scalar_t = uint64_t;            \
            return __VA_ARGS__();                 \
        } else {                                  \
            LOGE("Unsupported data type.");       \
        }                                         \
    }()

#define DISPATCH_DTYPE_TO_TEMPLATE_WITH_BOOL(DTYPE, ...)    \
    [&] {                                                   \
        if (DTYPE == vox::core::Bool) {                     \
            using scalar_t = bool;                          \
            return __VA_ARGS__();                           \
        } else {                                            \
            DISPATCH_DTYPE_TO_TEMPLATE(DTYPE, __VA_ARGS__); \
        }                                                   \
    }()

#define DISPATCH_FLOAT_DTYPE_TO_TEMPLATE(DTYPE, ...) \
    [&] {                                            \
        if (DTYPE == vox::core::Float32) {           \
            using scalar_t = float;                  \
            return __VA_ARGS__();                    \
        } else if (DTYPE == vox::core::Float64) {    \
            using scalar_t = double;                 \
            return __VA_ARGS__();                    \
        } else {                                     \
            LOGE("Unsupported data type.");          \
        }                                            \
    }()

#define DISPATCH_FLOAT_INT_DTYPE_TO_TEMPLATE(FDTYPE, IDTYPE, ...)                \
    [&] {                                                                        \
        if (FDTYPE == vox::core::Float32 && IDTYPE == vox::core::Int32) {        \
            using scalar_t = float;                                              \
            using int_t = int32_t;                                               \
            return __VA_ARGS__();                                                \
        } else if (FDTYPE == vox::core::Float32 && IDTYPE == vox::core::Int64) { \
            using scalar_t = float;                                              \
            using int_t = int64_t;                                               \
            return __VA_ARGS__();                                                \
        } else if (FDTYPE == vox::core::Float64 && IDTYPE == vox::core::Int32) { \
            using scalar_t = double;                                             \
            using int_t = int32_t;                                               \
            return __VA_ARGS__();                                                \
        } else if (FDTYPE == vox::core::Float64 && IDTYPE == vox::core::Int64) { \
            using scalar_t = double;                                             \
            using int_t = int64_t;                                               \
            return __VA_ARGS__();                                                \
        } else {                                                                 \
            LOGE("Unsupported data type.");                                      \
        }                                                                        \
    }()
