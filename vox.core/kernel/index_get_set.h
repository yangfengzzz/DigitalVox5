//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.base/logging.h"
#include "vox.core/tensor.h"

namespace vox {
namespace core {
namespace kernel {

void IndexGet(const Tensor& src,
              Tensor& dst,
              const std::vector<Tensor>& index_tensors,
              const SizeVector& indexed_shape,
              const SizeVector& indexed_strides);

void IndexGetCPU(const Tensor& src,
                 Tensor& dst,
                 const std::vector<Tensor>& index_tensors,
                 const SizeVector& indexed_shape,
                 const SizeVector& indexed_strides);

#ifdef BUILD_CUDA_MODULE
void IndexGetCUDA(const Tensor& src,
                  Tensor& dst,
                  const std::vector<Tensor>& index_tensors,
                  const SizeVector& indexed_shape,
                  const SizeVector& indexed_strides);
#endif

void IndexSet(const Tensor& src,
              Tensor& dst,
              const std::vector<Tensor>& index_tensors,
              const SizeVector& indexed_shape,
              const SizeVector& indexed_strides);

void IndexSetCPU(const Tensor& src,
                 Tensor& dst,
                 const std::vector<Tensor>& index_tensors,
                 const SizeVector& indexed_shape,
                 const SizeVector& indexed_strides);

#ifdef BUILD_CUDA_MODULE
void IndexSetCUDA(const Tensor& src,
                  Tensor& dst,
                  const std::vector<Tensor>& index_tensors,
                  const SizeVector& indexed_shape,
                  const SizeVector& indexed_strides);
#endif

}  // namespace kernel
}  // namespace core
}  // namespace vox
