//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "material.h"

#include <utility>

namespace vox {
std::unordered_map<VkSamplerCreateInfo, std::unique_ptr<core::Sampler>> Material::sampler_pool_{};

Material::Material(Device &device, std::string name) :
shader_data_(device),
name_{std::move(name)} {
}

}        // namespace vox
