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

/*
 * High dynamic range rendering
 */

#pragma once

#include "api_vulkan_sample.h"
#include "matrix4x4.h"

namespace vox {
class HDR : public ApiVulkanSample {
public:
    bool bloom_ = true;
    bool display_skybox_ = true;
    
    struct {
        Texture envmap_;
    } textures_;
    
    struct Models {
        std::unique_ptr<vox::sg::SubMesh> skybox;
        std::vector<std::unique_ptr<vox::sg::SubMesh>> objects;
        std::vector<Matrix4x4F> transforms;
        int32_t object_index = 0;
    } models_;
    
    struct {
        std::unique_ptr<vox::core::Buffer> matrices_;
        std::unique_ptr<vox::core::Buffer> params_;
    } uniform_buffers_;
    
    struct UBOVS {
        Matrix4x4F projection;
        Matrix4x4F modelview;
        Matrix4x4F skybox_modelview;
        float modelscale = 0.05f;
    } ubo_vs_;
    
    struct UBOParams {
        float exposure = 1.0f;
    } ubo_params_;
    
    struct {
        VkPipeline skybox_;
        VkPipeline reflect_;
        VkPipeline composition_;
        VkPipeline bloom_[2];
    } pipelines_{};
    
    struct {
        VkPipelineLayout models_;
        VkPipelineLayout composition_;
        VkPipelineLayout bloom_filter_;
    } pipeline_layouts_{};
    
    struct {
        VkDescriptorSet object_;
        VkDescriptorSet skybox_;
        VkDescriptorSet composition_;
        VkDescriptorSet bloom_filter_;
    } descriptor_sets_{};
    
    struct {
        VkDescriptorSetLayout models_;
        VkDescriptorSetLayout composition_;
        VkDescriptorSetLayout bloom_filter_;
    } descriptor_set_layouts_{};
    
    // Framebuffer for offscreen rendering
    struct FrameBufferAttachment {
        VkImage image;
        VkDeviceMemory mem;
        VkImageView view;
        VkFormat format;
        
        void destroy(VkDevice device) const {
            vkDestroyImageView(device, view, nullptr);
            vkDestroyImage(device, image, nullptr);
            vkFreeMemory(device, mem, nullptr);
        }
    };
    
    struct FrameBuffer {
        int32_t width, height;
        VkFramebuffer framebuffer;
        FrameBufferAttachment color[2];
        FrameBufferAttachment depth;
        VkRenderPass render_pass;
        VkSampler sampler;
    } offscreen_{};
    
    struct {
        int32_t width_, height_;
        VkFramebuffer framebuffer_;
        FrameBufferAttachment color_[1];
        VkRenderPass render_pass_;
        VkSampler sampler_;
    } filter_pass_{};
    
    std::vector<std::string> object_names_;
    
    HDR();
    
    ~HDR() override;
    
    void request_gpu_features(vox::PhysicalDevice &gpu) override;
    
    void build_command_buffers() override;
    
    void create_attachment(VkFormat format, VkImageUsageFlagBits usage, FrameBufferAttachment *attachment);
    
    void prepare_offscreen_buffer();
    
    void load_assets();
    
    void setup_descriptor_pool();
    
    void setup_descriptor_set_layout();
    
    void setup_descriptor_sets();
    
    void prepare_pipelines();
    
    void prepare_uniform_buffers();
    
    void update_uniform_buffers();
    
    void update_params() const;
    
    void draw();
    
    bool prepare(vox::Platform &platform) override;
    
    void render(float delta_time) override;
    
    void on_update_ui_overlay(vox::Drawer &drawer) override;
    
    bool resize(uint32_t width, uint32_t height) override;
};

}
