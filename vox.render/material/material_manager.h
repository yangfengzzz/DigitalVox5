//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "material.h"
#include "singleton.h"

namespace vox {
class MaterialManager: public Singleton<MaterialManager> {
public:
    static MaterialManager &get_singleton();
    
    static MaterialManager *get_singleton_ptr();
    
    explicit MaterialManager(Device& device);

    core::Sampler* get_sampler(const VkSamplerCreateInfo& info);

    VkSamplerCreateInfo last_sampler_create_info_;
    
public:
    
private:
    /** sampler pool*/
    std::unordered_map<VkSamplerCreateInfo, std::unique_ptr<core::Sampler>> sampler_pool_;
    
    Device& device_;
};

template<> inline MaterialManager *Singleton<MaterialManager>::ms_singleton_{nullptr};

}
