//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "tensor.h"

namespace vox {
namespace t {
namespace geometry {
namespace kernel {
namespace image {

void To(const core::Tensor &src, core::Tensor &dst, double scale, double offset);

void ClipTransform(const core::Tensor &src,
                   core::Tensor &dst,
                   float scale,
                   float min_value,
                   float max_value,
                   float clip_fill = 0.0f);

void PyrDownDepth(const core::Tensor &src, core::Tensor &dst, float diff_threshold, float invalid_fill);

void CreateVertexMap(const core::Tensor &src, core::Tensor &dst, const core::Tensor &intrinsics, float invalid_fill);

void CreateNormalMap(const core::Tensor &src, core::Tensor &dst, float invalid_fill);

void ColorizeDepth(const core::Tensor &src, core::Tensor &dst, float scale, float min_value, float max_value);

void ToCPU(const core::Tensor &src, core::Tensor &dst, double scale, double offset);

void ClipTransformCPU(const core::Tensor &src,
                      core::Tensor &dst,
                      float scale,
                      float min_value,
                      float max_value,
                      float clip_fill = 0.0f);

void PyrDownDepthCPU(const core::Tensor &src, core::Tensor &dst, float diff_threshold, float invalid_fill);

void CreateVertexMapCPU(const core::Tensor &src, core::Tensor &dst, const core::Tensor &intrinsics, float invalid_fill);

void CreateNormalMapCPU(const core::Tensor &src, core::Tensor &dst, float invalid_fill);

void ColorizeDepthCPU(const core::Tensor &src, core::Tensor &dst, float scale, float min_value, float max_value);

#ifdef BUILD_CUDA_MODULE
void ToCUDA(const core::Tensor &src, core::Tensor &dst, double scale, double offset);

void ClipTransformCUDA(const core::Tensor &src,
                       core::Tensor &dst,
                       float scale,
                       float min_value,
                       float max_value,
                       float clip_fill = 0.0f);

void PyrDownDepthCUDA(const core::Tensor &src, core::Tensor &dst, float diff_threshold, float invalid_fill);

void CreateVertexMapCUDA(const core::Tensor &src,
                         core::Tensor &dst,
                         const core::Tensor &intrinsics,
                         float invalid_fill);

void CreateNormalMapCUDA(const core::Tensor &src, core::Tensor &dst, float invalid_fill);

void ColorizeDepthCUDA(const core::Tensor &src, core::Tensor &dst, float scale, float min_value, float max_value);

#endif
}  // namespace image
}  // namespace kernel
}  // namespace geometry
}  // namespace t
}  // namespace vox
