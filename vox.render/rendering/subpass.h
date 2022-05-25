//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.base/helper.h"
#include "vox.render/buffer_pool.h"
#include "vox.render/rendering/pipeline_state.h"
#include "vox.render/rendering/render_context.h"
#include "vox.render/rendering/render_element.h"
#include "vox.render/rendering/render_frame.h"
#include "vox.render/scene.h"

namespace vox {
class CommandBuffer;

/**
 * @brief This class defines an interface for subpasses
 *        where they need to implement the draw function.
 *        It is used to construct a RenderPipeline
 */
class Subpass {
public:
    Subpass(RenderContext &render_context, Scene *scene, Camera *camera);

    Subpass(const Subpass &) = delete;

    Subpass(Subpass &&) = default;

    virtual ~Subpass() = default;

    Subpass &operator=(const Subpass &) = delete;

    Subpass &operator=(Subpass &&) = delete;

    /**
     * @brief Prepares the shaders and shader variants for a subpass
     */
    virtual void Prepare() = 0;

    /**
     * @brief Updates the render target attachments with the ones stored in this subpass
     *        This function is called by the RenderPipeline before beginning the render
     *        pass and before proceeding with a new subpass.
     */
    void UpdateRenderTargetAttachments(RenderTarget &render_target);

    /**
     * @brief Draw virtual function
     * @param command_buffer Command buffer to use to record draw commands
     */
    virtual void Draw(CommandBuffer &command_buffer) = 0;

    RenderContext &GetRenderContext();

    DepthStencilState &GetDepthStencilState();

    [[nodiscard]] const std::vector<uint32_t> &GetInputAttachments() const;

    void SetInputAttachments(std::vector<uint32_t> input);

    [[nodiscard]] const std::vector<uint32_t> &GetOutputAttachments() const;

    void SetOutputAttachments(std::vector<uint32_t> output);

    void SetSampleCount(VkSampleCountFlagBits sample_count);

    [[nodiscard]] const std::vector<uint32_t> &GetColorResolveAttachments() const;

    void SetColorResolveAttachments(std::vector<uint32_t> color_resolve);

    [[nodiscard]] const bool &GetDisableDepthStencilAttachment() const;

    void SetDisableDepthStencilAttachment(bool disable_depth_stencil);

    [[nodiscard]] const uint32_t &GetDepthStencilResolveAttachment() const;

    void SetDepthStencilResolveAttachment(uint32_t depth_stencil_resolve);

    [[nodiscard]] VkResolveModeFlagBits GetDepthStencilResolveMode() const;

    void SetDepthStencilResolveMode(VkResolveModeFlagBits mode);

    [[nodiscard]] const std::string &GetDebugName() const;

    void SetDebugName(const std::string &name);

protected:
    RenderContext &render_context_;
    Scene *scene_{nullptr};
    Camera *camera_{nullptr};

    VkSampleCountFlagBits sample_count_{VK_SAMPLE_COUNT_1_BIT};

    // A map of shader resource names and the mode of constant data
    std::unordered_map<std::string, ShaderResourceMode> resource_mode_map_;

    static bool CompareFromNearToFar(const RenderElement &a, const RenderElement &b);

    static bool CompareFromFarToNear(const RenderElement &a, const RenderElement &b);

    virtual PipelineLayout &PreparePipelineLayout(CommandBuffer &command_buffer,
                                                  const std::vector<ShaderModule *> &shader_modules);

private:
    std::string debug_name_{};

    /**
     * @brief When creating the renderpass, pDepthStencilAttachment will
     *        be set to nullptr, which disables depth testing
     */
    bool disable_depth_stencil_attachment_{false};

    /**
     * @brief When creating the renderpass, if not None, the resolve
     *        of the multisampled depth attachment will be enabled,
     *        with this mode, to depth_stencil_resolve_attachment
     */
    VkResolveModeFlagBits depth_stencil_resolve_mode_{VK_RESOLVE_MODE_NONE};

    /// Default to no input attachments
    std::vector<uint32_t> input_attachments_ = {};

    /// Default to swapchain output attachment
    std::vector<uint32_t> output_attachments_ = {0};

    /// Default to no color resolve attachments
    std::vector<uint32_t> color_resolve_attachments_ = {};

    /// Default to no depth stencil resolve attachment
    uint32_t depth_stencil_resolve_attachment_{VK_ATTACHMENT_UNUSED};
};

}  // namespace vox
