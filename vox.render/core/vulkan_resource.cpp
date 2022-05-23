//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/core/vulkan_resource.h"

#include "vox.render/core/device.h"

namespace vox::core::detail {
void SetDebugName(const Device *device, VkObjectType object_type, uint64_t handle, const char *debug_name) {
    if (!debug_name || *debug_name == '\0' || !device) {
        // Can't set name, or no point in setting an empty name
        return;
    }

    device->GetDebugUtils().SetDebugName(device->GetHandle(), object_type, handle, debug_name);
}

}  // namespace vox::core::detail
