//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "heightmap.h"

#include <cstring>
#include "platform/filesystem.h"

namespace vox {
HeightMap::HeightMap(const std::string &file_name, const uint32_t patchsize) {
    std::string file_path = fs::path::get(fs::path::ASSETS, file_name);
    
    ktxTexture *ktx_texture;
    ktxResult ktx_result;
    ktx_result = ktxTexture_CreateFromNamedFile(file_path.c_str(), KTX_TEXTURE_CREATE_LOAD_IMAGE_DATA_BIT,
                                                &ktx_texture);
    
    assert(ktx_result == KTX_SUCCESS);
    
    ktx_size_t ktx_size = ktxTexture_GetImageSize(ktx_texture, 0);
    ktx_uint8_t *ktx_image = ktxTexture_GetData(ktx_texture);
    
    dim_ = ktx_texture->baseWidth;
    data_ = new uint16_t[dim_ * dim_];
    
    memcpy(data_, ktx_image, ktx_size);
    
    scale_ = dim_ / patchsize;
    
    ktxTexture_Destroy(ktx_texture);
}

HeightMap::~HeightMap() {
    delete[] data_;
}

float HeightMap::get_height(const uint32_t x, const uint32_t y) {
    uint32_t rposx = std::max(0u, std::min(x * scale_, dim_ - 1));
    uint32_t rposy = std::max(0u, std::min(y * scale_, dim_ - 1));
    return *(data_ + rposx + rposy * dim_) / 65535.0f;
}

}        // namespace vox
