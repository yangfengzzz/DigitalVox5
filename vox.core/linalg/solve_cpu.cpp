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

#include "linalg/lapack_wrapper.h"
#include "linalg/linalg_utils.h"
#include "linalg/solve.h"

namespace vox {
namespace core {

void SolveCPU(void* A_data, void* B_data, void* ipiv_data, int64_t n, int64_t k, Dtype dtype, const Device& device) {
    DISPATCH_LINALG_DTYPE_TO_TEMPLATE(dtype, [&]() {
        OPEN3D_LAPACK_CHECK(
                gesv_cpu<scalar_t>(LAPACK_COL_MAJOR, n, k, static_cast<scalar_t*>(A_data), n,
                                   static_cast<OPEN3D_CPU_LINALG_INT*>(ipiv_data), static_cast<scalar_t*>(B_data), n),
                "gels failed in SolveCPU");
    });
}

}  // namespace core
}  // namespace vox
