//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.core/tensor_check.h"

#include <string>

#include "vox.base/helper.h"
#include "vox.base/logging.h"
#include "vox.core/device.h"
#include "vox.core/dtype.h"
#include "vox.core/tensor.h"

namespace vox {
namespace core {
namespace tensor_check {

void AssertTensorDtype_(const char* file, int line, const char* function, const Tensor& tensor, const Dtype& dtype) {
    if (tensor.GetDtype() == dtype) {
        return;
    }
    std::string error_message = fmt::format("Tensor has dtype {}, but is expected to have {}.",
                                            tensor.GetDtype().ToString(), dtype.ToString());
    LOGE(file, line, function, error_message.c_str());
}

void AssertTensorDtypes_(
        const char* file, int line, const char* function, const Tensor& tensor, const std::vector<Dtype>& dtypes) {
    for (auto& it : dtypes) {
        if (tensor.GetDtype() == it) {
            return;
        }
    }

    std::vector<std::string> dtype_strings;
    for (const Dtype& dtype : dtypes) {
        dtype_strings.push_back(dtype.ToString());
    }
    std::string error_message = fmt::format("Tensor has dtype {}, but is expected to have dtype among {{{}}}.",
                                            tensor.GetDtype().ToString(), utility::JoinStrings(dtype_strings));
    LOGE(file, line, function, error_message.c_str());
}

void AssertTensorDevice_(const char* file, int line, const char* function, const Tensor& tensor, const Device& device) {
    if (tensor.GetDevice() == device) {
        return;
    }
    std::string error_message = fmt::format("Tensor has device {}, but is expected to have {}.",
                                            tensor.GetDevice().ToString(), device.ToString());
    LOGE(file, line, function, error_message.c_str());
}

void AssertTensorShape_(
        const char* file, int line, const char* function, const Tensor& tensor, const DynamicSizeVector& shape) {
    if (shape.IsDynamic()) {
        if (tensor.GetShape().IsCompatible(shape)) {
            return;
        }
        std::string error_message = fmt::format(
                "Tensor has shape {}, but is expected to have compatible with "
                "{}.",
                tensor.GetShape().ToString(), shape.ToString());
        LOGE(file, line, function, error_message.c_str());
    } else {
        SizeVector static_shape = shape.ToSizeVector();
        if (tensor.GetShape() == static_shape) {
            return;
        }
        std::string error_message = fmt::format("Tensor has shape {}, but is expected to have {}.",
                                                tensor.GetShape().ToString(), static_shape.ToString());
        LOGE(file, line, function, error_message.c_str());
    }
}

}  // namespace tensor_check
}  // namespace core
}  // namespace vox
