//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.core/kernel/binary_ew.h"

#include <vector>

#include "vox.base/logging.h"
#include "vox.core/shape_util.h"
#include "vox.core/tensor.h"

namespace vox {
namespace core {
namespace kernel {

const std::unordered_set<BinaryEWOpCode, utility::hash_enum_class> s_boolean_binary_ew_op_codes{
        BinaryEWOpCode::LogicalAnd, BinaryEWOpCode::LogicalOr, BinaryEWOpCode::LogicalXor,
        BinaryEWOpCode::Gt,         BinaryEWOpCode::Lt,        BinaryEWOpCode::Ge,
        BinaryEWOpCode::Le,         BinaryEWOpCode::Eq,        BinaryEWOpCode::Ne,
};

void BinaryEW(const Tensor& lhs, const Tensor& rhs, Tensor& dst, BinaryEWOpCode op_code) {
    // lhs, rhs and dst must be on the same device.
    for (auto device : std::vector<Device>({rhs.GetDevice(), dst.GetDevice()})) {
        if (lhs.GetDevice() != device) {
            LOGE("Device mismatch {} != {}.", lhs.GetDevice().ToString(), device.ToString());
        }
    }

    // broadcast(lhs.shape, rhs.shape) must be dst.shape.
    const SizeVector broadcasted_input_shape = shape_util::BroadcastedShape(lhs.GetShape(), rhs.GetShape());
    if (broadcasted_input_shape != dst.GetShape()) {
        LOGE("The broadcasted input shape {} does not match the output "
             "shape {}.",
             broadcasted_input_shape.ToString(), dst.GetShape().ToString());
    }

    Device::DeviceType device_type = lhs.GetDevice().GetType();
    if (device_type == Device::DeviceType::CPU) {
        BinaryEWCPU(lhs, rhs, dst, op_code);
    } else if (device_type == Device::DeviceType::CUDA) {
#ifdef BUILD_CUDA_MODULE
        BinaryEWCUDA(lhs, rhs, dst, op_code);
#else
        LOGE("Not compiled with CUDA, but CUDA device is used.");
#endif
    } else {
        LOGE("BinaryEW: Unimplemented device");
    }
}

}  // namespace kernel
}  // namespace core
}  // namespace vox
