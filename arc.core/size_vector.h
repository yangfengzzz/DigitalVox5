// ----------------------------------------------------------------------------
// -                        Open3D: www.open3d.org                            -
// ----------------------------------------------------------------------------
// The MIT License (MIT)
//
// Copyright (c) 2018-2021 www.open3d.org
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
// ----------------------------------------------------------------------------

#pragma once

#include <cstddef>
#include <optional>
#include <string>
#include <vector>

namespace arc::core {

class SizeVector;

/// DynamicSizeVector is a vector of optional<int64_t>, it is used to represent
/// a shape with unknown (dynamic) dimensions.
///
/// Example: create a shape of (None, 3)
/// ```
/// core::DynamicSizeVector shape{std::nullopt, 3};
/// ```
class DynamicSizeVector : public std::vector<std::optional<int64_t>> {
public:
    DynamicSizeVector() = default;

    DynamicSizeVector(const std::initializer_list<std::optional<int64_t>>& dim_sizes);

    explicit DynamicSizeVector(const std::vector<std::optional<int64_t>>& dim_sizes);

    DynamicSizeVector(const DynamicSizeVector& other);

    explicit DynamicSizeVector(int64_t n, int64_t initial_value = 0);

    template <class InputIterator>
    DynamicSizeVector(InputIterator first, InputIterator last) : std::vector<std::optional<int64_t>>(first, last) {}

    explicit DynamicSizeVector(const SizeVector& dim_sizes);

    [[nodiscard]] SizeVector ToSizeVector() const;

    DynamicSizeVector& operator=(const DynamicSizeVector& v);

    DynamicSizeVector& operator=(DynamicSizeVector&& v) noexcept;

    [[nodiscard]] std::string ToString() const;

    [[nodiscard]] bool IsDynamic() const;
};

/// SizeVector is a vector of int64_t, typically used in Tensor shape and
/// strides. A signed int64_t type is chosen to allow negative strides.
class SizeVector : public std::vector<int64_t> {
public:
    SizeVector() = default;

    SizeVector(const std::initializer_list<int64_t>& dim_sizes);

    SizeVector(const std::vector<int64_t>& dim_sizes);

    SizeVector(const SizeVector& other);

    explicit SizeVector(int64_t n, int64_t initial_value = 0);

    template <class InputIterator>
    SizeVector(InputIterator first, InputIterator last) : std::vector<int64_t>(first, last) {}

    SizeVector& operator=(const SizeVector& v);

    SizeVector& operator=(SizeVector&& v) noexcept;

    [[nodiscard]] int64_t NumElements() const;

    [[nodiscard]] int64_t GetLength() const;

    [[nodiscard]] std::string ToString() const;

    void AssertCompatible(const DynamicSizeVector& dsv, const std::string& msg = "") const;

    [[nodiscard]] bool IsCompatible(const DynamicSizeVector& dsv) const;
};

}  // namespace arc::core
