//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.render/rendering/postprocessing_pass.h"

namespace vox {
class PostProcessingRenderPass;

/**
 * @brief A rendering pipeline specialized for fullscreen post-processing and compute passes.
 */
class PostProcessingPipeline {
public:
    friend class PostProcessingPassBase;

    /**
     * @brief Creates a rendering pipeline entirely made of fullscreen post-processing subpasses.
     */
    PostProcessingPipeline(RenderContext &render_context, ShaderSource triangle_vs);

    PostProcessingPipeline(const PostProcessingPipeline &to_copy) = delete;
    PostProcessingPipeline &operator=(const PostProcessingPipeline &to_copy) = delete;

    PostProcessingPipeline(PostProcessingPipeline &&to_move) = delete;
    PostProcessingPipeline &operator=(PostProcessingPipeline &&to_move) = delete;

    virtual ~PostProcessingPipeline() = default;

    /**
     * @brief Runs all renderpasses in this pipeline, recording commands into the given command buffer.
     * @remarks vox::PostProcessingRenderpass that do not explicitly have a vox::RenderTarget set will render
     *          to default_render_target.
     */
    void Draw(CommandBuffer &command_buffer, RenderTarget &default_render_target);

    /**
     * @brief Gets all of the passes in the pipeline.
     */
    inline std::vector<std::unique_ptr<PostProcessingPassBase>> &GetPasses() { return passes_; }

    /**
     * @brief Get the pass at a certain index as a `TPass`.
     */
    template <typename TPass = vox::PostProcessingRenderPass>
    inline TPass &GetPass(size_t index) {
        return *dynamic_cast<TPass *>(passes_[index].get());
    }

    /**
     * @brief Adds a pass of the given type to the end of the pipeline by constructing it in-place.
     */
    template <typename TPass = vox::PostProcessingRenderPass, typename... ConstructorArgs>
    TPass &AddPass(ConstructorArgs &&...args) {
        passes_.emplace_back(std::make_unique<TPass>(this, std::forward<ConstructorArgs>(args)...));
        auto &added_pass = *dynamic_cast<TPass *>(passes_.back().get());
        return added_pass;
    }

    /**
     * @brief Returns the current render context.
     */
    [[nodiscard]] inline RenderContext &GetRenderContext() const { return *render_context_; }

    /**
     * @brief Returns the index of the currently-being-drawn pass.
     */
    [[nodiscard]] inline size_t GetCurrentPassIndex() const { return current_pass_index_; }

private:
    RenderContext *render_context_{nullptr};
    ShaderSource triangle_vs_;
    std::vector<std::unique_ptr<PostProcessingPassBase>> passes_{};
    size_t current_pass_index_{0};
};

}  // namespace vox
