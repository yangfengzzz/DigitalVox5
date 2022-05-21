//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "kernel/index_get_set.h"

#include "dtype.h"
#include "kernel/unary_ew.h"
#include "logging.h"
#include "memory_manager.h"
#include "size_vector.h"
#include "tensor.h"

namespace vox {
namespace core {
namespace kernel {

void IndexGet(const Tensor& src,
              Tensor& dst,
              const std::vector<Tensor>& index_tensors,
              const SizeVector& indexed_shape,
              const SizeVector& indexed_strides) {
    // index_tensors has been preprocessed to be on the same device as src,
    // however, dst may be in a different device.
    if (dst.GetDevice() != src.GetDevice()) {
        Tensor dst_same_device(dst.GetShape(), dst.GetDtype(), src.GetDevice());
        IndexGet(src, dst_same_device, index_tensors, indexed_shape, indexed_strides);
        dst.CopyFrom(dst_same_device);
        return;
    }

    if (src.GetDevice().GetType() == Device::DeviceType::CPU) {
        IndexGetCPU(src, dst, index_tensors, indexed_shape, indexed_strides);
    } else if (src.GetDevice().GetType() == Device::DeviceType::CUDA) {
#ifdef BUILD_CUDA_MODULE
        IndexGetCUDA(src, dst, index_tensors, indexed_shape, indexed_strides);
#endif
    } else {
        LOGE("IndexGet: Unimplemented device");
    }
}

void IndexSet(const Tensor& src,
              Tensor& dst,
              const std::vector<Tensor>& index_tensors,
              const SizeVector& indexed_shape,
              const SizeVector& indexed_strides) {
    // index_tensors has been preprocessed to be on the same device as dst,
    // however, src may be on a different device.
    Tensor src_same_device = src.To(dst.GetDevice());

    if (dst.GetDevice().GetType() == Device::DeviceType::CPU) {
        IndexSetCPU(src_same_device, dst, index_tensors, indexed_shape, indexed_strides);
    } else if (dst.GetDevice().GetType() == Device::DeviceType::CUDA) {
#ifdef BUILD_CUDA_MODULE
        IndexSetCUDA(src_same_device, dst, index_tensors, indexed_shape, indexed_strides);
#endif
    } else {
        LOGE("IndexSet: Unimplemented device");
    }
}

}  // namespace kernel
}  // namespace core
}  // namespace vox
