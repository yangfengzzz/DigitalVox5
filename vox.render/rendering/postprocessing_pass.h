//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "core/command_buffer.h"
#include "render_context.h"
#include "render_target.h"
#include <functional>

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
    virtual void prepare(CommandBuffer &command_buffer, RenderTarget &default_render_target) {
        prepared_ = true;
    }
    
    /**
     * @brief Runs this pass, recording commands into the given command buffer.
     * @remarks Passes that that do not explicitly have a vox::RenderTarget set will render
     *          to default_render_target.
     */
    virtual void draw(CommandBuffer &command_buffer, RenderTarget &default_render_target) {}
    
    /**
     * @brief A functor ran in the context of this renderpass.
     * @see set_pre_draw_func(), set_post_draw_func()
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
    [[nodiscard]] RenderContext &get_render_context() const;
    
    /**
     * @brief Returns the parent's fullscreen triangle vertex shader source.
     */
    [[nodiscard]] ShaderSource &get_triangle_vs() const;
    
    struct BarrierInfo {
        VkPipelineStageFlags pipeline_stage;            // Pipeline stage of this pass' inputs/outputs
        VkAccessFlags image_read_access;         // Access mask for images read from this pass
        VkAccessFlags image_write_access;        // Access mask for images written to by this pass
    };
    
    /**
     * @brief Returns information that can be used to setup memory barriers of images
     *        that are produced (e.g. image stores, color attachment output) by this pass.
     */
    [[nodiscard]] virtual BarrierInfo get_src_barrier_info() const = 0;
    
    /**
     * @brief Returns information that can be used to setup memory barriers of images
     *        that are consumed (e.g. image loads, texture sampling) by this pass.
     */
    [[nodiscard]] virtual BarrierInfo get_dst_barrier_info() const = 0;
    
    /**
     * @brief Convenience function that calls get_src_barrier_info() on the previous pass of the pipeline,
     *        if any, or returns the specified default if this is the first pass in the pipeline.
     */
    [[nodiscard]] BarrierInfo get_predecessor_src_barrier_info(BarrierInfo fallback = {}) const;
};

/**
 * @brief CRTP base of all types of passes in a vox::PostProcessingPipeline.
 */
template<typename Self>
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
    inline Self &set_pre_draw_func(HookFunc &&new_func) {
        pre_draw_ = std::move(new_func);
        
        return static_cast<Self &>(*this);
    }
    
    /**
     * @brief Sets a functor that, if non-null, will be invoked after draw()ing this pass.
     * @remarks The function after drawing the last subpass, and before ending this RenderPass.
     */
    inline Self &set_post_draw_func(HookFunc &&new_func) {
        post_draw_ = std::move(new_func);
        
        return static_cast<Self &>(*this);
    }
    
    /**
     * @brief Render target to output to.
     *        If set, this pass will output to the given render target instead of the one passed to draw().
     */
    [[nodiscard]] inline RenderTarget *get_render_target() const {
        return render_target_;
    }
    
    /**
     * @copydoc get_render_target()
     */
    inline Self &set_render_target(RenderTarget *new_render_target) {
        render_target_ = new_render_target;
        
        return static_cast<Self &>(*this);
    }
    
    /**
     * @brief Returns the the debug name of this pass.
     */
    [[nodiscard]] inline const std::string &get_debug_name() const {
        return debug_name_;
    }
    
    /**
     * @brief Sets the debug name of this pass.
     */
    inline Self &set_debug_name(const std::string &new_debug_name) {
        debug_name_ = new_debug_name;
        
        return static_cast<Self &>(*this);
    }
    
    /**
     * @brief Returns the vox::PostProcessingPipeline that is the parent of this pass.
     */
    [[nodiscard]] inline PostProcessingPipeline &get_parent() const {
        return *parent_;
    }
};

}        // namespace vox
