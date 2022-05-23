//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.core/linalg/inverse.h"
#include "vox.core/linalg/lapack_wrapper.h"
#include "vox.core/linalg/linalg_utils.h"

namespace vox {
namespace core {

void InverseCPU(void* A_data, void* ipiv_data, void* output_data, int64_t n, Dtype dtype, const Device& device) {
    DISPATCH_LINALG_DTYPE_TO_TEMPLATE(dtype, [&]() {
        OPEN3D_LAPACK_CHECK(getrf_cpu<scalar_t>(LAPACK_COL_MAJOR, n, n, static_cast<scalar_t*>(A_data), n,
                                                static_cast<OPEN3D_CPU_LINALG_INT*>(ipiv_data)),
                            "getrf failed in InverseCPU");
        OPEN3D_LAPACK_CHECK(getri_cpu<scalar_t>(LAPACK_COL_MAJOR, n, static_cast<scalar_t*>(A_data), n,
                                                static_cast<OPEN3D_CPU_LINALG_INT*>(ipiv_data)),
                            "getri failed in InverseCPU");
    });
}

}  // namespace core
}  // namespace vox
