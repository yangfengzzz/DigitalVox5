//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <functional>

#include "vox.render/core/command_buffer.h"
#include "vox.render/rendering/render_context.h"
#include "vox.render/rendering/render_target.h"

namespace vox {
class PostProcessingPipeline;

/**
 * @brief The base of all types of passes in a vox::PostProcessingPipeline.
 */
class PostProcessingPassBase {
    friend class PostProcessingPipeline;

public:
    explicit PostProcessingPassBase(PostProcessingPipeline *parent);

    PostProcessingPassBase(const PostProcessingPassBase &to_copy) = delete;
    PostProcessingPassBase &operator=(const PostProcessingPassBase &to_copy) = delete;

    PostProcessingPassBase(PostProcessingPassBase &&to_move) = default;
    PostProcessingPassBase &operator=(PostProcessingPassBase &&to_move) = default;

    virtual ~PostProcessingPassBase() = default;

protected:
    /**
     * @brief Prepares this pass, recording commands into the given command buffer.
     * @remarks Passes that that do not explicitly have a vox::RenderTarget set will render
     *          to default_render_target.
     */
    virtual void Prepare(CommandBuffer &command_buffer, RenderTarget &default_render_target) { prepared_ = true; }

    /**
     * @brief Runs this pass, recording commands into the given command buffer.
     * @remarks Passes that that do not explicitly have a vox::RenderTarget set will render
     *          to default_render_target.
     */
    virtual void Draw(CommandBuffer &command_buffer, RenderTarget &default_render_target) {}

    /**
     * @brief A functor ran in the context of this renderpass.
     * @see SetPreDrawFunc(), SetPostDrawFunc()
     */
    using HookFunc = std::function<void()>;

    // NOTE: Protected members are exposed via getters and setters in PostProcessingPass<>
    PostProcessingPipeline *parent_{nullptr};
    bool prepared_{false};

    std::string debug_name_{};

    RenderTarget *render_target_{nullptr};
    std::shared_ptr<core::Sampler> default_sampler_{};

    HookFunc pre_draw_{};
    HookFunc post_draw_{};

    /**
     * @brief Returns the parent's render context.
     */
    [[nodiscard]] RenderContext &GetRenderContext() const;

    /**
     * @brief Returns the parent's fullscreen triangle vertex shader source.
     */
    [[nodiscard]] ShaderSource &GetTriangleVs() const;

    struct BarrierInfo {
        VkPipelineStageFlags pipeline_stage;  // Pipeline stage of this pass' inputs/outputs
        VkAccessFlags image_read_access;      // Access mask for images read from this pass
        VkAccessFlags image_write_access;     // Access mask for images written to by this pass
    };

    /**
     * @brief Returns information that can be used to setup memory barriers of images
     *        that are produced (e.g. image stores, color attachment output) by this pass.
     */
    [[nodiscard]] virtual BarrierInfo GetSrcBarrierInfo() const = 0;

    /**
     * @brief Returns information that can be used to setup memory barriers of images
     *        that are consumed (e.g. image loads, texture sampling) by this pass.
     */
    [[nodiscard]] virtual BarrierInfo GetDstBarrierInfo() const = 0;

    /**
     * @brief Convenience function that calls GetSrcBarrierInfo() on the previous pass of the pipeline,
     *        if any, or returns the specified default if this is the first pass in the pipeline.
     */
    [[nodiscard]] BarrierInfo GetPredecessorSrcBarrierInfo(BarrierInfo fallback = {}) const;
};

// MARK: - PostProcessingPass
/**
 * @brief CRTP base of all types of passes in a vox::PostProcessingPipeline.
 */
template <typename Self>
class PostProcessingPass : public PostProcessingPassBase {
public:
    using PostProcessingPassBase::PostProcessingPassBase;

    PostProcessingPass(const PostProcessingPass &to_copy) = delete;
    PostProcessingPass &operator=(const PostProcessingPass &to_copy) = delete;

    PostProcessingPass(PostProcessingPass &&to_move) noexcept = default;
    PostProcessingPass &operator=(PostProcessingPass &&to_move) = default;

    ~PostProcessingPass() override = default;

    /**
     * @brief Sets a functor that, if non-null, will be invoked before draw()ing this pass.
     * @remarks The function is invoked after ending the previous RenderPass, and before beginning this one.
     */
    inline Self &SetPreDrawFunc(HookFunc &&new_func) {
        pre_draw_ = std::move(new_func);

        return static_cast<Self &>(*this);
    }

    /**
     * @brief Sets a functor that, if non-null, will be invoked after draw()ing this pass.
     * @remarks The function after drawing the last subpass, and before ending this RenderPass.
     */
    inline Self &SetPostDrawFunc(HookFunc &&new_func) {
        post_draw_ = std::move(new_func);

        return static_cast<Self &>(*this);
    }

    /**
     * @brief Render target to output to.
     *        If set, this pass will output to the given render target instead of the one passed to draw().
     */
    [[nodiscard]] inline RenderTarget *GetRenderTarget() const { return render_target_; }

    /**
     * @copydoc GetRenderTarget()
     */
    inline Self &SetRenderTarget(RenderTarget *new_render_target) {
        render_target_ = new_render_target;

        return static_cast<Self &>(*this);
    }

    /**
     * @brief Returns the the debug name of this pass.
     */
    [[nodiscard]] inline const std::string &GetDebugName() const { return debug_name_; }

    /**
     * @brief Sets the debug name of this pass.
     */
    inline Self &SetDebugName(const std::string &new_debug_name) {
        debug_name_ = new_debug_name;

        return static_cast<Self &>(*this);
    }

    /**
     * @brief Returns the vox::PostProcessingPipeline that is the parent of this pass.
     */
    [[nodiscard]] inline PostProcessingPipeline &GetParent() const { return *parent_; }
};

}  // namespace vox
