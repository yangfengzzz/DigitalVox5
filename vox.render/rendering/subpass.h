/* Copyright (c) 2019-2021, Arm Limited and Contributors
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

#pragma once

#include "buffer_pool.h"
#include "helpers.h"
#include "core/shader_module.h"
#include "rendering/pipeline_state.h"
#include "rendering/render_context.h"
#include "rendering/render_frame.h"

namespace vox {
class CommandBuffer;

/**
 * @brief This class defines an interface for subpasses
 *        where they need to implement the draw function.
 *        It is used to construct a RenderPipeline
 */
class Subpass {
public:
    Subpass(RenderContext &render_context, ShaderSource &&vertex_shader, ShaderSource &&fragment_shader);
    
    Subpass(const Subpass &) = delete;
    
    Subpass(Subpass &&) = default;
    
    virtual ~Subpass() = default;
    
    Subpass &operator=(const Subpass &) = delete;
    
    Subpass &operator=(Subpass &&) = delete;
    
    /**
     * @brief Prepares the shaders and shader variants for a subpass
     */
    virtual void prepare() = 0;
    
    /**
     * @brief Updates the render target attachments with the ones stored in this subpass
     *        This function is called by the RenderPipeline before beginning the render
     *        pass and before proceeding with a new subpass.
     */
    void update_render_target_attachments(RenderTarget &render_target);
    
    /**
     * @brief Draw virtual function
     * @param command_buffer Command buffer to use to record draw commands
     */
    virtual void draw(CommandBuffer &command_buffer) = 0;
    
    RenderContext &get_render_context();
    
    [[nodiscard]] const ShaderSource &get_vertex_shader() const;
    
    [[nodiscard]] const ShaderSource &get_fragment_shader() const;
    
    DepthStencilState &get_depth_stencil_state();
    
    [[nodiscard]] const std::vector<uint32_t> &get_input_attachments() const;
    
    void set_input_attachments(std::vector<uint32_t> input);
    
    [[nodiscard]] const std::vector<uint32_t> &get_output_attachments() const;
    
    void set_output_attachments(std::vector<uint32_t> output);
    
    void set_sample_count(VkSampleCountFlagBits sample_count);
    
    [[nodiscard]] const std::vector<uint32_t> &get_color_resolve_attachments() const;
    
    void set_color_resolve_attachments(std::vector<uint32_t> color_resolve);
    
    [[nodiscard]] const bool &get_disable_depth_stencil_attachment() const;
    
    void set_disable_depth_stencil_attachment(bool disable_depth_stencil);
    
    [[nodiscard]] const uint32_t &get_depth_stencil_resolve_attachment() const;
    
    void set_depth_stencil_resolve_attachment(uint32_t depth_stencil_resolve);
    
    [[nodiscard]] VkResolveModeFlagBits get_depth_stencil_resolve_mode() const;
    
    void set_depth_stencil_resolve_mode(VkResolveModeFlagBits mode);
    
    [[nodiscard]] const std::string &get_debug_name() const;
    
    void set_debug_name(const std::string &name);
    
protected:
    RenderContext &render_context;
    
    VkSampleCountFlagBits sample_count{VK_SAMPLE_COUNT_1_BIT};
    
    // A map of shader resource names and the mode of constant data
    std::unordered_map<std::string, ShaderResourceMode> resource_mode_map;
    
private:
    std::string debug_name{};
    
    ShaderSource vertex_shader;
    
    ShaderSource fragment_shader;
    
    DepthStencilState depth_stencil_state{};
    
    /**
     * @brief When creating the renderpass, pDepthStencilAttachment will
     *        be set to nullptr, which disables depth testing
     */
    bool disable_depth_stencil_attachment{false};
    
    /**
     * @brief When creating the renderpass, if not None, the resolve
     *        of the multisampled depth attachment will be enabled,
     *        with this mode, to depth_stencil_resolve_attachment
     */
    VkResolveModeFlagBits depth_stencil_resolve_mode{VK_RESOLVE_MODE_NONE};
    
    /// Default to no input attachments
    std::vector<uint32_t> input_attachments = {};
    
    /// Default to swapchain output attachment
    std::vector<uint32_t> output_attachments = {0};
    
    /// Default to no color resolve attachments
    std::vector<uint32_t> color_resolve_attachments = {};
    
    /// Default to no depth stencil resolve attachment
    uint32_t depth_stencil_resolve_attachment{VK_ATTACHMENT_UNUSED};
};

}        // namespace vox
