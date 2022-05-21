//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

// Private header. Do not include in Open3d.h.

#pragma once

#include "linalg/lu.h"

namespace vox {
namespace core {

void LUCPU(void* A_data, void* ipiv_data, int64_t rows, int64_t cols, Dtype dtype, const Device& device);

#ifdef BUILD_CUDA_MODULE
void LUCUDA(void* A_data, void* ipiv_data, int64_t rows, int64_t cols, Dtype dtype, const Device& device);
#endif
}  // namespace core
}  // namespace vox
