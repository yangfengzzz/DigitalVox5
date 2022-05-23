//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <unordered_set>

#include "vox.base/helper.h"
#include "vox.base/logging.h"
#include "vox.core/tensor.h"

namespace vox {
namespace core {
namespace kernel {

enum class BinaryEWOpCode {
    Add,
    Sub,
    Mul,
    Div,
    LogicalAnd,
    LogicalOr,
    LogicalXor,
    Gt,
    Lt,
    Ge,
    Le,
    Eq,
    Ne,
};

extern const std::unordered_set<BinaryEWOpCode, utility::hash_enum_class> s_boolean_binary_ew_op_codes;

void BinaryEW(const Tensor& lhs, const Tensor& rhs, Tensor& dst, BinaryEWOpCode op_code);

void BinaryEWCPU(const Tensor& lhs, const Tensor& rhs, Tensor& dst, BinaryEWOpCode op_code);

#ifdef BUILD_CUDA_MODULE
void BinaryEWCUDA(const Tensor& lhs, const Tensor& rhs, Tensor& dst, BinaryEWOpCode op_code);
#endif

inline void Add(const Tensor& lhs, const Tensor& rhs, Tensor& dst) { BinaryEW(lhs, rhs, dst, BinaryEWOpCode::Add); }

inline void Sub(const Tensor& lhs, const Tensor& rhs, Tensor& dst) { BinaryEW(lhs, rhs, dst, BinaryEWOpCode::Sub); }

inline void Mul(const Tensor& lhs, const Tensor& rhs, Tensor& dst) { BinaryEW(lhs, rhs, dst, BinaryEWOpCode::Mul); }

inline void Div(const Tensor& lhs, const Tensor& rhs, Tensor& dst) { BinaryEW(lhs, rhs, dst, BinaryEWOpCode::Div); }

}  // namespace kernel
}  // namespace core
}  // namespace vox
