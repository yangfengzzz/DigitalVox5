//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.
#pragma once

#ifdef WITH_IPPICV
#define IPP_CALL(ipp_function, ...) ipp_function(__VA_ARGS__);

// Required by IPPICV headers, defined here to keep other compile commands clean
#define ICV_BASE
#define IW_BUILD
#include <iw++/iw_core.hpp>

#include "Dtype.h"
#include "Tensor.h"
#include "open3d/t/geometry/Image.h"

namespace vox {
namespace t {
namespace geometry {
namespace ipp {

inline ::ipp::IppDataType ToIppDataType(core::Dtype dtype) {
    if (dtype == core::UInt8 || dtype == core::Bool) {
        return ipp8u;
    } else if (dtype == core::UInt16) {
        return ipp16u;
    } else if (dtype == core::Int16) {
        return ipp16s;
    } else if (dtype == core::Int32) {
        return ipp32s;
    } else if (dtype == core::Int64) {
        return ipp64s;
    } else if (dtype == core::Float32) {
        return ipp32f;
    } else if (dtype == core::Float64) {
        return ipp64f;
    } else {
        return ippUndef;
    }
}

void To(const core::Tensor &src_im, core::Tensor &dst_im, double scale, double offset);

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
}  // namespace ipp
}  // namespace geometry
}  // namespace t
}  // namespace vox

#else
#define IPP_CALL(ipp_function, ...) LOGE("Not built with IPP-IW, cannot call " #ipp_function);
#endif  // WITH_IPPICV
