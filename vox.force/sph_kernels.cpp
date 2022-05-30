//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "sph_kernels.h"

namespace vox::force {
Real CubicKernel::m_radius;
Real CubicKernel::m_k;
Real CubicKernel::m_l;
Real CubicKernel::m_W_zero;
}  // namespace vox::force