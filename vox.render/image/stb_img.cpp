//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "image/stb_img.h"

#define STB_IMAGE_IMPLEMENTATION

#include <stb_image.h>

namespace vox {
Stb::Stb(const std::string &name, const std::vector<uint8_t> &data) :
Image{name} {
    int width;
    int height;
    int comp;
    int req_comp = 4;
    
    auto data_buffer = reinterpret_cast<const stbi_uc *>(data.data());
    auto data_size = static_cast<int>(data.size());
    
    auto raw_data = stbi_load_from_memory(data_buffer, data_size, &width, &height, &comp, req_comp);
    
    if (!raw_data) {
        throw std::runtime_error{"Failed to load " + name + ": " + stbi_failure_reason()};
    }
    
    set_data(raw_data, width * height * req_comp);
    stbi_image_free(raw_data);
    
    set_format(VK_FORMAT_R8G8B8A8_UNORM);
    set_width(to_u32(width));
    set_height(to_u32(height));
    set_depth(1u);
}

}        // namespace vox
