//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "sampler.h"

#include <utility>

namespace vox::sg {
Sampler::Sampler(std::string name, core::Sampler &&vk_sampler) :
name_{std::move(name)},
vk_sampler_{std::move(vk_sampler)} {}

}        // namespace vox
