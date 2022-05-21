//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "tensor_map.h"

#include <spdlog/fmt/fmt.h>

#include <sstream>
#include <string>
#include <unordered_map>

#include "logging.h"

namespace vox {
namespace t {
namespace geometry {

bool TensorMap::IsSizeSynchronized() const {
    const int64_t primary_size = GetPrimarySize();
    for (auto& kv : *this) {
        if (kv.second.GetLength() != primary_size) {
            return false;
        }
    }
    return true;
}

void TensorMap::AssertPrimaryKeyInMapOrEmpty() const {
    if (this->size() != 0 && this->count(primary_key_) == 0) {
        LOGE("TensorMap does not contain primary key \"{}\".", primary_key_);
    }
}

void TensorMap::AssertSizeSynchronized() const {
    if (!IsSizeSynchronized()) {
        const int64_t primary_size = GetPrimarySize();
        std::stringstream ss;
        ss << fmt::format("Primary Tensor \"{}\" has size {}, however: \n", primary_key_, primary_size);
        for (auto& kv : *this) {
            if (kv.first != primary_key_ && kv.second.GetLength() != primary_size) {
                fmt::format("    > Tensor \"{}\" has size {}.\n", kv.first, kv.second.GetLength());
            }
        }
        LOGE("{}", ss.str());
    }
}

bool TensorMap::IsContiguous() const {
    for (const auto& kv : *this) {
        if (!kv.second.IsContiguous()) {
            return false;
        }
    }
    return true;
}

TensorMap TensorMap::Contiguous() const {
    TensorMap tensor_map_contiguous(GetPrimaryKey());
    for (const auto& kv : *this) {
        // If the tensor is contiguous, the underlying memory is used.
        tensor_map_contiguous[kv.first] = kv.second.Contiguous();
    }
    return tensor_map_contiguous;
}

}  // namespace geometry
}  // namespace t
}  // namespace vox
