//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <cstring>
#include <string>

#include "vox.base/logging.h"
#include "vox.base/macro.h"
#include "vox.core/dispatch.h"

namespace vox::core {

class OPEN3D_API Dtype {
public:
    static const Dtype Undefined;
    static const Dtype Float32;
    static const Dtype Float64;
    static const Dtype Int8;
    static const Dtype Int16;
    static const Dtype Int32;
    static const Dtype Int64;
    static const Dtype UInt8;
    static const Dtype UInt16;
    static const Dtype UInt32;
    static const Dtype UInt64;
    static const Dtype Bool;

public:
    enum class DtypeCode {
        Undefined,
        Bool,  // Needed to distinguish bool from uint8_t.
        Int,
        UInt,
        Float,
        Object,
    };

    Dtype() : Dtype(DtypeCode::Undefined, 1, "Undefined") {}

    explicit Dtype(DtypeCode dtype_code, int64_t byte_size, const std::string &name);

    /// Convert from C++ types to Dtype. Known types are explicitly specialized,
    /// e.g. FromType<float>(). Unsupported type results in an exception.
    template <typename T>
    static inline Dtype FromType() {
        throw std::runtime_error("Unsupported data for Dtype::FromType.");
    }

    [[nodiscard]] int64_t ByteSize() const { return byte_size_; }

    [[nodiscard]] DtypeCode GetDtypeCode() const { return dtype_code_; }

    [[nodiscard]] bool IsObject() const { return dtype_code_ == DtypeCode::Object; }

    [[nodiscard]] std::string ToString() const { return name_; }

    bool operator==(const Dtype &other) const;

    bool operator!=(const Dtype &other) const;

private:
    static constexpr size_t max_name_len_ = 16;
    DtypeCode dtype_code_;
    int64_t byte_size_;
    char name_[max_name_len_]{};  // MSVC warns if std::string is exported to DLL.
};

OPEN3D_API extern const Dtype Undefined;
OPEN3D_API extern const Dtype Float32;
OPEN3D_API extern const Dtype Float64;
OPEN3D_API extern const Dtype Int8;
OPEN3D_API extern const Dtype Int16;
OPEN3D_API extern const Dtype Int32;
OPEN3D_API extern const Dtype Int64;
OPEN3D_API extern const Dtype UInt8;
OPEN3D_API extern const Dtype UInt16;
OPEN3D_API extern const Dtype UInt32;
OPEN3D_API extern const Dtype UInt64;
OPEN3D_API extern const Dtype Bool;

template <>
inline Dtype Dtype::FromType<float>() {
    return Dtype::Float32;
}

template <>
inline Dtype Dtype::FromType<double>() {
    return Dtype::Float64;
}

template <>
inline Dtype Dtype::FromType<int8_t>() {
    return Dtype::Int8;
}

template <>
inline Dtype Dtype::FromType<int16_t>() {
    return Dtype::Int16;
}

template <>
inline Dtype Dtype::FromType<int32_t>() {
    return Dtype::Int32;
}

template <>
inline Dtype Dtype::FromType<int64_t>() {
    return Dtype::Int64;
}

template <>
inline Dtype Dtype::FromType<uint8_t>() {
    return Dtype::UInt8;
}

template <>
inline Dtype Dtype::FromType<uint16_t>() {
    return Dtype::UInt16;
}

template <>
inline Dtype Dtype::FromType<uint32_t>() {
    return Dtype::UInt32;
}

template <>
inline Dtype Dtype::FromType<uint64_t>() {
    return Dtype::UInt64;
}

template <>
inline Dtype Dtype::FromType<bool>() {
    return Dtype::Bool;
}

}  // namespace vox::core
