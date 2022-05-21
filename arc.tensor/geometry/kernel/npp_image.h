//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.
#pragma once

#ifdef BUILD_CUDA_MODULE
#include "dtype.h"
#include "geometry/image.h"
#include "tensor.h"

namespace vox {
namespace t {
namespace geometry {
namespace npp {

void RGBToGray(const core::Tensor &src_im, core::Tensor &dst_im);

void Dilate(const arc::core::Tensor &srcim, arc::core::Tensor &dstim, int kernel_size);

void Resize(const arc::core::Tensor &srcim, arc::core::Tensor &dstim, t::geometry::Image::InterpType interp_type);

void Filter(const arc::core::Tensor &srcim, arc::core::Tensor &dstim, const arc::core::Tensor &kernel);

void FilterBilateral(const arc::core::Tensor &srcim,
                     arc::core::Tensor &dstim,
                     int kernel_size,
                     float value_sigma,
                     float distance_sigma);

void FilterGaussian(const arc::core::Tensor &srcim, arc::core::Tensor &dstim, int kernel_size, float sigma);

void FilterSobel(const arc::core::Tensor &srcim,
                 arc::core::Tensor &dstim_dx,
                 arc::core::Tensor &dstim_dy,
                 int kernel_size);
}  // namespace npp
}  // namespace geometry
}  // namespace t
}  // namespace vox

#endif  // BUILD_CUDA_MODULE
