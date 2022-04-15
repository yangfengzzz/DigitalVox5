//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "core/vulkan_resource.h"

#include "core/device.h"

namespace vox::core::detail {
void set_debug_name(const Device *device, VkObjectType object_type, uint64_t handle, const char *debug_name) {
    if (!debug_name || *debug_name == '\0' || !device) {
        // Can't set name, or no point in setting an empty name
        return;
    }
    
    device->get_debug_utils().set_debug_name(device->get_handle(), object_type, handle, debug_name);
}

}        // namespace vox
