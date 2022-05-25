//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.base/helper.h"
#include "vox.render/core/buffer.h"
#include "vox.render/rendering/render_frame.h"
#include "vox.render/rendering/subpass.h"
#include "vox.render/utils.h"

namespace vox {
/**
 * @brief A RenderPipeline is a sequence of Subpass objects.
 * Subpass holds shaders and can draw the core::sg::Scene.
 * More subpasses can be added to the sequence if required.
 * For example, postprocessing can be implemented with two pipelines which
 * share render targets.
 *
 * GeometrySubpass -> Processes Scene for Shaders, use by itself if shader requires no lighting
 * ForwardSubpass -> Binds lights at the beginning of a GeometrySubpass to create Forward Rendering, should be used with
 * most default shaders LightingSubpass -> Holds a Global Light uniform, Can be combined with GeometrySubpass to create
 * Deferred Rendering
 */
class RenderPipeline {
public:
    explicit RenderPipeline(std::vector<std::unique_ptr<Subpass>> &&subpasses = {});

    RenderPipeline(const RenderPipeline &) = delete;

    RenderPipeline(RenderPipeline &&) = default;

    virtual ~RenderPipeline() = default;

    RenderPipeline &operator=(const RenderPipeline &) = delete;

    RenderPipeline &operator=(RenderPipeline &&) = default;

    /**
     * @brief Prepares the subpasses
     */
    void Prepare();

    /**
     * @return Load store info
     */
    [[nodiscard]] const std::vector<LoadStoreInfo> &GetLoadStore() const;

    /**
     * @param load_store Load store info to set
     */
    void SetLoadStore(const std::vector<LoadStoreInfo> &load_store);

    /**
     * @return Clear values
     */
    [[nodiscard]] const std::vector<VkClearValue> &GetClearValue() const;

    /**
     * @param clear_values Clear values to set
     */
    void SetClearValue(const std::vector<VkClearValue> &clear_values);

    /**
     * @brief Appends a subpass to the pipeline
     * @param subpass Subpass to append
     */
    void AddSubpass(std::unique_ptr<Subpass> &&subpass);

    std::vector<std::unique_ptr<Subpass>> &GetSubpasses();

    /**
     * @brief Record draw commands for each Subpass
     */
    void Draw(CommandBuffer &command_buffer,
              RenderTarget &render_target,
              VkSubpassContents contents = VK_SUBPASS_CONTENTS_INLINE);

    /**
     * @return Subpass currently being recorded, or the first one
     *         if drawing has not started
     */
    std::unique_ptr<Subpass> &GetActiveSubpass();

private:
    std::vector<std::unique_ptr<Subpass>> subpasses_;

    /// Default to two load store
    std::vector<LoadStoreInfo> load_store_ = std::vector<LoadStoreInfo>(2);

    /// Default to two clear values
    std::vector<VkClearValue> clear_value_ = std::vector<VkClearValue>(2);

    size_t active_subpass_index_{0};
};

}  // namespace vox
