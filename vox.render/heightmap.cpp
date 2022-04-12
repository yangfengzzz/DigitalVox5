/* Copyright (c) 2019-2021, Sascha Willems
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

#include "heightmap.h"

#include <cstring>

#include "error.h"
#include "platform/filesystem.h"

namespace vox {
HeightMap::HeightMap(const std::string &file_name, const uint32_t patchsize) {
    std::string file_path = fs::path::get(fs::path::Assets, file_name);
    
    ktxTexture *ktx_texture;
    ktxResult ktx_result;
    ktx_result = ktxTexture_CreateFromNamedFile(file_path.c_str(), KTX_TEXTURE_CREATE_LOAD_IMAGE_DATA_BIT,
                                                &ktx_texture);
    
    assert(ktx_result == KTX_SUCCESS);
    
    ktx_size_t ktx_size = ktxTexture_GetImageSize(ktx_texture, 0);
    ktx_uint8_t *ktx_image = ktxTexture_GetData(ktx_texture);
    
    dim = ktx_texture->baseWidth;
    data = new uint16_t[dim * dim];
    
    memcpy(data, ktx_image, ktx_size);
    
    this->scale = dim / patchsize;
    
    ktxTexture_Destroy(ktx_texture);
}

HeightMap::~HeightMap() {
    delete[] data;
}

float HeightMap::get_height(const uint32_t x, const uint32_t y) {
    uint32_t rposx = std::max(0u, std::min(x * scale, dim - 1));
    uint32_t rposy = std::max(0u, std::min(y * scale, dim - 1));
    return *(data + rposx + rposy * dim) / 65535.0f;
}

}        // namespace vox
