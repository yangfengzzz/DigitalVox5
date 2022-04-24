//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "shader/shader_module.h"
#include "shader/shader_data.h"
#include "postprocessing_pass.h"

namespace vox {
/**
 * @brief A compute pass in a vox::PostProcessingPipeline.
 */
class PostProcessingComputePass : public PostProcessingPass<PostProcessingComputePass> {
public:
    PostProcessingComputePass(PostProcessingPipeline *parent,
                              std::shared_ptr<ShaderSource> cs_source);
    
    PostProcessingComputePass(const PostProcessingComputePass &to_copy) = delete;
    PostProcessingComputePass &operator=(const PostProcessingComputePass &to_copy) = delete;
    
    PostProcessingComputePass(PostProcessingComputePass &&to_move) = default;
    PostProcessingComputePass &operator=(PostProcessingComputePass &&to_move) = default;
    
    void prepare(CommandBuffer &command_buffer, RenderTarget &default_render_target) override;
    
    void draw(CommandBuffer &command_buffer, RenderTarget &default_render_target) override;
    
public:
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
    
    void attach_shader_data(ShaderData *data);
    
    void detach_shader_data(ShaderData *data);
    
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
    std::shared_ptr<ShaderSource> cs_source_;
    std::array<uint32_t, 3> n_workgroups_{1, 1, 1};
    
    std::vector<ShaderData *> data_{};
    std::vector<uint8_t> push_constants_data_{};
    
    /**
     * @brief Transitions sampled_images (to SHADER_READ_ONLY_OPTIMAL)
     *        and storage_images (to GENERAL) as appropriate.
     */
    void transition_images(CommandBuffer &command_buffer, RenderTarget &default_render_target,
                           const ShaderVariant &cs_variant);
    
    [[nodiscard]] BarrierInfo get_src_barrier_info() const override;
    [[nodiscard]] BarrierInfo get_dst_barrier_info() const override;
};

}        // namespace vox
