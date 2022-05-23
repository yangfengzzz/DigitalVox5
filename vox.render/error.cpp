//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/error.h"

namespace vox {
VulkanException::VulkanException(const VkResult result, const std::string &msg)
    : result_{result}, std::runtime_error{msg} {
    error_message_ = std::string(std::runtime_error::what()) + std::string{" : "} + ToString(result);
}

const char *VulkanException::what() const noexcept { return error_message_.c_str(); }

}  // namespace vox
