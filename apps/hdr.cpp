/* Copyright (c) 2019-2022, Sascha Willems
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

/*
 * High dynamic range rendering
 */

#include "hdr.h"
#include "matrix_utils.h"
#include "sub_mesh.h"

namespace vox {
HDR::HDR() {
    title_ = "High dynamic range rendering";
}

HDR::~HDR() {
    if (device_) {
        vkDestroyPipeline(get_device().get_handle(), pipelines_.skybox_, nullptr);
        vkDestroyPipeline(get_device().get_handle(), pipelines_.reflect_, nullptr);
        vkDestroyPipeline(get_device().get_handle(), pipelines_.composition_, nullptr);
        vkDestroyPipeline(get_device().get_handle(), pipelines_.bloom_[0], nullptr);
        vkDestroyPipeline(get_device().get_handle(), pipelines_.bloom_[1], nullptr);
        
        vkDestroyPipelineLayout(get_device().get_handle(), pipeline_layouts_.models_, nullptr);
        vkDestroyPipelineLayout(get_device().get_handle(), pipeline_layouts_.composition_, nullptr);
        vkDestroyPipelineLayout(get_device().get_handle(), pipeline_layouts_.bloom_filter_, nullptr);
        
        vkDestroyDescriptorSetLayout(get_device().get_handle(), descriptor_set_layouts_.models_, nullptr);
        vkDestroyDescriptorSetLayout(get_device().get_handle(), descriptor_set_layouts_.composition_, nullptr);
        vkDestroyDescriptorSetLayout(get_device().get_handle(), descriptor_set_layouts_.bloom_filter_, nullptr);
        
        vkDestroyRenderPass(get_device().get_handle(), offscreen_.render_pass, nullptr);
        vkDestroyRenderPass(get_device().get_handle(), filter_pass_.render_pass_, nullptr);
        
        vkDestroyFramebuffer(get_device().get_handle(), offscreen_.framebuffer, nullptr);
        vkDestroyFramebuffer(get_device().get_handle(), filter_pass_.framebuffer_, nullptr);
        
        vkDestroySampler(get_device().get_handle(), offscreen_.sampler, nullptr);
        vkDestroySampler(get_device().get_handle(), filter_pass_.sampler_, nullptr);
        
        offscreen_.depth.destroy(get_device().get_handle());
        offscreen_.color[0].destroy(get_device().get_handle());
        offscreen_.color[1].destroy(get_device().get_handle());
        
        filter_pass_.color_[0].destroy(get_device().get_handle());
        
        vkDestroySampler(get_device().get_handle(), textures_.envmap_.sampler, nullptr);
    }
}

void HDR::request_gpu_features(vox::PhysicalDevice &gpu) {
    // Enable anisotropic filtering if supported
    if (gpu.get_features().samplerAnisotropy) {
        gpu.get_mutable_requested_features().samplerAnisotropy = VK_TRUE;
    }
}

void HDR::build_command_buffers() {
    VkCommandBufferBeginInfo command_buffer_begin_info = vox::initializers::command_buffer_begin_info();
    
    VkClearValue clear_values[2];
    clear_values[0].color = {{0.0f, 0.0f, 0.0f, 0.0f}};
    clear_values[1].depthStencil = {0.0f, 0};
    
    VkRenderPassBeginInfo render_pass_begin_info = vox::initializers::render_pass_begin_info();
    render_pass_begin_info.renderPass = render_pass_;
    render_pass_begin_info.renderArea.offset.x = 0;
    render_pass_begin_info.renderArea.offset.y = 0;
    render_pass_begin_info.clearValueCount = 2;
    render_pass_begin_info.pClearValues = clear_values;
    
    for (int32_t i = 0; i < draw_cmd_buffers_.size(); ++i) {
        VK_CHECK(vkBeginCommandBuffer(draw_cmd_buffers_[i], &command_buffer_begin_info));
        
        {
            /*
             First pass: Render scene to offscreen framebuffer
             */
            
            std::array<VkClearValue, 3> clear_values{};
            clear_values[0].color = {{0.0f, 0.0f, 0.0f, 0.0f}};
            clear_values[1].color = {{0.0f, 0.0f, 0.0f, 0.0f}};
            clear_values[2].depthStencil = {0.0f, 0};
            
            VkRenderPassBeginInfo render_pass_begin_info = vox::initializers::render_pass_begin_info();
            render_pass_begin_info.renderPass = offscreen_.render_pass;
            render_pass_begin_info.framebuffer = offscreen_.framebuffer;
            render_pass_begin_info.renderArea.extent.width = offscreen_.width;
            render_pass_begin_info.renderArea.extent.height = offscreen_.height;
            render_pass_begin_info.clearValueCount = 3;
            render_pass_begin_info.pClearValues = clear_values.data();
            
            vkCmdBeginRenderPass(draw_cmd_buffers_[i], &render_pass_begin_info, VK_SUBPASS_CONTENTS_INLINE);
            
            VkViewport viewport = vox::initializers::viewport((float) offscreen_.width, (float) offscreen_.height,
                                                              0.0f, 1.0f);
            vkCmdSetViewport(draw_cmd_buffers_[i], 0, 1, &viewport);
            
            VkRect2D scissor = vox::initializers::rect_2d(offscreen_.width, offscreen_.height, 0, 0);
            vkCmdSetScissor(draw_cmd_buffers_[i], 0, 1, &scissor);
            
            // Skybox
            if (display_skybox_) {
                vkCmdBindPipeline(draw_cmd_buffers_[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipelines_.skybox_);
                vkCmdBindDescriptorSets(draw_cmd_buffers_[i], VK_PIPELINE_BIND_POINT_GRAPHICS,
                                        pipeline_layouts_.models_, 0, 1, &descriptor_sets_.skybox_, 0, nullptr);
                
                draw_model(models_.skybox, draw_cmd_buffers_[i]);
            }
            
            // 3D object
            vkCmdBindPipeline(draw_cmd_buffers_[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipelines_.reflect_);
            vkCmdBindDescriptorSets(draw_cmd_buffers_[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline_layouts_.models_,
                                    0, 1, &descriptor_sets_.object_, 0, nullptr);
            
            draw_model(models_.objects[models_.object_index], draw_cmd_buffers_[i]);
            
            vkCmdEndRenderPass(draw_cmd_buffers_[i]);
        }
        
        /*
         Second render pass: First bloom pass
         */
        if (bloom_) {
            VkClearValue clear_values[2];
            clear_values[0].color = {{0.0f, 0.0f, 0.0f, 0.0f}};
            clear_values[1].depthStencil = {0.0f, 0};
            
            // Bloom filter
            VkRenderPassBeginInfo render_pass_begin_info = vox::initializers::render_pass_begin_info();
            render_pass_begin_info.framebuffer = filter_pass_.framebuffer_;
            render_pass_begin_info.renderPass = filter_pass_.render_pass_;
            render_pass_begin_info.clearValueCount = 1;
            render_pass_begin_info.renderArea.extent.width = filter_pass_.width_;
            render_pass_begin_info.renderArea.extent.height = filter_pass_.height_;
            render_pass_begin_info.pClearValues = clear_values;
            
            vkCmdBeginRenderPass(draw_cmd_buffers_[i], &render_pass_begin_info, VK_SUBPASS_CONTENTS_INLINE);
            
            VkViewport viewport = vox::initializers::viewport((float) filter_pass_.width_, (float) filter_pass_.height_,
                                                              0.0f, 1.0f);
            vkCmdSetViewport(draw_cmd_buffers_[i], 0, 1, &viewport);
            
            VkRect2D scissor = vox::initializers::rect_2d(filter_pass_.width_, filter_pass_.height_, 0, 0);
            vkCmdSetScissor(draw_cmd_buffers_[i], 0, 1, &scissor);
            
            vkCmdBindDescriptorSets(draw_cmd_buffers_[i], VK_PIPELINE_BIND_POINT_GRAPHICS,
                                    pipeline_layouts_.bloom_filter_, 0, 1, &descriptor_sets_.bloom_filter_, 0, nullptr);
            
            vkCmdBindPipeline(draw_cmd_buffers_[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipelines_.bloom_[1]);
            vkCmdDraw(draw_cmd_buffers_[i], 3, 1, 0, 0);
            
            vkCmdEndRenderPass(draw_cmd_buffers_[i]);
        }
        
        /*
         Note: Explicit synchronization is not required between the render pass, as this is done implicit via sub pass dependencies
         */
        
        /*
         Third render pass: Scene rendering with applied second bloom pass (when enabled)
         */
        {
            VkClearValue clear_values[2];
            clear_values[0].color = {{0.0f, 0.0f, 0.0f, 0.0f}};
            clear_values[1].depthStencil = {0.0f, 0};
            
            // Final composition
            VkRenderPassBeginInfo render_pass_begin_info = vox::initializers::render_pass_begin_info();
            render_pass_begin_info.framebuffer = framebuffers_[i];
            render_pass_begin_info.renderPass = render_pass_;
            render_pass_begin_info.clearValueCount = 2;
            render_pass_begin_info.renderArea.extent.width = width_;
            render_pass_begin_info.renderArea.extent.height = height_;
            render_pass_begin_info.pClearValues = clear_values;
            
            vkCmdBeginRenderPass(draw_cmd_buffers_[i], &render_pass_begin_info, VK_SUBPASS_CONTENTS_INLINE);
            
            VkViewport viewport = vox::initializers::viewport((float) width_, (float) height_, 0.0f, 1.0f);
            vkCmdSetViewport(draw_cmd_buffers_[i], 0, 1, &viewport);
            
            VkRect2D scissor = vox::initializers::rect_2d(width_, height_, 0, 0);
            vkCmdSetScissor(draw_cmd_buffers_[i], 0, 1, &scissor);
            
            vkCmdBindDescriptorSets(draw_cmd_buffers_[i], VK_PIPELINE_BIND_POINT_GRAPHICS,
                                    pipeline_layouts_.composition_, 0, 1, &descriptor_sets_.composition_, 0, nullptr);
            
            // Scene
            vkCmdBindPipeline(draw_cmd_buffers_[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipelines_.composition_);
            vkCmdDraw(draw_cmd_buffers_[i], 3, 1, 0, 0);
            
            // Bloom
            if (bloom_) {
                vkCmdBindPipeline(draw_cmd_buffers_[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipelines_.bloom_[0]);
                vkCmdDraw(draw_cmd_buffers_[i], 3, 1, 0, 0);
            }
            
            draw_ui(draw_cmd_buffers_[i]);
            
            vkCmdEndRenderPass(draw_cmd_buffers_[i]);
        }
        
        VK_CHECK(vkEndCommandBuffer(draw_cmd_buffers_[i]));
    }
}

void HDR::create_attachment(VkFormat format, VkImageUsageFlagBits usage, FrameBufferAttachment *attachment) {
    VkImageAspectFlags aspect_mask = 0;
    VkImageLayout image_layout;
    
    attachment->format = format;
    
    if (usage & VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT) {
        aspect_mask = VK_IMAGE_ASPECT_COLOR_BIT;
        image_layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    }
    if (usage & VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT) {
        aspect_mask = VK_IMAGE_ASPECT_DEPTH_BIT;
        // Stencil aspect should only be set on depth + stencil formats (VK_FORMAT_D16_UNORM_S8_UINT..VK_FORMAT_D32_SFLOAT_S8_UINT
        if (format >= VK_FORMAT_D16_UNORM_S8_UINT) {
            aspect_mask |= VK_IMAGE_ASPECT_STENCIL_BIT;
        }
        image_layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    }
    
    assert(aspect_mask > 0);
    
    VkImageCreateInfo image = vox::initializers::image_create_info();
    image.imageType = VK_IMAGE_TYPE_2D;
    image.format = format;
    image.extent.width = offscreen_.width;
    image.extent.height = offscreen_.height;
    image.extent.depth = 1;
    image.mipLevels = 1;
    image.arrayLayers = 1;
    image.samples = VK_SAMPLE_COUNT_1_BIT;
    image.tiling = VK_IMAGE_TILING_OPTIMAL;
    image.usage = usage | VK_IMAGE_USAGE_SAMPLED_BIT;
    
    VkMemoryAllocateInfo memory_allocate_info = vox::initializers::memory_allocate_info();
    VkMemoryRequirements memory_requirements;
    
    VK_CHECK(vkCreateImage(get_device().get_handle(), &image, nullptr, &attachment->image));
    vkGetImageMemoryRequirements(get_device().get_handle(), attachment->image, &memory_requirements);
    memory_allocate_info.allocationSize = memory_requirements.size;
    memory_allocate_info.memoryTypeIndex = get_device().get_memory_type(memory_requirements.memoryTypeBits,
                                                                        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    VK_CHECK(vkAllocateMemory(get_device().get_handle(), &memory_allocate_info, nullptr, &attachment->mem));
    VK_CHECK(vkBindImageMemory(get_device().get_handle(), attachment->image, attachment->mem, 0));
    
    VkImageViewCreateInfo image_view_create_info = vox::initializers::image_view_create_info();
    image_view_create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
    image_view_create_info.format = format;
    image_view_create_info.subresourceRange = {};
    image_view_create_info.subresourceRange.aspectMask = aspect_mask;
    image_view_create_info.subresourceRange.baseMipLevel = 0;
    image_view_create_info.subresourceRange.levelCount = 1;
    image_view_create_info.subresourceRange.baseArrayLayer = 0;
    image_view_create_info.subresourceRange.layerCount = 1;
    image_view_create_info.image = attachment->image;
    VK_CHECK(vkCreateImageView(get_device().get_handle(), &image_view_create_info, nullptr, &attachment->view));
}

// Prepare a new framebuffer and attachments for offscreen rendering (G-Buffer)
void HDR::prepare_offscreen_buffer() {
    {
        offscreen_.width = width_;
        offscreen_.height = height_;
        
        // Color attachments
        
        // We are using two 128-Bit RGBA floating point color buffers for this sample
        // In a performance or bandwith-limited scenario you should consider using a format with lower precision
        create_attachment(VK_FORMAT_R32G32B32A32_SFLOAT, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, &offscreen_.color[0]);
        create_attachment(VK_FORMAT_R32G32B32A32_SFLOAT, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, &offscreen_.color[1]);
        // Depth attachment
        create_attachment(depth_format_, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, &offscreen_.depth);
        
        // Set up separate renderpass with references to the colorand depth attachments
        std::array<VkAttachmentDescription, 3> attachment_descriptions = {};
        
        // Init attachment properties
        for (uint32_t i = 0; i < 3; ++i) {
            attachment_descriptions[i].samples = VK_SAMPLE_COUNT_1_BIT;
            attachment_descriptions[i].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
            attachment_descriptions[i].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
            attachment_descriptions[i].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            attachment_descriptions[i].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
            if (i == 2) {
                attachment_descriptions[i].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
                attachment_descriptions[i].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
            } else {
                attachment_descriptions[i].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
                attachment_descriptions[i].finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            }
        }
        
        // Formats
        attachment_descriptions[0].format = offscreen_.color[0].format;
        attachment_descriptions[1].format = offscreen_.color[1].format;
        attachment_descriptions[2].format = offscreen_.depth.format;
        
        std::vector<VkAttachmentReference> color_references;
        color_references.push_back({0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL});
        color_references.push_back({1, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL});
        
        VkAttachmentReference depth_reference = {};
        depth_reference.attachment = 2;
        depth_reference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        
        VkSubpassDescription subpass = {};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.pColorAttachments = color_references.data();
        subpass.colorAttachmentCount = 2;
        subpass.pDepthStencilAttachment = &depth_reference;
        
        // Use subpass dependencies for attachment layput transitions
        std::array<VkSubpassDependency, 2> dependencies{};
        
        dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
        dependencies[0].dstSubpass = 0;
        dependencies[0].srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
        dependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependencies[0].srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
        dependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;
        
        dependencies[1].srcSubpass = 0;
        dependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
        dependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependencies[1].dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
        dependencies[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        dependencies[1].dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
        dependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;
        
        VkRenderPassCreateInfo render_pass_create_info = {};
        render_pass_create_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        render_pass_create_info.pAttachments = attachment_descriptions.data();
        render_pass_create_info.attachmentCount = static_cast<uint32_t>(attachment_descriptions.size());
        render_pass_create_info.subpassCount = 1;
        render_pass_create_info.pSubpasses = &subpass;
        render_pass_create_info.dependencyCount = 2;
        render_pass_create_info.pDependencies = dependencies.data();
        
        VK_CHECK(vkCreateRenderPass(get_device().get_handle(), &render_pass_create_info, nullptr,
                                    &offscreen_.render_pass));
        
        std::array<VkImageView, 3> attachments{};
        attachments[0] = offscreen_.color[0].view;
        attachments[1] = offscreen_.color[1].view;
        attachments[2] = offscreen_.depth.view;
        
        VkFramebufferCreateInfo framebuffer_create_info = {};
        framebuffer_create_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebuffer_create_info.pNext = nullptr;
        framebuffer_create_info.renderPass = offscreen_.render_pass;
        framebuffer_create_info.pAttachments = attachments.data();
        framebuffer_create_info.attachmentCount = static_cast<uint32_t>(attachments.size());
        framebuffer_create_info.width = offscreen_.width;
        framebuffer_create_info.height = offscreen_.height;
        framebuffer_create_info.layers = 1;
        VK_CHECK(vkCreateFramebuffer(get_device().get_handle(), &framebuffer_create_info, nullptr,
                                     &offscreen_.framebuffer));
        
        // Create sampler to sample from the color attachments
        VkSamplerCreateInfo sampler = vox::initializers::sampler_create_info();
        sampler.magFilter = VK_FILTER_NEAREST;
        sampler.minFilter = VK_FILTER_NEAREST;
        sampler.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        sampler.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        sampler.addressModeV = sampler.addressModeU;
        sampler.addressModeW = sampler.addressModeU;
        sampler.mipLodBias = 0.0f;
        sampler.maxAnisotropy = 1.0f;
        sampler.minLod = 0.0f;
        sampler.maxLod = 1.0f;
        sampler.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
        VK_CHECK(vkCreateSampler(get_device().get_handle(), &sampler, nullptr, &offscreen_.sampler));
    }
    
    // Bloom separable filter pass
    {
        filter_pass_.width_ = width_;
        filter_pass_.height_ = height_;
        
        // Color attachments
        
        // Two floating point color buffers
        create_attachment(VK_FORMAT_R32G32B32A32_SFLOAT, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
                          &filter_pass_.color_[0]);
        
        // Set up separate renderpass with references to the colorand depth attachments
        std::array<VkAttachmentDescription, 1> attachment_descriptions = {};
        
        // Init attachment properties
        attachment_descriptions[0].samples = VK_SAMPLE_COUNT_1_BIT;
        attachment_descriptions[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        attachment_descriptions[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        attachment_descriptions[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        attachment_descriptions[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        attachment_descriptions[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        attachment_descriptions[0].finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        attachment_descriptions[0].format = filter_pass_.color_[0].format;
        
        std::vector<VkAttachmentReference> color_references;
        color_references.push_back({0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL});
        
        VkSubpassDescription subpass = {};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.pColorAttachments = color_references.data();
        subpass.colorAttachmentCount = 1;
        
        // Use subpass dependencies for attachment layput transitions
        std::array<VkSubpassDependency, 2> dependencies{};
        
        dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
        dependencies[0].dstSubpass = 0;
        dependencies[0].srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
        dependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependencies[0].srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
        dependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;
        
        dependencies[1].srcSubpass = 0;
        dependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
        dependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependencies[1].dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
        dependencies[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        dependencies[1].dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
        dependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;
        
        VkRenderPassCreateInfo render_pass_create_info = {};
        render_pass_create_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        render_pass_create_info.pAttachments = attachment_descriptions.data();
        render_pass_create_info.attachmentCount = static_cast<uint32_t>(attachment_descriptions.size());
        render_pass_create_info.subpassCount = 1;
        render_pass_create_info.pSubpasses = &subpass;
        render_pass_create_info.dependencyCount = 2;
        render_pass_create_info.pDependencies = dependencies.data();
        
        VK_CHECK(vkCreateRenderPass(get_device().get_handle(), &render_pass_create_info, nullptr,
                                    &filter_pass_.render_pass_));
        
        std::array<VkImageView, 1> attachments{};
        attachments[0] = filter_pass_.color_[0].view;
        
        VkFramebufferCreateInfo framebuffer_create_info = {};
        framebuffer_create_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebuffer_create_info.pNext = nullptr;
        framebuffer_create_info.renderPass = filter_pass_.render_pass_;
        framebuffer_create_info.pAttachments = attachments.data();
        framebuffer_create_info.attachmentCount = static_cast<uint32_t>(attachments.size());
        framebuffer_create_info.width = filter_pass_.width_;
        framebuffer_create_info.height = filter_pass_.height_;
        framebuffer_create_info.layers = 1;
        VK_CHECK(vkCreateFramebuffer(get_device().get_handle(), &framebuffer_create_info, nullptr,
                                     &filter_pass_.framebuffer_));
        
        // Create sampler to sample from the color attachments
        VkSamplerCreateInfo sampler = vox::initializers::sampler_create_info();
        sampler.magFilter = VK_FILTER_NEAREST;
        sampler.minFilter = VK_FILTER_NEAREST;
        sampler.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        sampler.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        sampler.addressModeV = sampler.addressModeU;
        sampler.addressModeW = sampler.addressModeU;
        sampler.mipLodBias = 0.0f;
        sampler.maxAnisotropy = 1.0f;
        sampler.minLod = 0.0f;
        sampler.maxLod = 1.0f;
        sampler.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
        VK_CHECK(vkCreateSampler(get_device().get_handle(), &sampler, nullptr, &filter_pass_.sampler_));
    }
}

void HDR::load_assets() {
    // Models
    models_.skybox = load_model("Scenes/cube.gltf");
    std::vector<std::string> filenames = {"geosphere.gltf", "teapot.gltf", "torusknot.gltf"};
    object_names_ = {"Sphere", "Teapot", "Torusknot"};
    for (const auto& file: filenames) {
        auto object = load_model("Scenes/" + file);
        models_.objects.emplace_back(std::move(object));
    }
    
    // Transforms
    auto geosphere_matrix = Matrix4x4F();
    auto teapot_matrix = Matrix4x4F();
    teapot_matrix *= makeScaleMatrix(10.0f, 10.0f, 10.0f);
    teapot_matrix *= makeRotationMatrix(Vector3F(1.0f, 0.0f, 0.0f), degreesToRadians(180.0f));
    auto torus_matrix = Matrix4x4F();
    models_.transforms.push_back(geosphere_matrix);
    models_.transforms.push_back(teapot_matrix);
    models_.transforms.push_back(torus_matrix);
    
    // Load HDR cube map
    textures_.envmap_ = load_texture_cubemap("Textures/uffizi_rgba16f_cube.ktx");
}

void HDR::setup_descriptor_pool() {
    std::vector<VkDescriptorPoolSize> pool_sizes = {
        vox::initializers::descriptor_pool_size(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 4),
        vox::initializers::descriptor_pool_size(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 6)};
    uint32_t num_descriptor_sets = 4;
    VkDescriptorPoolCreateInfo descriptor_pool_create_info =
    vox::initializers::descriptor_pool_create_info(static_cast<uint32_t>(pool_sizes.size()),
                                                   pool_sizes.data(), num_descriptor_sets);
    VK_CHECK(vkCreateDescriptorPool(get_device().get_handle(), &descriptor_pool_create_info, nullptr,
                                    &descriptor_pool_));
}

void HDR::setup_descriptor_set_layout() {
    std::vector<VkDescriptorSetLayoutBinding> set_layout_bindings = {
        vox::initializers::descriptor_set_layout_binding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                                                         VK_SHADER_STAGE_VERTEX_BIT, 0),
        vox::initializers::descriptor_set_layout_binding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                                                         VK_SHADER_STAGE_FRAGMENT_BIT, 1),
        vox::initializers::descriptor_set_layout_binding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                                                         VK_SHADER_STAGE_FRAGMENT_BIT, 2),
    };
    
    VkDescriptorSetLayoutCreateInfo descriptor_layout_create_info =
    vox::initializers::descriptor_set_layout_create_info(set_layout_bindings.data(),
                                                         static_cast<uint32_t>(set_layout_bindings.size()));
    
    VK_CHECK(vkCreateDescriptorSetLayout(get_device().get_handle(), &descriptor_layout_create_info, nullptr,
                                         &descriptor_set_layouts_.models_));
    
    VkPipelineLayoutCreateInfo pipeline_layout_create_info =
    vox::initializers::pipeline_layout_create_info(
                                                   &descriptor_set_layouts_.models_,
                                                   1);
    
    VK_CHECK(vkCreatePipelineLayout(get_device().get_handle(), &pipeline_layout_create_info, nullptr,
                                    &pipeline_layouts_.models_));
    
    // Bloom filter
    set_layout_bindings = {
        vox::initializers::descriptor_set_layout_binding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                                                         VK_SHADER_STAGE_FRAGMENT_BIT, 0),
        vox::initializers::descriptor_set_layout_binding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                                                         VK_SHADER_STAGE_FRAGMENT_BIT, 1),
    };
    
    descriptor_layout_create_info = vox::initializers::descriptor_set_layout_create_info(set_layout_bindings.data(),
                                                                                         static_cast<uint32_t>(set_layout_bindings.size()));
    VK_CHECK(vkCreateDescriptorSetLayout(get_device().get_handle(), &descriptor_layout_create_info, nullptr,
                                         &descriptor_set_layouts_.bloom_filter_));
    
    pipeline_layout_create_info = vox::initializers::pipeline_layout_create_info(
                                                                                 &descriptor_set_layouts_.bloom_filter_, 1);
    VK_CHECK(vkCreatePipelineLayout(get_device().get_handle(), &pipeline_layout_create_info, nullptr,
                                    &pipeline_layouts_.bloom_filter_));
    
    // G-Buffer composition
    set_layout_bindings = {
        vox::initializers::descriptor_set_layout_binding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                                                         VK_SHADER_STAGE_FRAGMENT_BIT, 0),
        vox::initializers::descriptor_set_layout_binding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                                                         VK_SHADER_STAGE_FRAGMENT_BIT, 1),
    };
    
    descriptor_layout_create_info = vox::initializers::descriptor_set_layout_create_info(set_layout_bindings.data(),
                                                                                         static_cast<uint32_t>(set_layout_bindings.size()));
    VK_CHECK(vkCreateDescriptorSetLayout(get_device().get_handle(), &descriptor_layout_create_info, nullptr,
                                         &descriptor_set_layouts_.composition_));
    
    pipeline_layout_create_info = vox::initializers::pipeline_layout_create_info(
                                                                                 &descriptor_set_layouts_.composition_, 1);
    VK_CHECK(vkCreatePipelineLayout(get_device().get_handle(), &pipeline_layout_create_info, nullptr,
                                    &pipeline_layouts_.composition_));
}

void HDR::setup_descriptor_sets() {
    VkDescriptorSetAllocateInfo alloc_info =
    vox::initializers::descriptor_set_allocate_info(
                                                    descriptor_pool_,
                                                    &descriptor_set_layouts_.models_,
                                                    1);
    
    // 3D object descriptor set
    VK_CHECK(vkAllocateDescriptorSets(get_device().get_handle(), &alloc_info, &descriptor_sets_.object_));
    
    VkDescriptorBufferInfo matrix_buffer_descriptor = create_descriptor(*uniform_buffers_.matrices_);
    VkDescriptorImageInfo environment_image_descriptor = create_descriptor(textures_.envmap_);
    VkDescriptorBufferInfo params_buffer_descriptor = create_descriptor(*uniform_buffers_.params_);
    std::vector<VkWriteDescriptorSet> write_descriptor_sets = {
        vox::initializers::write_descriptor_set(descriptor_sets_.object_, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 0,
                                                &matrix_buffer_descriptor),
        vox::initializers::write_descriptor_set(descriptor_sets_.object_,
                                                VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1,
                                                &environment_image_descriptor),
        vox::initializers::write_descriptor_set(descriptor_sets_.object_, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 2,
                                                &params_buffer_descriptor),
    };
    vkUpdateDescriptorSets(get_device().get_handle(), static_cast<uint32_t>(write_descriptor_sets.size()),
                           write_descriptor_sets.data(), 0, nullptr);
    
    // Sky box descriptor set
    VK_CHECK(vkAllocateDescriptorSets(get_device().get_handle(), &alloc_info, &descriptor_sets_.skybox_));
    
    matrix_buffer_descriptor = create_descriptor(*uniform_buffers_.matrices_);
    environment_image_descriptor = create_descriptor(textures_.envmap_);
    params_buffer_descriptor = create_descriptor(*uniform_buffers_.params_);
    write_descriptor_sets = {
        vox::initializers::write_descriptor_set(descriptor_sets_.skybox_, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 0,
                                                &matrix_buffer_descriptor),
        vox::initializers::write_descriptor_set(descriptor_sets_.skybox_,
                                                VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1,
                                                &environment_image_descriptor),
        vox::initializers::write_descriptor_set(descriptor_sets_.skybox_, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 2,
                                                &params_buffer_descriptor),
    };
    vkUpdateDescriptorSets(get_device().get_handle(), static_cast<uint32_t>(write_descriptor_sets.size()),
                           write_descriptor_sets.data(), 0, nullptr);
    
    // Bloom filter
    alloc_info = vox::initializers::descriptor_set_allocate_info(descriptor_pool_,
                                                                 &descriptor_set_layouts_.bloom_filter_, 1);
    VK_CHECK(vkAllocateDescriptorSets(get_device().get_handle(), &alloc_info, &descriptor_sets_.bloom_filter_));
    
    std::vector<VkDescriptorImageInfo> color_descriptors = {
        vox::initializers::descriptor_image_info(offscreen_.sampler, offscreen_.color[0].view,
                                                 VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL),
        vox::initializers::descriptor_image_info(offscreen_.sampler, offscreen_.color[1].view,
                                                 VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL),
    };
    
    write_descriptor_sets = {
        vox::initializers::write_descriptor_set(descriptor_sets_.bloom_filter_,
                                                VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 0,
                                                &color_descriptors[0]),
        vox::initializers::write_descriptor_set(descriptor_sets_.bloom_filter_,
                                                VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1,
                                                &color_descriptors[1]),
    };
    vkUpdateDescriptorSets(get_device().get_handle(), static_cast<uint32_t>(write_descriptor_sets.size()),
                           write_descriptor_sets.data(), 0, nullptr);
    
    // Composition descriptor set
    alloc_info = vox::initializers::descriptor_set_allocate_info(descriptor_pool_,
                                                                 &descriptor_set_layouts_.composition_, 1);
    VK_CHECK(vkAllocateDescriptorSets(get_device().get_handle(), &alloc_info, &descriptor_sets_.composition_));
    
    color_descriptors = {
        vox::initializers::descriptor_image_info(offscreen_.sampler, offscreen_.color[0].view,
                                                 VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL),
        vox::initializers::descriptor_image_info(offscreen_.sampler, filter_pass_.color_[0].view,
                                                 VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL),
    };
    
    write_descriptor_sets = {
        vox::initializers::write_descriptor_set(descriptor_sets_.composition_,
                                                VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 0,
                                                &color_descriptors[0]),
        vox::initializers::write_descriptor_set(descriptor_sets_.composition_,
                                                VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1,
                                                &color_descriptors[1]),
    };
    vkUpdateDescriptorSets(get_device().get_handle(), static_cast<uint32_t>(write_descriptor_sets.size()),
                           write_descriptor_sets.data(), 0, nullptr);
}

void HDR::prepare_pipelines() {
    VkPipelineInputAssemblyStateCreateInfo input_assembly_state =
    vox::initializers::pipeline_input_assembly_state_create_info(
                                                                 VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
                                                                 0,
                                                                 VK_FALSE);
    
    VkPipelineRasterizationStateCreateInfo rasterization_state =
    vox::initializers::pipeline_rasterization_state_create_info(
                                                                VK_POLYGON_MODE_FILL,
                                                                VK_CULL_MODE_BACK_BIT,
                                                                VK_FRONT_FACE_COUNTER_CLOCKWISE,
                                                                0);
    
    VkPipelineColorBlendAttachmentState blend_attachment_state =
    vox::initializers::pipeline_color_blend_attachment_state(
                                                             0xf,
                                                             VK_FALSE);
    
    VkPipelineColorBlendStateCreateInfo color_blend_state =
    vox::initializers::pipeline_color_blend_state_create_info(
                                                              1,
                                                              &blend_attachment_state);
    
    // Note: Using Reversed depth-buffer for increased precision, so Greater depth values are kept
    VkPipelineDepthStencilStateCreateInfo depth_stencil_state =
    vox::initializers::pipeline_depth_stencil_state_create_info(
                                                                VK_FALSE,
                                                                VK_FALSE,
                                                                VK_COMPARE_OP_GREATER);
    
    VkPipelineViewportStateCreateInfo viewport_state =
    vox::initializers::pipeline_viewport_state_create_info(1, 1, 0);
    
    VkPipelineMultisampleStateCreateInfo multisample_state =
    vox::initializers::pipeline_multisample_state_create_info(
                                                              VK_SAMPLE_COUNT_1_BIT,
                                                              0);
    
    std::vector<VkDynamicState> dynamic_state_enables = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR};
    VkPipelineDynamicStateCreateInfo dynamic_state =
    vox::initializers::pipeline_dynamic_state_create_info(
                                                          dynamic_state_enables.data(),
                                                          static_cast<uint32_t>(dynamic_state_enables.size()),
                                                          0);
    
    VkGraphicsPipelineCreateInfo pipeline_create_info =
    vox::initializers::pipeline_create_info(
                                            pipeline_layouts_.models_,
                                            render_pass_,
                                            0);
    
    std::vector<VkPipelineColorBlendAttachmentState> blend_attachment_states = {
        vox::initializers::pipeline_color_blend_attachment_state(0xf, VK_FALSE),
        vox::initializers::pipeline_color_blend_attachment_state(0xf, VK_FALSE),
    };
    
    pipeline_create_info.pInputAssemblyState = &input_assembly_state;
    pipeline_create_info.pRasterizationState = &rasterization_state;
    pipeline_create_info.pColorBlendState = &color_blend_state;
    pipeline_create_info.pMultisampleState = &multisample_state;
    pipeline_create_info.pViewportState = &viewport_state;
    pipeline_create_info.pDepthStencilState = &depth_stencil_state;
    pipeline_create_info.pDynamicState = &dynamic_state;
    
    std::array<VkPipelineShaderStageCreateInfo, 2> shader_stages{};
    pipeline_create_info.stageCount = static_cast<uint32_t>(shader_stages.size());
    pipeline_create_info.pStages = shader_stages.data();
    
    VkSpecializationInfo specialization_info;
    std::array<VkSpecializationMapEntry, 1> specialization_map_entries{};
    
    // Full screen pipelines
    
    // Empty vertex input state, full screen triangles are generated by the vertex shader
    VkPipelineVertexInputStateCreateInfo empty_input_state = vox::initializers::pipeline_vertex_input_state_create_info();
    pipeline_create_info.pVertexInputState = &empty_input_state;
    
    // Final fullscreen composition pass pipeline
    shader_stages[0] = load_shader("hdr/composition.vert", VK_SHADER_STAGE_VERTEX_BIT);
    shader_stages[1] = load_shader("hdr/composition.frag", VK_SHADER_STAGE_FRAGMENT_BIT);
    pipeline_create_info.layout = pipeline_layouts_.composition_;
    pipeline_create_info.renderPass = render_pass_;
    rasterization_state.cullMode = VK_CULL_MODE_FRONT_BIT;
    color_blend_state.attachmentCount = 1;
    color_blend_state.pAttachments = blend_attachment_states.data();
    VK_CHECK(vkCreateGraphicsPipelines(get_device().get_handle(), pipeline_cache_, 1, &pipeline_create_info, nullptr,
                                       &pipelines_.composition_));
    
    // Bloom pass
    shader_stages[0] = load_shader("hdr/bloom.vert", VK_SHADER_STAGE_VERTEX_BIT);
    shader_stages[1] = load_shader("hdr/bloom.frag", VK_SHADER_STAGE_FRAGMENT_BIT);
    color_blend_state.pAttachments = &blend_attachment_state;
    blend_attachment_state.colorWriteMask = 0xF;
    blend_attachment_state.blendEnable = VK_TRUE;
    blend_attachment_state.colorBlendOp = VK_BLEND_OP_ADD;
    blend_attachment_state.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
    blend_attachment_state.dstColorBlendFactor = VK_BLEND_FACTOR_ONE;
    blend_attachment_state.alphaBlendOp = VK_BLEND_OP_ADD;
    blend_attachment_state.srcAlphaBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    blend_attachment_state.dstAlphaBlendFactor = VK_BLEND_FACTOR_DST_ALPHA;
    
    // Set constant parameters via specialization constants
    specialization_map_entries[0] = vox::initializers::specialization_map_entry(0, 0, sizeof(uint32_t));
    uint32_t dir = 1;
    specialization_info = vox::initializers::specialization_info(1, specialization_map_entries.data(), sizeof(dir),
                                                                 &dir);
    shader_stages[1].pSpecializationInfo = &specialization_info;
    
    VK_CHECK(vkCreateGraphicsPipelines(get_device().get_handle(), pipeline_cache_, 1, &pipeline_create_info, nullptr,
                                       &pipelines_.bloom_[0]));
    
    // Second blur pass (into separate framebuffer)
    pipeline_create_info.renderPass = filter_pass_.render_pass_;
    dir = 0;
    VK_CHECK(vkCreateGraphicsPipelines(get_device().get_handle(), pipeline_cache_, 1, &pipeline_create_info, nullptr,
                                       &pipelines_.bloom_[1]));
    
    // Object rendering pipelines
    rasterization_state.cullMode = VK_CULL_MODE_BACK_BIT;
    
    // Vertex bindings an attributes for model rendering
    // Binding description
    std::vector<VkVertexInputBindingDescription> vertex_input_bindings = {
        vox::initializers::vertex_input_binding_description(0, sizeof(Vertex), VK_VERTEX_INPUT_RATE_VERTEX),
    };
    
    // Attribute descriptions
    std::vector<VkVertexInputAttributeDescription> vertex_input_attributes = {
        vox::initializers::vertex_input_attribute_description(0, 0, VK_FORMAT_R32G32B32_SFLOAT,
                                                              0),                       // Position
        vox::initializers::vertex_input_attribute_description(0, 1, VK_FORMAT_R32G32B32_SFLOAT,
                                                              sizeof(float) * 3)        // Normal
    };
    
    VkPipelineVertexInputStateCreateInfo vertex_input_state = vox::initializers::pipeline_vertex_input_state_create_info();
    vertex_input_state.vertexBindingDescriptionCount = static_cast<uint32_t>(vertex_input_bindings.size());
    vertex_input_state.pVertexBindingDescriptions = vertex_input_bindings.data();
    vertex_input_state.vertexAttributeDescriptionCount = static_cast<uint32_t>(vertex_input_attributes.size());
    vertex_input_state.pVertexAttributeDescriptions = vertex_input_attributes.data();
    
    pipeline_create_info.pVertexInputState = &vertex_input_state;
    
    // Skybox pipeline (background cube)
    blend_attachment_state.blendEnable = VK_FALSE;
    pipeline_create_info.layout = pipeline_layouts_.models_;
    pipeline_create_info.renderPass = offscreen_.render_pass;
    color_blend_state.attachmentCount = 2;
    color_blend_state.pAttachments = blend_attachment_states.data();
    
    shader_stages[0] = load_shader("hdr/gbuffer.vert", VK_SHADER_STAGE_VERTEX_BIT);
    shader_stages[1] = load_shader("hdr/gbuffer.frag", VK_SHADER_STAGE_FRAGMENT_BIT);
    
    // Set constant parameters via specialization constants
    specialization_map_entries[0] = vox::initializers::specialization_map_entry(0, 0, sizeof(uint32_t));
    uint32_t shadertype = 0;
    specialization_info = vox::initializers::specialization_info(1, specialization_map_entries.data(),
                                                                 sizeof(shadertype), &shadertype);
    shader_stages[0].pSpecializationInfo = &specialization_info;
    shader_stages[1].pSpecializationInfo = &specialization_info;
    
    VK_CHECK(vkCreateGraphicsPipelines(get_device().get_handle(), pipeline_cache_, 1, &pipeline_create_info, nullptr,
                                       &pipelines_.skybox_));
    
    // Object rendering pipeline
    shadertype = 1;
    
    // Enable depth test and write
    depth_stencil_state.depthWriteEnable = VK_TRUE;
    depth_stencil_state.depthTestEnable = VK_TRUE;
    // Flip cull mode
    rasterization_state.cullMode = VK_CULL_MODE_FRONT_BIT;
    VK_CHECK(vkCreateGraphicsPipelines(get_device().get_handle(), pipeline_cache_, 1, &pipeline_create_info, nullptr,
                                       &pipelines_.reflect_));
}

// Prepare and initialize uniform buffer containing shader uniforms
void HDR::prepare_uniform_buffers() {
    // Matrices vertex shader uniform buffer
    uniform_buffers_.matrices_ = std::make_unique<vox::core::Buffer>(get_device(),
                                                                   sizeof(ubo_vs_),
                                                                   VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                                                                   VMA_MEMORY_USAGE_CPU_TO_GPU);
    
    // Params
    uniform_buffers_.params_ = std::make_unique<vox::core::Buffer>(get_device(),
                                                                 sizeof(ubo_params_),
                                                                 VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                                                                 VMA_MEMORY_USAGE_CPU_TO_GPU);
    
    update_uniform_buffers();
    update_params();
}

void HDR::update_uniform_buffers() {
    ubo_vs_.projection = camera_.matrices_.perspective_;
    ubo_vs_.modelview = camera_.matrices_.view_ * models_.transforms[models_.object_index];
    ubo_vs_.skybox_modelview = camera_.matrices_.view_;
    uniform_buffers_.matrices_->convert_and_update(ubo_vs_);
}

void HDR::update_params() const {
    uniform_buffers_.params_->convert_and_update(ubo_params_);
}

void HDR::draw() {
    ApiVulkanSample::prepare_frame();
    submit_info_.commandBufferCount = 1;
    submit_info_.pCommandBuffers = &draw_cmd_buffers_[current_buffer_];
    VK_CHECK(vkQueueSubmit(queue_, 1, &submit_info_, VK_NULL_HANDLE));
    ApiVulkanSample::submit_frame();
}

bool HDR::prepare(vox::Platform &platform) {
    if (!ApiVulkanSample::prepare(platform)) {
        return false;
    }
    
    camera_.type_ = vox::CameraType::LOOK_AT;
    camera_.set_position(Point3F(0.0f, 0.0f, -4.0f));
    camera_.set_rotation(Vector3F(0.0f, 180.0f, 0.0f));
    
    // Note: Using Revsered depth-buffer for increased precision, so Znear and Zfar are flipped
    camera_.set_perspective(60.0f, (float) width_ / (float) height_, 256.0f, 0.1f);
    
    load_assets();
    prepare_uniform_buffers();
    prepare_offscreen_buffer();
    setup_descriptor_set_layout();
    prepare_pipelines();
    setup_descriptor_pool();
    setup_descriptor_sets();
    build_command_buffers();
    prepared_ = true;
    return true;
}

void HDR::render(float delta_time) {
    if (!prepared_)
        return;
    draw();
    if (camera_.updated_)
        update_uniform_buffers();
}

void HDR::on_update_ui_overlay(vox::Drawer &drawer) {
    if (vox::Drawer::header("Settings")) {
        if (drawer.combo_box("Object type", &models_.object_index, object_names_)) {
            update_uniform_buffers();
            build_command_buffers();
        }
        if (drawer.input_float("Exposure", &ubo_params_.exposure, 0.025f)) {
            update_params();
        }
        if (drawer.checkbox("Bloom", &bloom_)) {
            build_command_buffers();
        }
        if (drawer.checkbox("Skybox", &display_skybox_)) {
            build_command_buffers();
        }
    }
}

bool HDR::resize(const uint32_t width, const uint32_t height) {
    ApiVulkanSample::resize(width, height);
    update_uniform_buffers();
    return true;
}

std::unique_ptr<vox::Application> create_hdr() {
    return std::make_unique<HDR>();
}

}
