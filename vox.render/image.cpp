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

namespace vox::sg {
bool is_astc(const VkFormat format) {
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

const std::vector<uint8_t> &Image::get_data() const {
    return data_;
}

void Image::clear_data() {
    data_.clear();
    data_.shrink_to_fit();
}

VkFormat Image::get_format() const {
    return format_;
}

const VkExtent3D &Image::get_extent() const {
    return mipmaps_.at(0).extent;
}

uint32_t Image::get_layers() const {
    return layers_;
}

const std::vector<Mipmap> &Image::get_mipmaps() const {
    return mipmaps_;
}

const std::vector<std::vector<VkDeviceSize>> &Image::get_offsets() const {
    return offsets_;
}

void Image::create_vk_image(Device const &device, VkImageViewType image_view_type, VkImageCreateFlags flags) {
    assert(!vk_image_ && !vk_image_view_ && "Vulkan image already constructed");
    
    vk_image_ = std::make_unique<core::Image>(device,
                                             get_extent(),
                                             format_,
                                             VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
                                             VMA_MEMORY_USAGE_GPU_ONLY,
                                             VK_SAMPLE_COUNT_1_BIT,
                                             to_u32(mipmaps_.size()),
                                             layers_,
                                             VK_IMAGE_TILING_OPTIMAL,
                                             flags);
    vk_image_->set_debug_name(name_);
    
    vk_image_view_ = std::make_unique<core::ImageView>(*vk_image_, image_view_type);
    vk_image_view_->set_debug_name("View on " + name_);
}

const core::Image &Image::get_vk_image() const {
    assert(vk_image_ && "Vulkan image was not created");
    return *vk_image_;
}

const core::ImageView &Image::get_vk_image_view() const {
    assert(vk_image_view_ && "Vulkan image view was not created");
    return *vk_image_view_;
}

Mipmap &Image::get_mipmap(const size_t index) {
    return mipmaps_.at(index);
}

void Image::generate_mipmaps() {
    assert(mipmaps_.size() == 1 && "Mipmaps already generated");
    
    if (mipmaps_.size() > 1) {
        return;        // Do not generate again
    }
    
    auto extent = get_extent();
    auto next_width = std::max<uint32_t>(1u, extent.width / 2);
    auto next_height = std::max<uint32_t>(1u, extent.height / 2);
    auto channels = 4;
    auto next_size = next_width * next_height * channels;
    
    while (true) {
        // Make space for next mipmap
        auto old_size = to_u32(data_.size());
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

std::vector<Mipmap> &Image::get_mut_mipmaps() {
    return mipmaps_;
}

std::vector<uint8_t> &Image::get_mut_data() {
    return data_;
}

void Image::set_data(const uint8_t *raw_data, size_t size) {
    assert(data_.empty() && "Image data already set");
    data_ = {raw_data, raw_data + size};
}

void Image::set_format(const VkFormat f) {
    format_ = f;
}

void Image::set_width(const uint32_t width) {
    mipmaps_.at(0).extent.width = width;
}

void Image::set_height(const uint32_t height) {
    mipmaps_.at(0).extent.height = height;
}

void Image::set_depth(const uint32_t depth) {
    mipmaps_.at(0).extent.depth = depth;
}

void Image::set_layers(uint32_t l) {
    layers_ = l;
}

void Image::set_offsets(const std::vector<std::vector<VkDeviceSize>> &o) {
    offsets_ = o;
}

std::unique_ptr<Image> Image::load(const std::string &name, const std::string &uri) {
    std::unique_ptr<Image> image{nullptr};
    
    auto data = fs::read_asset(uri);
    
    // Get extension
    auto extension = get_extension(uri);
    
    if (extension == "png" || extension == "jpg") {
        image = std::make_unique<Stb>(name, data);
    } else if (extension == "astc") {
        image = std::make_unique<Astc>(name, data);
    } else if (extension == "ktx") {
        image = std::make_unique<Ktx>(name, data);
    } else if (extension == "ktx2") {
        image = std::make_unique<Ktx>(name, data);
    }
    
    return image;
}

}        // namespace vox
