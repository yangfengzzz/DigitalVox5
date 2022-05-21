//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "image.h"

#include <mutex>
#include <utility>

#include "error.h"

VKBP_DISABLE_WARNINGS()

#define STB_IMAGE_RESIZE_IMPLEMENTATION

#include <stb_image_resize.h>

VKBP_ENABLE_WARNINGS()

#include "utils.h"
#include "platform/filesystem.h"
#include "image/astc_img.h"
#include "image/ktx_img.h"
#include "image/stb_img.h"

namespace vox {
bool IsAstc(VkFormat format) {
    return (format == VK_FORMAT_ASTC_4x4_UNORM_BLOCK ||
            format == VK_FORMAT_ASTC_4x4_SRGB_BLOCK ||
            format == VK_FORMAT_ASTC_5x4_UNORM_BLOCK ||
            format == VK_FORMAT_ASTC_5x4_SRGB_BLOCK ||
            format == VK_FORMAT_ASTC_5x5_UNORM_BLOCK ||
            format == VK_FORMAT_ASTC_5x5_SRGB_BLOCK ||
            format == VK_FORMAT_ASTC_6x5_UNORM_BLOCK ||
            format == VK_FORMAT_ASTC_6x5_SRGB_BLOCK ||
            format == VK_FORMAT_ASTC_6x6_UNORM_BLOCK ||
            format == VK_FORMAT_ASTC_6x6_SRGB_BLOCK ||
            format == VK_FORMAT_ASTC_8x5_UNORM_BLOCK ||
            format == VK_FORMAT_ASTC_8x5_SRGB_BLOCK ||
            format == VK_FORMAT_ASTC_8x6_UNORM_BLOCK ||
            format == VK_FORMAT_ASTC_8x6_SRGB_BLOCK ||
            format == VK_FORMAT_ASTC_8x8_UNORM_BLOCK ||
            format == VK_FORMAT_ASTC_8x8_SRGB_BLOCK ||
            format == VK_FORMAT_ASTC_10x5_UNORM_BLOCK ||
            format == VK_FORMAT_ASTC_10x5_SRGB_BLOCK ||
            format == VK_FORMAT_ASTC_10x6_UNORM_BLOCK ||
            format == VK_FORMAT_ASTC_10x6_SRGB_BLOCK ||
            format == VK_FORMAT_ASTC_10x8_UNORM_BLOCK ||
            format == VK_FORMAT_ASTC_10x8_SRGB_BLOCK ||
            format == VK_FORMAT_ASTC_10x10_UNORM_BLOCK ||
            format == VK_FORMAT_ASTC_10x10_SRGB_BLOCK ||
            format == VK_FORMAT_ASTC_12x10_UNORM_BLOCK ||
            format == VK_FORMAT_ASTC_12x10_SRGB_BLOCK ||
            format == VK_FORMAT_ASTC_12x12_UNORM_BLOCK ||
            format == VK_FORMAT_ASTC_12x12_SRGB_BLOCK);
}

Image::Image(std::string name, std::vector<uint8_t> &&d, std::vector<Mipmap> &&m) :
name_{std::move(name)},
data_{std::move(d)},
format_{VK_FORMAT_R8G8B8A8_UNORM},
mipmaps_{std::move(m)} {
}

const std::vector<uint8_t> &Image::GetData() const {
    return data_;
}

void Image::ClearData() {
    data_.clear();
    data_.shrink_to_fit();
}

VkFormat Image::GetFormat() const {
    return format_;
}

const VkExtent3D &Image::GetExtent() const {
    return mipmaps_.at(0).extent;
}

uint32_t Image::GetLayers() const {
    return layers_;
}

const std::vector<Mipmap> &Image::GetMipmaps() const {
    return mipmaps_;
}

const std::vector<std::vector<VkDeviceSize>> &Image::GetOffsets() const {
    return offsets_;
}

void Image::CreateVkImage(Device const &device, VkImageCreateFlags flags, VkImageUsageFlags image_usage) {
    assert(!vk_image_ && vk_image_views_.empty() && "Vulkan image already constructed");
    
    vk_image_ = std::make_unique<core::Image>(device, GetExtent(),
                                              format_,
                                              image_usage,
                                              VMA_MEMORY_USAGE_GPU_ONLY,
                                              VK_SAMPLE_COUNT_1_BIT,
                                              ToU32(mipmaps_.size()),
                                              layers_,
                                              VK_IMAGE_TILING_OPTIMAL,
                                              flags);
    vk_image_->SetDebugName(name_);
}

const core::Image &Image::GetVkImage() const {
    assert(vk_image_ && "Vulkan image was not created");
    return *vk_image_;
}

const core::ImageView &Image::GetVkImageView(VkImageViewType view_type,
                                                uint32_t base_mip_level,
                                                uint32_t base_array_layer,
                                                uint32_t n_mip_levels,
                                                uint32_t n_array_layers) {
    std::size_t key = 0;
    vox::HashCombine(key, view_type);
    vox::HashCombine(key, base_mip_level);
    vox::HashCombine(key, base_array_layer);
    vox::HashCombine(key, n_mip_levels);
    vox::HashCombine(key, n_array_layers);
    auto iter = vk_image_views_.find(key);
    if (iter == vk_image_views_.end()) {
        vk_image_views_.insert(std::make_pair(key, std::make_unique<core::ImageView>(*vk_image_, view_type, GetFormat(),
                                                                                     base_mip_level, base_array_layer,
                                                                                     n_mip_levels, n_array_layers)));
    }
    
    return *vk_image_views_.find(key)->second.get();
}

Mipmap &Image::GetMipmap(size_t index) {
    return mipmaps_.at(index);
}

void Image::GenerateMipmaps() {
    assert(mipmaps_.size() == 1 && "Mipmaps already generated");
    
    if (mipmaps_.size() > 1) {
        return;        // Do not generate again
    }
    
    auto extent = GetExtent();
    auto next_width = std::max<uint32_t>(1u, extent.width / 2);
    auto next_height = std::max<uint32_t>(1u, extent.height / 2);
    auto channels = 4;
    auto next_size = next_width * next_height * channels;
    
    while (true) {
        // Make space for next mipmap
        auto old_size = ToU32(data_.size());
        data_.resize(old_size + next_size);
        
        auto &prev_mipmap = mipmaps_.back();
        // Update mipmaps
        Mipmap next_mipmap{};
        next_mipmap.level = prev_mipmap.level + 1;
        next_mipmap.offset = old_size;
        next_mipmap.extent = {next_width, next_height, 1u};
        
        // Fill next mipmap memory
        stbir_resize_uint8(data_.data() + prev_mipmap.offset, prev_mipmap.extent.width,
                           prev_mipmap.extent.height, 0,
                           data_.data() + next_mipmap.offset, next_mipmap.extent.width,
                           next_mipmap.extent.height, 0, channels);
        
        mipmaps_.emplace_back(next_mipmap);
        
        // Next mipmap values
        next_width = std::max<uint32_t>(1u, next_width / 2);
        next_height = std::max<uint32_t>(1u, next_height / 2);
        next_size = next_width * next_height * channels;
        
        if (next_width == 1 && next_height == 1) {
            break;
        }
    }
}

std::vector<Mipmap> &Image::GetMutMipmaps() {
    return mipmaps_;
}

std::vector<uint8_t> &Image::GetMutData() {
    return data_;
}

void Image::SetData(const uint8_t *raw_data, size_t size) {
    assert(data_.empty() && "Image data already set");
    data_ = {raw_data, raw_data + size};
}

void Image::SetFormat(VkFormat format) {
    format_ = format;
}

void Image::SetWidth(uint32_t width) {
    mipmaps_.at(0).extent.width = width;
}

void Image::SetHeight(uint32_t height) {
    mipmaps_.at(0).extent.height = height;
}

void Image::SetDepth(uint32_t depth) {
    mipmaps_.at(0).extent.depth = depth;
}

void Image::SetLayers(uint32_t layers) {
    layers_ = layers;
}

void Image::SetOffsets(const std::vector<std::vector<VkDeviceSize>> &offsets) {
    offsets_ = offsets;
}

std::shared_ptr<Image> Image::Load(const std::string &name, const std::string &uri) {
    std::shared_ptr<Image> image{nullptr};
    
    auto data = fs::ReadAsset(uri);
    
    // Get extension
    auto extension = GetExtension(uri);
    
    if (extension == "png" || extension == "jpg") {
        image = std::make_shared<Stb>(name, data);
    } else if (extension == "astc") {
        image = std::make_shared<Astc>(name, data);
    } else if (extension == "ktx") {
        image = std::make_shared<Ktx>(name, data);
    } else if (extension == "ktx2") {
        image = std::make_shared<Ktx>(name, data);
    }
    
    return image;
}

}        // namespace vox
