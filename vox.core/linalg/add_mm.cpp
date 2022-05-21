//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "linalg/add_mm.h"

#include <unordered_map>

namespace vox {
namespace core {

void AddMM(const Tensor& A, const Tensor& B, Tensor& output, double alpha, double beta) {
    AssertTensorDevice(B, A.GetDevice());
    AssertTensorDtype(B, A.GetDtype());
    AssertTensorDevice(output, A.GetDevice());
    AssertTensorDtype(output, A.GetDtype());

    const Device device = A.GetDevice();
    const Dtype dtype = A.GetDtype();

    if (dtype != core::Float32 && dtype != core::Float64) {
        LOGE("AddMM is not implemented for {}.", dtype.ToString());
    }

    // Check shapes
    SizeVector A_shape = A.GetShape();
    SizeVector B_shape = B.GetShape();
    SizeVector output_shape = output.GetShape();

    if (A_shape.size() != 2) {
        LOGE("Tensor A must be 2D, but got {}D.", A_shape.size());
    }
    if (B_shape.size() != 1 && B_shape.size() != 2) {
        LOGE("Tensor B must be 1D (vector) or 2D (matrix), but got {}D.", B_shape.size());
    }
    if (A_shape[1] != B_shape[0]) {
        LOGE("Tensor A columns {} mismatch with Tensor B rows {}.", A_shape[1], B_shape[0]);
    }
    if (output_shape[0] != A_shape[0] && output_shape[1] != B_shape[B_shape.size() - 1]) {
        LOGE("Tensor output must match A rows {} and B columns {}.", A_shape[0], B_shape[B_shape.size() - 1]);
    }

    // Check the memory layout of tensors.
    Tensor A_contiguous, B_contiguous;
    bool transA = false;
    bool transB = false;
    if (A.IsContiguous() || A.T().IsContiguous()) {
        transA = A.T().IsContiguous();
        A_contiguous = A;
    } else {
        A_contiguous = A.Contiguous();
    }

    if (B.IsContiguous() || B.T().IsContiguous()) {
        transB = B.T().IsContiguous();
        B_contiguous = B;
    } else {
        B_contiguous = B.Contiguous();
    }

    // Dispatch to backends
    int64_t m = output.GetShape(0);
    int64_t n = output.GetShape(1);
    int64_t k = A_contiguous.GetShape(1);

    int lda = A_contiguous.GetStride(transA ? 1 : 0);
    int ldb = B_contiguous.GetStride(transB ? 1 : 0);
    int ldc = output.GetStride(0);

    if (m == 0 || k == 0 || n == 0) {
        LOGE("Tensor shapes should not contain dimensions with zero.");
    }

    void* A_data = A_contiguous.To(dtype).GetDataPtr();
    void* B_data = B_contiguous.To(dtype).GetDataPtr();
    void* C_data = output.GetDataPtr();

    if (device.GetType() == Device::DeviceType::CUDA) {
#ifdef BUILD_CUDA_MODULE
        AddMMCUDA(B_data, A_data, C_data, n, k, m, alpha, beta, transB, transA, ldb, lda, ldc, dtype);
#else
        LOGE("Unimplemented device.");
#endif
    } else {
        AddMMCPU(B_data, A_data, C_data, n, k, m, alpha, beta, transB, transA, ldb, lda, ldc, dtype);
    }
};

}  // namespace core
}  // namespace vox
