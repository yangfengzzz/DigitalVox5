//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "material.h"

#include <utility>

namespace vox {
Material::Material(Device &device, std::string name) :
	shader_data_(device),
	name_{std::move(name)},
	get_sampler_([&](const VkSamplerCreateInfo& info)->core::Sampler* {
    auto iter = sampler_pool_.find(info);
    if (iter != sampler_pool_.end()) {
        return &iter->second;
    } else {
        auto pair = std::make_pair(info, core::Sampler(device, info));
        auto sampler = &pair.second;
        sampler_pool_.insert(std::move(pair));
        return sampler;
    }
}){
    
}

}        // namespace vox
