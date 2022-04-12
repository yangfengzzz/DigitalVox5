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

#pragma once

#include <memory>
#include <string>
#include <typeinfo>
#include <unordered_map>
#include <vector>
#include "vector4.h"
#include "error.h"
#include "material.h"

namespace vox {
namespace sg {
class PBRMaterial : public Material {
public:
    PBRMaterial(const std::string &name);
    
    virtual ~PBRMaterial() = default;
    
    Vector4F base_color_factor{0.0f, 0.0f, 0.0f, 0.0f};
    
    float metallic_factor{0.0f};
    
    float roughness_factor{0.0f};
};

}        // namespace sg
}        // namespace vox
