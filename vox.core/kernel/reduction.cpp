//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "kernel/reduction.h"

#include "size_vector.h"

namespace vox {
namespace core {
namespace kernel {

void Reduction(const Tensor& src, Tensor& dst, const SizeVector& dims, bool keepdim, ReductionOpCode op_code) {
    // For ArgMin and ArgMax, keepdim == false, and dims can only contain one or
    // all dimensions.
    if (s_arg_reduce_ops.find(op_code) != s_arg_reduce_ops.end()) {
        if (keepdim) {
            LOGE("Arg-reduction keepdim must be false");
        }
        if (dims.size() != 1) {
            std::vector<bool> seen_dims(src.NumDims(), false);
            for (const int64_t& dim : dims) {
                seen_dims[dim] = true;
            }
            if (!std::all_of(seen_dims.begin(), seen_dims.end(), [](bool seen) { return seen; })) {
                LOGE("Arg-reduction can only have 1 or all reduction "
                     "dimensions. However, dims = {}.",
                     dims.ToString());
            }
        }
    }

    SizeVector keepdim_shape = shape_util::ReductionShape(src.GetShape(), dims, true);
    SizeVector non_keepdim_shape = shape_util::ReductionShape(src.GetShape(), dims, false);
    if (keepdim && keepdim_shape != dst.GetShape()) {
        LOGE("Expected output shape {} but got {}.", keepdim_shape.ToString(), dst.GetShape().ToString());
    }
    if (!keepdim && non_keepdim_shape != dst.GetShape()) {
        LOGE("Expected output shape {} but got {}.", keepdim_shape.ToString(), dst.GetShape().ToString());
    }

    // Directly copy for non-reduction.
    if (dims.size() == 0) {
        dst.AsRvalue() = src;
        return;
    }

    // Always reshape to keepdim case. This reshaping is copy-free.
    if (!keepdim) {
        dst = dst.Reshape(keepdim_shape);
    }

    if (src.GetDevice() != dst.GetDevice()) {
        LOGE("Device mismatch {} != {}.", src.GetDevice().ToString(), dst.GetDevice().ToString());
    }

    Device::DeviceType device_type = src.GetDevice().GetType();
    if (device_type == Device::DeviceType::CPU) {
        ReductionCPU(src, dst, dims, keepdim, op_code);
    } else if (device_type == Device::DeviceType::CUDA) {
#ifdef BUILD_CUDA_MODULE
        ReductionCUDA(src, dst, dims, keepdim, op_code);
#else
        LOGE("Not compiled with CUDA, but CUDA device is used.");
#endif
    } else {
        LOGE("Unimplemented device.");
    }

    if (!keepdim) {
        dst = dst.Reshape(non_keepdim_shape);
    }
}

}  // namespace kernel
}  // namespace core
}  // namespace vox
