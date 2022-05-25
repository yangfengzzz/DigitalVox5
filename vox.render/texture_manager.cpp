//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/texture_manager.h"

#include "vox.render/core/device.h"
#include "vox.render/shader/shader_manager.h"

namespace vox {
TextureManager *TextureManager::GetSingletonPtr() { return ms_singleton; }

TextureManager &TextureManager::GetSingleton() {
    assert(ms_singleton);
    return (*ms_singleton);
}

TextureManager::TextureManager(Device &device)
    : device_(device), shader_data_(device), sampler_create_info_{VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO} {
    // Create a default sampler
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
    // In a real-world scenario the level of anisotropy should be a user setting or e.g. lowered for mobile devices by
    // default
    sampler_create_info_.maxAnisotropy = device.GetGpu().GetFeatures().samplerAnisotropy
                                                 ? (device.GetGpu().GetProperties().limits.maxSamplerAnisotropy)
                                                 : 1.0f;
    sampler_create_info_.anisotropyEnable = device.GetGpu().GetFeatures().samplerAnisotropy;
    sampler_create_info_.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;

    sampler_ = std::make_unique<core::Sampler>(device, sampler_create_info_);
}

std::shared_ptr<Texture> TextureManager::LoadTexture(const std::string &file) {
    auto iter = image_pool_.find(file);
    if (iter != image_pool_.end()) {
        return iter->second;
    } else {
        auto image = vox::Texture::Load(file, file);
        image->CreateVkImage(device_);
        UploadTexture(image.get());
        image_pool_.insert(std::make_pair(file, image));
        return image;
    }
}

std::shared_ptr<Texture> TextureManager::LoadTextureArray(const std::string &file) {
    auto iter = image_pool_.find(file);
    if (iter != image_pool_.end()) {
        return iter->second;
    } else {
        auto image = vox::Texture::Load(file, file);
        image->CreateVkImage(device_, VK_IMAGE_VIEW_TYPE_2D_ARRAY);
        UploadTexture(image.get());
        image_pool_.insert(std::make_pair(file, image));
        return image;
    }
}

std::shared_ptr<Texture> TextureManager::LoadTextureCubemap(const std::string &file) {
    auto iter = image_pool_.find(file);
    if (iter != image_pool_.end()) {
        return iter->second;
    } else {
        auto image = vox::Texture::Load(file, file);
        image->CreateVkImage(device_, VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT);
        UploadTexture(image.get());
        image_pool_.insert(std::make_pair(file, image));
        return image;
    }
}

void TextureManager::UploadTexture(Texture *image) {
    const auto &queue = device_.GetQueueByFlags(VK_QUEUE_GRAPHICS_BIT, 0);

    VkCommandBuffer command_buffer = device_.CreateCommandBuffer(VK_COMMAND_BUFFER_LEVEL_PRIMARY, true);

    vox::core::Buffer stage_buffer{device_, image->GetData().size(), VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                   VMA_MEMORY_USAGE_CPU_ONLY};

    stage_buffer.Update(image->GetData());

    // Setup buffer copy regions for each mip level
    std::vector<VkBufferImageCopy> buffer_copy_regions;

    auto &mipmaps = image->GetMipmaps();
    const auto &layers = image->GetLayers();

    auto &offsets = image->GetOffsets();

    for (uint32_t layer = 0; layer < layers; layer++) {
        for (size_t i = 0; i < mipmaps.size(); i++) {
            VkBufferImageCopy buffer_copy_region = {};
            buffer_copy_region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            buffer_copy_region.imageSubresource.mipLevel = utility::ToU32(i);
            buffer_copy_region.imageSubresource.baseArrayLayer = layer;
            buffer_copy_region.imageSubresource.layerCount = 1;
            buffer_copy_region.imageExtent.width = image->GetExtent().width >> i;
            buffer_copy_region.imageExtent.height = image->GetExtent().height >> i;
            buffer_copy_region.imageExtent.depth = 1;
            buffer_copy_region.bufferOffset = layers > 1 ? offsets[layer][i] : mipmaps[i].offset;

            buffer_copy_regions.push_back(buffer_copy_region);
        }
    }

    VkImageSubresourceRange subresource_range = {};
    subresource_range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    subresource_range.baseMipLevel = 0;
    subresource_range.levelCount = utility::ToU32(mipmaps.size());
    subresource_range.layerCount = layers;

    // Image barrier for optimal image (target)
    // Optimal image will be used as destination for the copy
    vox::SetImageLayout(command_buffer, image->GetVkImage().GetHandle(), VK_IMAGE_LAYOUT_UNDEFINED,
                        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, subresource_range);

    // Copy mip levels from staging buffer
    vkCmdCopyBufferToImage(command_buffer, stage_buffer.GetHandle(), image->GetVkImage().GetHandle(),
                           VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, static_cast<uint32_t>(buffer_copy_regions.size()),
                           buffer_copy_regions.data());

    // Change texture image layout to shader read after all mip levels have been copied
    vox::SetImageLayout(command_buffer, image->GetVkImage().GetHandle(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, subresource_range);

    device_.FlushCommandBuffer(command_buffer, queue.GetHandle());
}

void TextureManager::CollectGarbage() {
    for (auto &image : image_pool_) {
        if (image.second.use_count() == 1) {
            image.second.reset();
        }
    }
}

// MARK: - PBR
std::shared_ptr<Texture> TextureManager::GenerateIBL(const std::string &file, RenderContext &render_context) {
    auto iter = image_pool_.find(file + "ibl");
    if (iter != image_pool_.end()) {
        return iter->second;
    } else {
        auto &command_buffer = render_context.Begin();
        command_buffer.Begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

        auto source = LoadTextureCubemap(file);
        auto baker_mipmap_count = static_cast<uint32_t>(source->GetMipmaps().size());
        std::vector<Mipmap> mipmap = source->GetMipmaps();

        auto target = std::make_shared<Texture>(file + "ibl", std::vector<uint8_t>(), std::move(mipmap));
        target->SetLayers(source->GetLayers());
        target->SetFormat(source->GetFormat());
        target->CreateVkImage(device_, 0, VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_SAMPLED_BIT);

        if (!pipeline_) {
            pipeline_ = std::make_unique<PostProcessingPipeline>(render_context, ShaderSource());
            ibl_pass_ = &pipeline_->AddPass<PostProcessingComputePass>(
                    ShaderManager::GetSingleton().LoadShader("base/ibl.comp"));
            ibl_pass_->AttachShaderData(&shader_data_);
        }
        shader_data_.SetSampledTexture("environmentMap", source->GetVkImageView(VK_IMAGE_VIEW_TYPE_CUBE),
                                       sampler_.get());
        uint32_t source_width = source->GetExtent().width;
        shader_data_.SetData("textureSize", source_width);

        auto &render_frame = render_context.GetActiveFrame();
        for (uint32_t lod = 0; lod < baker_mipmap_count; lod++) {
            float lod_roughness = float(lod) / float(baker_mipmap_count - 1);  // linear
            auto allocation = render_frame.AllocateBuffer(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, sizeof(float), 0);
            allocation.Update(lod_roughness);
            shader_data_.SetData("lodRoughness", std::move(allocation));

            shader_data_.SetStorageTexture("o_results", target->GetVkImageView(VK_IMAGE_VIEW_TYPE_CUBE, lod, 0, 1, 0));

            ibl_pass_->SetDispatchSize({(source_width + 8) / 8, (source_width + 8) / 8, 6});
            pipeline_->Draw(command_buffer, render_context.GetActiveFrame().GetRenderTarget());
        }
        command_buffer.End();
        render_context.Submit(command_buffer);

        image_pool_.insert(std::make_pair(file + "ibl", target));
        return target;
    }
}

SphericalHarmonics3 TextureManager::GenerateSH(const std::string &file) {
    auto source = LoadTextureCubemap(file);
    const auto &layers = source->GetLayers();
    auto &offsets = source->GetOffsets();
    uint32_t texture_size = source->GetExtent().width;
    float texel_size = 2.f / static_cast<float>(texture_size);  // convolution is in the space of [-1, 1]

    float solid_angle_sum = 0;
    const uint32_t kChannelLength = GetBitsPerPixel(source->GetFormat()) / 8;
    const uint32_t kChannelShift = kChannelLength / 4;
    const float kTotalColor = std::powf(256.f, static_cast<float>(kChannelShift)) - 1;
    SphericalHarmonics3 sh;
    for (uint32_t layer = 0; layer < layers; layer++) {
        uint64_t buffer_offset = offsets[layer][0];
        float v = texel_size * 0.5f - 1.f;
        for (uint32_t y = 0; y < texture_size; y++) {
            float u = texel_size * 0.5f - 1.f;
            for (uint32_t x = 0; x < texture_size; x++) {
                uint64_t data_offset = y * texture_size * kChannelLength + x * kChannelLength + buffer_offset;

                float r = 0.f;
                for (uint8_t i = 0; i < kChannelShift; i++) {
                    r += static_cast<float>(source->data_[data_offset + i]) * std::powf(256.f, i);
                }

                float g = 0.f;
                for (uint8_t i = 0; i < kChannelShift; i++) {
                    g += static_cast<float>(source->data_[data_offset + i + kChannelShift]) * std::powf(256.f, i);
                }

                float b = 0.f;
                for (uint8_t i = 0; i < kChannelShift; i++) {
                    b += static_cast<float>(source->data_[data_offset + i + 2 * kChannelShift]) * std::powf(256.f, i);
                }
                Color color(r / kTotalColor, g / kTotalColor, b / kTotalColor, 0);

                Vector3F direction;
                switch (layer) {
                    case 0:  // TextureCubeFace.PositiveX
                        direction.set(1, -v, -u);
                        break;
                    case 1:  // TextureCubeFace.NegativeX
                        direction.set(-1, -v, u);
                        break;
                    case 2:  // TextureCubeFace.PositiveY
                        direction.set(u, -1, -v);
                        break;
                    case 3:  // TextureCubeFace.NegativeY
                        direction.set(u, 1, v);
                        break;
                    case 4:  // TextureCubeFace.PositiveZ
                        direction.set(u, -v, 1);
                        break;
                    case 5:  // TextureCubeFace.NegativeZ
                        direction.set(-u, -v, -1);
                        break;
                    default:
                        break;
                }

                /**
                 * dA = cos = S / r = 4 / r
                 * dw = dA / r2 = 4 / r / r2
                 */
                float solid_angle = 4.f / (direction.length() * direction.lengthSquared());
                solid_angle_sum += solid_angle;
                sh.addLight(direction.normalized(), color, solid_angle);
                u += texel_size;
            }
            v += texel_size;
        }
    }
    sh = sh * (static_cast<float>(4.0 * M_PI) / solid_angle_sum);
    return sh;
}

// MARK: - Shadow
std::shared_ptr<Texture> TextureManager::PackedShadowMap(CommandBuffer &command_buffer,
                                                         std::vector<RenderTarget *> used_shadow,
                                                         uint32_t shadow_map_resolution) {
    if (!packed_shadow_map_ || packed_shadow_map_->GetLayers() != used_shadow.size()) {
        std::vector<Mipmap> mipmap(1);
        mipmap[0].extent = {shadow_map_resolution, shadow_map_resolution, 1};
        packed_shadow_map_ = std::make_shared<Texture>("shadowmap", std::vector<uint8_t>(), std::move(mipmap));
        packed_shadow_map_->SetLayers(static_cast<uint32_t>(used_shadow.size()));
        packed_shadow_map_->SetFormat(GetSuitableDepthFormat(command_buffer.GetDevice().GetGpu().GetHandle()));
        packed_shadow_map_->CreateVkImage(command_buffer.GetDevice());
    }

    std::vector<VkImageCopy> regions(1);
    regions[0].extent = {shadow_map_resolution, shadow_map_resolution, 1};
    regions[0].srcSubresource.layerCount = 1;
    regions[0].dstSubresource.layerCount = 1;

    for (uint32_t i = 0; i < used_shadow.size(); i++) {
        regions[0].dstSubresource.baseArrayLayer = i;

        const auto &src_img = used_shadow[i]->GetViews()[0].GetImage();
        command_buffer.CopyImage(src_img, packed_shadow_map_->GetVkImage(), regions);
    }
    return packed_shadow_map_;
}

}  // namespace vox
