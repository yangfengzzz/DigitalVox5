//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "core/sampled_image.h"
#include "shader/shader_module.h"
#include "postprocessing_pass.h"

namespace vox {
/**
 * @brief Maps in-shader binding names to the core::SampledImage to bind.
 */
using SampledImageMap = std::unordered_map<std::string, core::SampledImage>;

/**
 * @brief A compute pass in a vox::PostProcessingPipeline.
 */
class PostProcessingComputePass : public PostProcessingPass<PostProcessingComputePass> {
public:
    PostProcessingComputePass(PostProcessingPipeline *parent,
                              ShaderSource cs_source,
                              ShaderVariant cs_variant = {},
                              std::shared_ptr<core::Sampler> &&default_sampler = {});
    
    PostProcessingComputePass(const PostProcessingComputePass &to_copy) = delete;
    PostProcessingComputePass &operator=(const PostProcessingComputePass &to_copy) = delete;
    
    PostProcessingComputePass(PostProcessingComputePass &&to_move) = default;
    PostProcessingComputePass &operator=(PostProcessingComputePass &&to_move) = default;
    
    void prepare(CommandBuffer &command_buffer, RenderTarget &default_render_target) override;
    void draw(CommandBuffer &command_buffer, RenderTarget &default_render_target) override;
    
    /**
     * @brief Sets the number of workgroups to be dispatched each draw().
     */
    inline PostProcessingComputePass &set_dispatch_size(std::array<uint32_t, 3> new_size) {
        n_workgroups_ = new_size;
        return *this;
    }
    
    /**
     * @brief Gets the number of workgroups that will be dispatched each draw().
     */
    [[nodiscard]] inline std::array<uint32_t, 3> get_dispatch_size() const {
        return n_workgroups_;
    }
    
    /**
     * @brief Maps the names of samplers in the shader to vox::core::SampledImage.
     *        These are given as samplers to the subpass, at set 0; they are bound automatically according to their name.
     * @remarks PostProcessingPipeline::get_sampler() is used as the default sampler if none is specified.
     *          The RenderTarget for the current PostprocessingStep is used if none is specified for attachment images.
     */
    [[nodiscard]] inline const SampledImageMap &get_sampled_images() const {
        return sampled_images_;
    }
    
    /**
     * @brief Maps the names of storage images in the shader to vox::core::SampledImage.
     *        These are given as image2D / image2DArray / ... to the subpass, at set 0;
     *        they are bound automatically according to their name.
     */
    [[nodiscard]] inline const SampledImageMap &get_storage_images() const {
        return storage_images_;
    }
    
    /**
     * @brief Changes (or adds) the sampled image at name for this step.
     * @remarks If no RenderTarget is specifically set for the core::SampledImage,
     *          it will default to sample in the RenderTarget currently bound for drawing in the parent PostProcessingRenderpass.
     * @remarks Images from RenderTarget attachments are automatically transitioned to SHADER_READ_ONLY_OPTIMAL layout if needed.
     */
    PostProcessingComputePass &bind_sampled_image(const std::string &name, core::SampledImage &&new_image);
    
    /**
     * @brief Changes (or adds) the storage image at name for this step.
     * @remarks Images from RenderTarget attachments are automatically transitioned to GENERAL layout if needed.
     */
    PostProcessingComputePass &bind_storage_image(const std::string &name, core::SampledImage &&new_image);
    
    /**
     * @brief Set the uniform data to be bound at set 0, binding 0.
     */
    template<typename T>
    inline PostProcessingComputePass &set_uniform_data(const T &data) {
        uniform_data_.reserve(sizeof(data));
        auto data_ptr = reinterpret_cast<const uint8_t *>(&data);
        uniform_data_.assign(data_ptr, data_ptr + sizeof(data));
        
        return *this;
    }
    
    /**
     * @copydoc set_uniform_data(const T&)
     */
    inline PostProcessingComputePass &set_uniform_data(const std::vector<uint8_t> &data) {
        uniform_data_ = data;
        
        return *this;
    }
    
    /**
     * @brief Set the constants that are pushed before each draw.
     */
    template<typename T>
    inline PostProcessingComputePass &set_push_constants(const T &data) {
        push_constants_data_.reserve(sizeof(data));
        auto data_ptr = reinterpret_cast<const uint8_t *>(&data);
        push_constants_data_.assign(data_ptr, data_ptr + sizeof(data));
        
        return *this;
    }
    
    /**
     * @copydoc set_push_constants(const T&)
     */
    inline PostProcessingComputePass &set_push_constants(const std::vector<uint8_t> &data) {
        push_constants_data_ = data;
        
        return *this;
    }
    
private:
    ShaderSource cs_source_;
    ShaderVariant cs_variant_;
    std::array<uint32_t, 3> n_workgroups_{1, 1, 1};
    
    std::shared_ptr<core::Sampler> default_sampler_{};
    SampledImageMap sampled_images_{};
    SampledImageMap storage_images_{};
    
    std::vector<uint8_t> uniform_data_{};
    std::unique_ptr<BufferAllocation> uniform_alloc_{};
    std::vector<uint8_t> push_constants_data_{};
    
    /**
     * @brief Transitions sampled_images (to SHADER_READ_ONLY_OPTIMAL)
     *        and storage_images (to GENERAL) as appropriate.
     */
    void transition_images(CommandBuffer &command_buffer, RenderTarget &default_render_target);
    
    [[nodiscard]] BarrierInfo get_src_barrier_info() const override;
    [[nodiscard]] BarrierInfo get_dst_barrier_info() const override;
};

}        // namespace vox
