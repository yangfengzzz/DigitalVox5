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

#include "sampler.h"

namespace vox::sg {
class Image;

class Sampler;

class Texture {
public:
    std::string name_;
    
    explicit Texture(std::string name);
    
    Texture(Texture &&other) = default;
    
    virtual ~Texture() = default;
    
    void set_image(Image &image);
    
    Image *get_image();
    
    void set_sampler(Sampler &sampler);
    
    Sampler *get_sampler();
    
private:
    Image *image_{nullptr};
    
    Sampler *sampler_{nullptr};
};

}        // namespace vox
