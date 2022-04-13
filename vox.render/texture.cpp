/* Copyright (c) 2018-2019, Arm Limited and Contributors
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 the "License";
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

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
