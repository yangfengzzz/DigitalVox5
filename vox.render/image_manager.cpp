//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "image_manager.h"
#include "core/device.h"
#include "shader/shader_manager.h"

namespace vox {
ImageManager *ImageManager::get_singleton_ptr() {
    return ms_singleton_;
}

ImageManager &ImageManager::get_singleton() {
    assert(ms_singleton_);
    return (*ms_singleton_);
}

ImageManager::ImageManager(Device& device):
device_(device),
shader_data_(device) {
    // Create a default sampler
    sampler_create_info_ = {VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO};
    sampler_create_info_.magFilter = VK_FILTER_LINEAR;
    sampler_create_info_.minFilter = VK_FILTER_LINEAR;
    sampler_create_info_.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    sampler_create_info_.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    sampler_create_info_.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    sampler_create_info_.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    sampler_create_info_.mipLodBias = 0.0f;
    sampler_create_info_.compareOp = VK_COMPARE_OP_NEVER;
    sampler_create_info_.minLod = 0.0f;
    // Max level-of-detail should match mip level count
    sampler_create_info_.maxLod = 0.0f;
    // Only enable anisotropic filtering if enabled on the device
    // Note that for simplicity, we will always be using max. available anisotropy level for the current device
    // This may have an impact on performance, esp. on lower-specced devices
    // In a real-world scenario the level of anisotropy should be a user setting or e.g. lowered for mobile devices by default
    sampler_create_info_.maxAnisotropy = device.get_gpu().get_features().samplerAnisotropy
    ? (device.get_gpu().get_properties().limits.maxSamplerAnisotropy)
    : 1.0f;
    sampler_create_info_.anisotropyEnable = device.get_gpu().get_features().samplerAnisotropy;
    sampler_create_info_.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
    
    sampler_ = std::make_unique<core::Sampler>(device, sampler_create_info_);
}

std::shared_ptr<Image> ImageManager::load_texture(const std::string &file) {
    auto iter = image_pool_.find(file);
    if (iter != image_pool_.end()) {
        return iter->second;
    } else {
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
        
        image_pool_.insert(std::make_pair(file, image));
        return image;
    }
}

std::shared_ptr<Image> ImageManager::load_texture_array(const std::string &file) {
    auto iter = image_pool_.find(file);
    if (iter != image_pool_.end()) {
        return iter->second;
    } else {
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
        
        image_pool_.insert(std::make_pair(file, image));
        return image;
    }
}

std::shared_ptr<Image> ImageManager::load_texture_cubemap(const std::string &file) {
    auto iter = image_pool_.find(file);
    if (iter != image_pool_.end()) {
        return iter->second;
    } else {
        auto image = vox::Image::load(file, file);
        image->create_vk_image(device_, VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT);
        
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
        
        image_pool_.insert(std::make_pair(file, image));
        return image;
    }
}

void ImageManager::collect_garbage() {
    for (auto& image : image_pool_) {
        if (image.second.use_count() == 1) {
            image.second.reset();
        }
    }
}

std::shared_ptr<Image> ImageManager::generate_ibl(const std::string &file,
                                                  RenderContext &render_context) {
    auto iter = image_pool_.find(file + "ibl");
    if (iter != image_pool_.end()) {
        return iter->second;
    } else {
        auto &command_buffer = render_context.begin();
        command_buffer.begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
        
        auto source = load_texture_cubemap(file);
        auto bakerMipmapCount = static_cast<uint32_t>(source->get_mipmaps().size());
        std::vector<Mipmap> mipmap = source->get_mipmaps();
        
        auto target = std::make_shared<Image>(file + "ibl", std::vector<uint8_t>(), std::move(mipmap));
        target->set_layers(source->get_layers());
        target->set_format(source->get_format());
        target->create_vk_image(device_, 0, VK_IMAGE_USAGE_STORAGE_BIT);
        
        if (!pipeline_) {
            pipeline_ = std::make_unique<PostProcessingPipeline>(render_context, ShaderSource());
            ibl_pass_ = &pipeline_->add_pass<PostProcessingComputePass>(ShaderManager::get_singleton().load_shader("base/ibl.comp"));
            ibl_pass_->attach_shader_data(&shader_data_);
        }
        shader_data_.set_sampled_texture("environmentMap", source->get_vk_image_view(VK_IMAGE_VIEW_TYPE_CUBE), sampler_.get());
        uint32_t source_width = source->get_extent().width;
        shader_data_.set_data("textureSize", source_width);
        for (uint32_t lod = 0; lod < bakerMipmapCount; lod++) {
            float lodRoughness = float(lod) / float(bakerMipmapCount - 1); // linear
            shader_data_.set_data("lodRoughness", lodRoughness);
            shader_data_.set_storage_texture("o_results", target->get_vk_image_view(VK_IMAGE_VIEW_TYPE_CUBE, lod, 0, 1, 0));

            ibl_pass_->set_dispatch_size({(source_width + 8) / 8, (source_width + 8) / 8, 6});
            pipeline_->draw(command_buffer, render_context.get_active_frame().get_render_target());
        }
        command_buffer.end();
        render_context.submit(command_buffer);
        
        image_pool_.insert(std::make_pair(file + "ibl", target));
        return target;
    }
}

}
