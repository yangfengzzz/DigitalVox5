//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.core/size_vector.h"

#include <spdlog/fmt/fmt.h>

#include <algorithm>
#include <cstddef>
#include <numeric>
#include <optional>
#include <sstream>
#include <vector>

#include "vox.base/logging.h"

namespace vox::core {

DynamicSizeVector::DynamicSizeVector(const std::initializer_list<std::optional<int64_t>>& dim_sizes)
    : std::vector<std::optional<int64_t>>(dim_sizes) {}

DynamicSizeVector::DynamicSizeVector(const std::vector<std::optional<int64_t>>& dim_sizes)
    : std::vector<std::optional<int64_t>>(dim_sizes) {}

DynamicSizeVector::DynamicSizeVector(const DynamicSizeVector& other) = default;

DynamicSizeVector::DynamicSizeVector(int64_t n, int64_t initial_value)
    : std::vector<std::optional<int64_t>>(n, initial_value) {}

DynamicSizeVector::DynamicSizeVector(const SizeVector& dim_sizes)
    : DynamicSizeVector(dim_sizes.begin(), dim_sizes.end()) {}

SizeVector DynamicSizeVector::ToSizeVector() const {
    SizeVector sv(size());
    std::transform(begin(), end(), sv.begin(), [](const auto& v) {
        if (!v.has_value()) {
            LOGE("Cannot convert dynamic shape to SizeVector.")
        }
        return v.value();
    });
    return sv;
}

DynamicSizeVector& DynamicSizeVector::operator=(const DynamicSizeVector& v) {
    static_cast<std::vector<std::optional<int64_t>>*>(this)->operator=(v);
    return *this;
}

DynamicSizeVector& DynamicSizeVector::operator=(DynamicSizeVector&& v) noexcept {
    static_cast<std::vector<std::optional<int64_t>>*>(this)->operator=(v);
    return *this;
}

std::string DynamicSizeVector::ToString() const {
    std::stringstream ss;
    ss << "{";
    bool first = true;
    for (const std::optional<int64_t>& element : *this) {
        if (first) {
            first = false;
        } else {
            ss << ", ";
        }
        if (element.has_value()) {
            ss << fmt::format("{}", element.value());
        } else {
            ss << "None";
        }
    }
    ss << "}";
    return ss.str();
}

bool DynamicSizeVector::IsDynamic() const {
    return std::any_of(this->begin(), this->end(), [](const std::optional<int64_t>& v) { return !v.has_value(); });
}

SizeVector::SizeVector(const std::initializer_list<int64_t>& dim_sizes) : std::vector<int64_t>(dim_sizes) {}

SizeVector::SizeVector(const std::vector<int64_t>& dim_sizes) : std::vector<int64_t>(dim_sizes) {}

SizeVector::SizeVector(const SizeVector& other) = default;

SizeVector::SizeVector(int64_t n, int64_t initial_value) : std::vector<int64_t>(n, initial_value) {}

SizeVector& SizeVector::operator=(const SizeVector& v) {
    static_cast<std::vector<int64_t>*>(this)->operator=(v);
    return *this;
}

SizeVector& SizeVector::operator=(SizeVector&& v) noexcept {
    static_cast<std::vector<int64_t>*>(this)->operator=(v);
    return *this;
}

int64_t SizeVector::NumElements() const {
    if (this->empty()) {
        return 1;
    }
    return std::accumulate(this->begin(), this->end(), 1LL, [this](const int64_t& lhs, const int64_t& rhs) -> int64_t {
        if (lhs < 0 || rhs < 0) {
            LOGE("Shape {} cannot contain negative dimensions.", this->ToString())
        }
        return std::multiplies<int64_t>()(lhs, rhs);
    });
}

int64_t SizeVector::GetLength() const {
    if (empty()) {
        throw std::runtime_error("Cannot get length of a 0-dimensional shape.");
    } else {
        return operator[](0);
    }
}

std::string SizeVector::ToString() const { return fmt::format("{}", this->ToString()); }

void SizeVector::AssertCompatible(const DynamicSizeVector& dsv, const std::string& msg) const {
    if (!IsCompatible(dsv)) {
        if (msg.empty()) {
            LOGE("Shape {} is not compatible with {}.", ToString(), dsv.ToString())
        } else {
            LOGE("Shape {} is not compatible with {}: {}", ToString(), dsv.ToString(), msg)
        }
    }
}

bool SizeVector::IsCompatible(const DynamicSizeVector& dsv) const {
    if (size() != dsv.size()) {
        return false;
    }
    for (size_t i = 0; i < size(); ++i) {
        if (dsv[i].has_value() && dsv[i].value() != at(i)) {
            return false;
        }
    }
    return true;
}

}  // namespace vox::core
