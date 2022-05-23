//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.core/linalg/det.h"

#include "vox.core/linalg/kernel/matrix.h"
#include "vox.core/linalg/lu.h"

namespace vox {
namespace core {

double Det(const Tensor& A) {
    AssertTensorDtypes(A, {Float32, Float64});
    const Dtype dtype = A.GetDtype();

    double det = 1.0;

    if (A.GetShape() == vox::core::SizeVector({3, 3})) {
        DISPATCH_FLOAT_DTYPE_TO_TEMPLATE(dtype, [&]() {
            core::Tensor A_3x3 = A.To(core::Device("CPU:0"), false).Contiguous();
            const scalar_t* A_3x3_ptr = A_3x3.GetDataPtr<scalar_t>();
            det = static_cast<double>(linalg::kernel::det3x3(A_3x3_ptr));
        });
    } else if (A.GetShape() == vox::core::SizeVector({2, 2})) {
        DISPATCH_FLOAT_DTYPE_TO_TEMPLATE(dtype, [&]() {
            core::Tensor A_2x2 = A.To(core::Device("CPU:0"), false).Contiguous();
            const scalar_t* A_2x2_ptr = A_2x2.GetDataPtr<scalar_t>();
            det = static_cast<double>(linalg::kernel::det2x2(A_2x2_ptr));
        });
    } else {
        Tensor ipiv, output;
        LUIpiv(A, ipiv, output);
        // Sequential loop to compute determinant from LU output, is more
        // efficient on CPU.
        Tensor output_cpu = output.To(core::Device("CPU:0"));
        Tensor ipiv_cpu = ipiv.To(core::Device("CPU:0"));
        int n = A.GetShape()[0];

        DISPATCH_FLOAT_DTYPE_TO_TEMPLATE(dtype, [&]() {
            scalar_t* output_ptr = output_cpu.GetDataPtr<scalar_t>();
            int* ipiv_ptr = static_cast<int*>(ipiv_cpu.GetDataPtr());

            for (int i = 0; i < n; i++) {
                det *= output_ptr[i * n + i];
                if (ipiv_ptr[i] != i) {
                    det *= -1;
                }
            }
        });
    }
    return det;
}

}  // namespace core
}  // namespace vox
