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

#include <string>
#include <unordered_map>

#include "tensor.h"

namespace vox {
namespace t {
namespace geometry {

/// TensorMap is a unordered_map<string, Tensor> with a primary key. It is
/// typically used as a container for geometry attributes.
///
/// e.g.
/// tensor_map.primary_key: "positions"
/// tensor_map["positions"]  : Tensor of shape {100, 3}.
/// tensor_map["colors"]  : Tensor of shape {100, 3}.
/// tensor_map["normals"] : Tensor of shape {100, 3}.
///
/// Typically, tensors in the TensorMap should have the same length (the first
/// dimension of shape) and device as the primary tensor.
class TensorMap : public std::unordered_map<std::string, core::Tensor> {
public:
    /// Create empty TensorMap and set primary key.
    explicit TensorMap(const std::string& primary_key)
        : std::unordered_map<std::string, core::Tensor>(), primary_key_(primary_key) {}

    /// A primary key is always required. This constructor can be marked as
    /// delete in C++, but it is needed for pybind to bind as a generic python
    /// map interface.
    explicit TensorMap() : TensorMap("Undefined") { LOGE("Please construct TensorMap with a primary key."); }

    template <class InputIt>
    TensorMap(const std::string& primary_key, InputIt first, InputIt last)
        : std::unordered_map<std::string, core::Tensor>(first, last), primary_key_(primary_key) {
        AssertPrimaryKeyInMapOrEmpty();
    }

    TensorMap(const std::string& primary_key, const std::unordered_map<std::string, core::Tensor>& tensor_map)
        : TensorMap(primary_key, tensor_map.begin(), tensor_map.end()) {}

    TensorMap(const std::string& primary_key, std::initializer_list<value_type> init)
        : std::unordered_map<std::string, core::Tensor>(init), primary_key_(primary_key) {
        AssertPrimaryKeyInMapOrEmpty();
    }

    /// Copy constructor performs a "shallow" copy of the Tensors.
    TensorMap(const TensorMap& other)
        : std::unordered_map<std::string, core::Tensor>(other), primary_key_(other.primary_key_) {
        AssertPrimaryKeyInMapOrEmpty();
    }

    /// Move constructor performs a "shallow" copy of the Tensors.
    TensorMap(TensorMap&& other)
        : std::unordered_map<std::string, core::Tensor>(other), primary_key_(other.primary_key_) {
        AssertPrimaryKeyInMapOrEmpty();
    }

    /// \brief Erase elements for the TensorMap by key value, if the key
    /// exists. If the key does not exists, a warning is thrown.
    /// Also `primary_key` cannot be deleted. It is based on
    /// `size_type unordered_map::erase(const key_type& k);`.
    /// \return The number of elements deleted. [0 if key was not present].
    std::size_t Erase(const std::string key) {
        if (key == primary_key_) {
            LOGE("Primary key: {} cannot be deleted.", primary_key_);
        } else if (!Contains(key)) {
            LOGW("Key: {} is not present.", key);
        }
        return this->erase(key);
    }

    TensorMap& operator=(const TensorMap&) = default;

    TensorMap& operator=(TensorMap&&) = default;

    /// Returns the primary key of the TensorMap.
    std::string GetPrimaryKey() const { return primary_key_; }

    /// Returns true if all tensors in the map have the same size.
    bool IsSizeSynchronized() const;

    /// Assert IsSizeSynchronized().
    void AssertSizeSynchronized() const;

    /// Returns True if the underlying memory buffers of all the Tensors in the
    /// TensorMap is contiguous.
    bool IsContiguous() const;

    /// Returns a new contiguous TensorMap containing the same data in the same
    /// device. For the contiguous tensors in the TensorMap, the same underlying
    /// memory will be used.
    TensorMap Contiguous() const;

    /// Returns true if the key exists in the map.
    /// Same as C++20's std::unordered_map::contains().
    bool Contains(const std::string& key) const { return count(key) != 0; }

private:
    /// Asserts that the map indeed contains the primary_key. This is typically
    /// called in constructors.
    void AssertPrimaryKeyInMapOrEmpty() const;

    /// Returns the size (length) of the primary key's tensor.
    int64_t GetPrimarySize() const { return at(primary_key_).GetLength(); }

    /// Returns the device of the primary key's tensor.
    core::Device GetPrimaryDevice() const { return at(primary_key_).GetDevice(); }

    /// Primary key of the TensorMap.
    std::string primary_key_;
};

}  // namespace geometry
}  // namespace t
}  // namespace vox
