//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "image_manager.h"
#include "core/device.h"

namespace vox {
ImageManager::ImageManager(Device& device):
device_(device) {
}

std::shared_ptr<Image> ImageManager::load_texture(const std::string &file) {
    auto image = vox::Image::load(file, file);
    image->create_vk_image(device_);
    
    const auto &queue = device_.get_queue_by_flags(VK_QUEUE_GRAPHICS_BIT, 0);
    
    VkCommandBuffer command_buffer = device_.create_command_buffer(VK_COMMAND_BUFFER_LEVEL_PRIMARY, true);
    
    vox::core::Buffer stage_buffer{device_,
        image->get_data().size(),
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VMA_MEMORY_USAGE_CPU_ONLY};
    
    stage_buffer.update(image->get_data());
    
    // Setup buffer copy regions for each mip level
    std::vector<VkBufferImageCopy> buffer_copy_regions;
    
    auto &mipmaps = image->get_mipmaps();
    
    for (size_t i = 0; i < mipmaps.size(); i++) {
        VkBufferImageCopy buffer_copy_region = {};
        buffer_copy_region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        buffer_copy_region.imageSubresource.mipLevel = vox::to_u32(i);
        buffer_copy_region.imageSubresource.baseArrayLayer = 0;
        buffer_copy_region.imageSubresource.layerCount = 1;
        buffer_copy_region.imageExtent.width = image->get_extent().width >> i;
        buffer_copy_region.imageExtent.height = image->get_extent().height >> i;
        buffer_copy_region.imageExtent.depth = 1;
        buffer_copy_region.bufferOffset = mipmaps[i].offset;
        
        buffer_copy_regions.push_back(buffer_copy_region);
    }
    
    VkImageSubresourceRange subresource_range = {};
    subresource_range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    subresource_range.baseMipLevel = 0;
    subresource_range.levelCount = vox::to_u32(mipmaps.size());
    subresource_range.layerCount = 1;
    
    // Image barrier for optimal image (target)
    // Optimal image will be used as destination for the copy
    vox::set_image_layout(
                          command_buffer,
                          image->get_vk_image().get_handle(),
                          VK_IMAGE_LAYOUT_UNDEFINED,
                          VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                          subresource_range);
    
    // Copy mip levels from staging buffer
    vkCmdCopyBufferToImage(
                           command_buffer,
                           stage_buffer.get_handle(),
                           image->get_vk_image().get_handle(),
                           VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                           static_cast<uint32_t>(buffer_copy_regions.size()),
                           buffer_copy_regions.data());
    
    // Change texture image layout to shader read after all mip levels have been copied
    vox::set_image_layout(
                          command_buffer,
                          image->get_vk_image().get_handle(),
                          VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                          VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                          subresource_range);
    
    device_.flush_command_buffer(command_buffer, queue.get_handle());
    
    image_pool_.emplace_back(image);
    return image;
}

std::shared_ptr<Image> ImageManager::load_texture_array(const std::string &file) {
    auto image = vox::Image::load(file, file);
    image->create_vk_image(device_, VK_IMAGE_VIEW_TYPE_2D_ARRAY);
    
    const auto &queue = device_.get_queue_by_flags(VK_QUEUE_GRAPHICS_BIT, 0);
    
    VkCommandBuffer command_buffer = device_.create_command_buffer(VK_COMMAND_BUFFER_LEVEL_PRIMARY, true);
    
    vox::core::Buffer stage_buffer{device_,
        image->get_data().size(),
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VMA_MEMORY_USAGE_CPU_ONLY};
    
    stage_buffer.update(image->get_data());
    
    // Setup buffer copy regions for each mip level
    std::vector<VkBufferImageCopy> buffer_copy_regions;
    
    auto &mipmaps = image->get_mipmaps();
    const auto &layers = image->get_layers();
    
    auto &offsets = image->get_offsets();
    
    for (uint32_t layer = 0; layer < layers; layer++) {
        for (size_t i = 0; i < mipmaps.size(); i++) {
            VkBufferImageCopy buffer_copy_region = {};
            buffer_copy_region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            buffer_copy_region.imageSubresource.mipLevel = vox::to_u32(i);
            buffer_copy_region.imageSubresource.baseArrayLayer = layer;
            buffer_copy_region.imageSubresource.layerCount = 1;
            buffer_copy_region.imageExtent.width = image->get_extent().width >> i;
            buffer_copy_region.imageExtent.height = image->get_extent().height >> i;
            buffer_copy_region.imageExtent.depth = 1;
            buffer_copy_region.bufferOffset = offsets[layer][i];
            
            buffer_copy_regions.push_back(buffer_copy_region);
        }
    }
    
    VkImageSubresourceRange subresource_range = {};
    subresource_range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    subresource_range.baseMipLevel = 0;
    subresource_range.levelCount = vox::to_u32(mipmaps.size());
    subresource_range.layerCount = layers;
    
    // Image barrier for optimal image (target)
    // Optimal image will be used as destination for the copy
    vox::set_image_layout(
                          command_buffer,
                          image->get_vk_image().get_handle(),
                          VK_IMAGE_LAYOUT_UNDEFINED,
                          VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                          subresource_range);
    
    // Copy mip levels from staging buffer
    vkCmdCopyBufferToImage(
                           command_buffer,
                           stage_buffer.get_handle(),
                           image->get_vk_image().get_handle(),
                           VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                           static_cast<uint32_t>(buffer_copy_regions.size()),
                           buffer_copy_regions.data());
    
    // Change texture image layout to shader read after all mip levels have been copied
    vox::set_image_layout(
                          command_buffer,
                          image->get_vk_image().get_handle(),
                          VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                          VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                          subresource_range);
    
    device_.flush_command_buffer(command_buffer, queue.get_handle());
    
    image_pool_.emplace_back(image);
    return image;
}

std::shared_ptr<Image> ImageManager::load_texture_cubemap(const std::string &file) {
    auto image = vox::Image::load(file, file);
    image->create_vk_image(device_, VK_IMAGE_VIEW_TYPE_CUBE, VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT);
    
    const auto &queue = device_.get_queue_by_flags(VK_QUEUE_GRAPHICS_BIT, 0);
    
    VkCommandBuffer command_buffer = device_.create_command_buffer(VK_COMMAND_BUFFER_LEVEL_PRIMARY, true);
    
    vox::core::Buffer stage_buffer{device_,
        image->get_data().size(),
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VMA_MEMORY_USAGE_CPU_ONLY};
    
    stage_buffer.update(image->get_data());
    
    // Setup buffer copy regions for each mip level
    std::vector<VkBufferImageCopy> buffer_copy_regions;
    
    auto &mipmaps = image->get_mipmaps();
    const auto &layers = image->get_layers();
    
    auto &offsets = image->get_offsets();
    
    for (uint32_t layer = 0; layer < layers; layer++) {
        for (size_t i = 0; i < mipmaps.size(); i++) {
            VkBufferImageCopy buffer_copy_region = {};
            buffer_copy_region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            buffer_copy_region.imageSubresource.mipLevel = vox::to_u32(i);
            buffer_copy_region.imageSubresource.baseArrayLayer = layer;
            buffer_copy_region.imageSubresource.layerCount = 1;
            buffer_copy_region.imageExtent.width = image->get_extent().width >> i;
            buffer_copy_region.imageExtent.height = image->get_extent().height >> i;
            buffer_copy_region.imageExtent.depth = 1;
            buffer_copy_region.bufferOffset = offsets[layer][i];
            
            buffer_copy_regions.push_back(buffer_copy_region);
        }
    }
    
    VkImageSubresourceRange subresource_range = {};
    subresource_range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    subresource_range.baseMipLevel = 0;
    subresource_range.levelCount = vox::to_u32(mipmaps.size());
    subresource_range.layerCount = layers;
    
    // Image barrier for optimal image (target)
    // Optimal image will be used as destination for the copy
    vox::set_image_layout(command_buffer,
                          image->get_vk_image().get_handle(),
                          VK_IMAGE_LAYOUT_UNDEFINED,
                          VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                          subresource_range);
    
    // Copy mip levels from staging buffer
    vkCmdCopyBufferToImage(command_buffer,
                           stage_buffer.get_handle(),
                           image->get_vk_image().get_handle(),
                           VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                           static_cast<uint32_t>(buffer_copy_regions.size()),
                           buffer_copy_regions.data());
    
    // Change texture image layout to shader read after all mip levels have been copied
    vox::set_image_layout(command_buffer,
                          image->get_vk_image().get_handle(),
                          VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                          VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                          subresource_range);
    
    device_.flush_command_buffer(command_buffer, queue.get_handle());
    
    image_pool_.emplace_back(image);
    return image;
}

void ImageManager::collect_garbage() {
    image_pool_.erase(std::remove_if(image_pool_.begin(), image_pool_.end(), [](const std::shared_ptr<Image>& image){
        return image.use_count() == 1;
    }), image_pool_.end());
}

}
