//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <memory>
#include <string>
#include <typeinfo>
#include <vector>

#include "core/sampler.h"

namespace vox::sg {
class Sampler {
public:
    std::string name_;
    
    Sampler(std::string name, core::Sampler &&vk_sampler);
    
    Sampler(Sampler &&other) = default;
    
    virtual ~Sampler() = default;
    
    core::Sampler vk_sampler_;
};

}        // namespace vox
