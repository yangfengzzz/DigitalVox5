//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "texture.h"

#include <utility>

#include "image.h"
#include "sampler.h"

namespace vox::sg {
Texture::Texture(std::string name) :
name{std::move(name)} {}

void Texture::set_image(Image &i) {
    image = &i;
}

Image *Texture::get_image() {
    return image;
}

void Texture::set_sampler(Sampler &s) {
    sampler = &s;
}

Sampler *Texture::get_sampler() {
    assert(sampler && "Texture has no sampler");
    return sampler;
}

}        // namespace vox
