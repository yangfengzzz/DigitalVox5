//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.render/rendering/postprocessing_pass.h"
#include "vox.render/shader/shader_data.h"
#include "vox.render/shader/shader_module.h"

namespace vox {
/**
 * @brief A compute pass in a vox::PostProcessingPipeline.
 */
class PostProcessingComputePass : public PostProcessingPass<PostProcessingComputePass> {
public:
    PostProcessingComputePass(PostProcessingPipeline *parent, std::shared_ptr<ShaderSource> cs_source);

    PostProcessingComputePass(const PostProcessingComputePass &to_copy) = delete;
    PostProcessingComputePass &operator=(const PostProcessingComputePass &to_copy) = delete;

    PostProcessingComputePass(PostProcessingComputePass &&to_move) = default;
    PostProcessingComputePass &operator=(PostProcessingComputePass &&to_move) = default;

    void Prepare(CommandBuffer &command_buffer, RenderTarget &default_render_target) override;

    void Draw(CommandBuffer &command_buffer, RenderTarget &default_render_target) override;

public:
    /**
     * @brief Sets the number of workgroups to be dispatched each draw().
     */
    inline PostProcessingComputePass &SetDispatchSize(std::array<uint32_t, 3> new_size) {
        n_workgroups_ = new_size;
        return *this;
    }

    /**
     * @brief Gets the number of workgroups that will be dispatched each draw().
     */
    [[nodiscard]] inline std::array<uint32_t, 3> GetDispatchSize() const { return n_workgroups_; }

    void AttachShaderData(ShaderData *data);

    void DetachShaderData(ShaderData *data);

    /**
     * @brief Set the constants that are pushed before each draw.
     */
    template <typename T>
    inline PostProcessingComputePass &SetPushConstants(const T &data) {
        push_constants_data_.reserve(sizeof(data));
        auto data_ptr = reinterpret_cast<const uint8_t *>(&data);
        push_constants_data_.assign(data_ptr, data_ptr + sizeof(data));

        return *this;
    }

    /**
     * @copydoc SetPushConstants(const T&)
     */
    inline PostProcessingComputePass &SetPushConstants(const std::vector<uint8_t> &data) {
        push_constants_data_ = data;

        return *this;
    }

private:
    std::shared_ptr<ShaderSource> cs_source_;
    std::array<uint32_t, 3> n_workgroups_{1, 1, 1};

    std::vector<ShaderData *> data_{};
    std::vector<uint8_t> push_constants_data_{};

    /**
     * @brief Transitions sampled_images (to SHADER_READ_ONLY_OPTIMAL)
     *        and storage_images (to GENERAL) as appropriate.
     */
    void TransitionImages(CommandBuffer &command_buffer,
                          RenderTarget &default_render_target,
                          const ShaderVariant &cs_variant);

    [[nodiscard]] BarrierInfo GetSrcBarrierInfo() const override;
    [[nodiscard]] BarrierInfo GetDstBarrierInfo() const override;
};

}  // namespace vox
