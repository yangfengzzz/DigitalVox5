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

#pragma once

#include "tensor.h"
#include "tensor_check.h"

namespace vox {
namespace t {
namespace geometry {

inline void CheckDepthTensor(const core::Tensor& depth) {
    if (depth.NumElements() == 0) {
        LOGE("Input depth is empty.");
    }
    if (depth.GetDtype() != core::UInt16 && depth.GetDtype() != core::Float32) {
        LOGE("Unsupported depth image dtype {}.", depth.GetDtype().ToString());
    }
}

inline void CheckColorTensor(const core::Tensor& color) {
    if (color.NumElements() == 0) {
        LOGE("Input color is empty.");
    }
    if (color.GetDtype() != core::UInt8 && color.GetDtype() != core::Float32) {
        LOGE("Unsupported color image dtype {}.", color.GetDtype().ToString());
    }
}

inline void CheckIntrinsicTensor(const core::Tensor& intrinsic) {
    if (intrinsic.GetShape() != core::SizeVector{3, 3}) {
        LOGE("Unsupported intrinsic matrix shape {}", intrinsic.GetShape().ToString());
    }

    if (intrinsic.GetDtype() != core::Dtype::Float64) {
        LOGE("Unsupported intrinsic matrix dtype {}", intrinsic.GetDtype().ToString());
    }

    if (!intrinsic.IsContiguous()) {
        LOGE("Intrinsic matrix must be contiguous.");
    }
}

inline void CheckExtrinsicTensor(const core::Tensor& extrinsic) {
    if (extrinsic.GetShape() != core::SizeVector{4, 4}) {
        LOGE("Unsupported extrinsic matrix shape {}", extrinsic.GetShape().ToString());
    }

    if (extrinsic.GetDtype() != core::Dtype::Float64) {
        LOGE("Unsupported extrinsic matrix dtype {}", extrinsic.GetDtype().ToString());
    }

    if (!extrinsic.IsContiguous()) {
        LOGE("Extrinsic matrix must be contiguous.");
    }
}

inline void CheckBlockCoorinates(const core::Tensor& block_coords) {
    if (block_coords.GetDtype() != core::Dtype::Int32) {
        LOGE("Unsupported block coordinate dtype {}", block_coords.GetDtype().ToString());
    }
}

/// TODO(wei): find a proper place for such functionalities
inline core::Tensor InverseTransformation(const core::Tensor& T) {
    core::AssertTensorShape(T, {4, 4});
    core::AssertTensorDtype(T, core::Float64);
    core::AssertTensorDevice(T, core::Device("CPU:0"));
    if (!T.IsContiguous()) {
        LOGE("T is expected to be contiguous");
    }

    core::Tensor Tinv({4, 4}, core::Float64, core::Device("CPU:0"));
    const double* T_ptr = T.GetDataPtr<double>();
    double* Tinv_ptr = Tinv.GetDataPtr<double>();

    // R' = R.T
    Tinv_ptr[0 * 4 + 0] = T_ptr[0 * 4 + 0];
    Tinv_ptr[0 * 4 + 1] = T_ptr[1 * 4 + 0];
    Tinv_ptr[0 * 4 + 2] = T_ptr[2 * 4 + 0];

    Tinv_ptr[1 * 4 + 0] = T_ptr[0 * 4 + 1];
    Tinv_ptr[1 * 4 + 1] = T_ptr[1 * 4 + 1];
    Tinv_ptr[1 * 4 + 2] = T_ptr[2 * 4 + 1];

    Tinv_ptr[2 * 4 + 0] = T_ptr[0 * 4 + 2];
    Tinv_ptr[2 * 4 + 1] = T_ptr[1 * 4 + 2];
    Tinv_ptr[2 * 4 + 2] = T_ptr[2 * 4 + 2];

    // t' = -R.T @ t = -R' @ t
    Tinv_ptr[0 * 4 + 3] = -(Tinv_ptr[0 * 4 + 0] * T_ptr[0 * 4 + 3] + Tinv_ptr[0 * 4 + 1] * T_ptr[1 * 4 + 3] +
                            Tinv_ptr[0 * 4 + 2] * T_ptr[2 * 4 + 3]);
    Tinv_ptr[1 * 4 + 3] = -(Tinv_ptr[1 * 4 + 0] * T_ptr[0 * 4 + 3] + Tinv_ptr[1 * 4 + 1] * T_ptr[1 * 4 + 3] +
                            Tinv_ptr[1 * 4 + 2] * T_ptr[2 * 4 + 3]);
    Tinv_ptr[2 * 4 + 3] = -(Tinv_ptr[2 * 4 + 0] * T_ptr[0 * 4 + 3] + Tinv_ptr[2 * 4 + 1] * T_ptr[1 * 4 + 3] +
                            Tinv_ptr[2 * 4 + 2] * T_ptr[2 * 4 + 3]);

    // Remaining part
    Tinv_ptr[3 * 4 + 0] = 0;
    Tinv_ptr[3 * 4 + 1] = 0;
    Tinv_ptr[3 * 4 + 2] = 0;
    Tinv_ptr[3 * 4 + 3] = 1;

    return Tinv;
}
}  // namespace geometry
}  // namespace t
}  // namespace vox
