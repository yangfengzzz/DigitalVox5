//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include "vox.render/error.h"

VKBP_DISABLE_WARNINGS()

#include <spirv_glsl.hpp>

VKBP_ENABLE_WARNINGS()

#include "vox.render/shader/shader_module.h"
#include "vox.render/vk_common.h"

namespace vox {
/// Generate a list of shader resource based on SPIRV reflection code, and provided ShaderVariant
class SpirvReflection {
public:
    /// @brief Reflects shader resources from SPIRV code
    /// @param stage The Vulkan shader stage flag
    /// @param spirv The SPIRV code of shader
    /// @param[out] resources The list of reflected shader resources
    /// @param variant ShaderVariant used for reflection to specify the size of the runtime arrays in Storage Buffers
    static bool ReflectShaderResources(VkShaderStageFlagBits stage,
                                       const std::vector<uint32_t> &spirv,
                                       std::vector<ShaderResource> &resources,
                                       const ShaderVariant &variant);

private:
    static void ParseShaderResources(const spirv_cross::Compiler &compiler,
                                     VkShaderStageFlagBits stage,
                                     std::vector<ShaderResource> &resources,
                                     const ShaderVariant &variant);

    static void ParsePushConstants(const spirv_cross::Compiler &compiler,
                                   VkShaderStageFlagBits stage,
                                   std::vector<ShaderResource> &resources,
                                   const ShaderVariant &variant);

    static void ParseSpecializationConstants(const spirv_cross::Compiler &compiler,
                                             VkShaderStageFlagBits stage,
                                             std::vector<ShaderResource> &resources,
                                             const ShaderVariant &variant);
};

}  // namespace vox
