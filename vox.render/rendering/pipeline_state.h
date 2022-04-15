//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <vector>

#include "vk_common.h"
#include "core/pipeline_layout.h"
#include "core/render_pass.h"

namespace vox {
struct VertexInputState {
    std::vector<VkVertexInputBindingDescription> bindings;
    
    std::vector<VkVertexInputAttributeDescription> attributes;
};

struct InputAssemblyState {
    VkPrimitiveTopology topology{VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST};
    
    VkBool32 primitive_restart_enable{VK_FALSE};
};

struct RasterizationState {
    VkBool32 depth_clamp_enable{VK_FALSE};
    
    VkBool32 rasterizer_discard_enable{VK_FALSE};
    
    VkPolygonMode polygon_mode{VK_POLYGON_MODE_FILL};
    
    VkCullModeFlags cull_mode{VK_CULL_MODE_BACK_BIT};
    
    VkFrontFace front_face{VK_FRONT_FACE_COUNTER_CLOCKWISE};
    
    VkBool32 depth_bias_enable{VK_FALSE};
};

struct ViewportState {
    uint32_t viewport_count{1};
    
    uint32_t scissor_count{1};
};

struct MultisampleState {
    VkSampleCountFlagBits rasterization_samples{VK_SAMPLE_COUNT_1_BIT};
    
    VkBool32 sample_shading_enable{VK_FALSE};
    
    float min_sample_shading{0.0f};
    
    VkSampleMask sample_mask{0};
    
    VkBool32 alpha_to_coverage_enable{VK_FALSE};
    
    VkBool32 alpha_to_one_enable{VK_FALSE};
};

struct StencilOpState {
    VkStencilOp fail_op{VK_STENCIL_OP_REPLACE};
    
    VkStencilOp pass_op{VK_STENCIL_OP_REPLACE};
    
    VkStencilOp depth_fail_op{VK_STENCIL_OP_REPLACE};
    
    VkCompareOp compare_op{VK_COMPARE_OP_NEVER};
};

struct DepthStencilState {
    VkBool32 depth_test_enable{VK_TRUE};
    
    VkBool32 depth_write_enable{VK_TRUE};
    
    // Note: Using Reversed depth-buffer for increased precision, so Greater depth values are kept
    VkCompareOp depth_compare_op{VK_COMPARE_OP_GREATER};
    
    VkBool32 depth_bounds_test_enable{VK_FALSE};
    
    VkBool32 stencil_test_enable{VK_FALSE};
    
    StencilOpState front{};
    
    StencilOpState back{};
};

struct ColorBlendAttachmentState {
    VkBool32 blend_enable{VK_FALSE};
    
    VkBlendFactor src_color_blend_factor{VK_BLEND_FACTOR_ONE};
    
    VkBlendFactor dst_color_blend_factor{VK_BLEND_FACTOR_ZERO};
    
    VkBlendOp color_blend_op{VK_BLEND_OP_ADD};
    
    VkBlendFactor src_alpha_blend_factor{VK_BLEND_FACTOR_ONE};
    
    VkBlendFactor dst_alpha_blend_factor{VK_BLEND_FACTOR_ZERO};
    
    VkBlendOp alpha_blend_op{VK_BLEND_OP_ADD};
    
    VkColorComponentFlags color_write_mask{
        VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT |
        VK_COLOR_COMPONENT_A_BIT};
};

struct ColorBlendState {
    VkBool32 logic_op_enable{VK_FALSE};
    
    VkLogicOp logic_op{VK_LOGIC_OP_CLEAR};
    
    std::vector<ColorBlendAttachmentState> attachments;
};

/// Helper class to create specialization constants for a Vulkan pipeline. The state tracks a pipeline globally, and not per shader. Two shaders using the same constant_id will have the same data.
class SpecializationConstantState {
public:
    void reset();
    
    [[nodiscard]] bool is_dirty() const;
    
    void clear_dirty();
    
    template<class T>
    void set_constant(uint32_t constant_id, const T &data);
    
    void set_constant(uint32_t constant_id, const std::vector<uint8_t> &data);
    
    void set_specialization_constant_state(const std::map<uint32_t, std::vector<uint8_t>> &state);
    
    [[nodiscard]] const std::map<uint32_t, std::vector<uint8_t>> &get_specialization_constant_state() const;
    
private:
    bool dirty_{false};
    // Map tracking state of the Specialization Constants
    std::map<uint32_t, std::vector<uint8_t>> specialization_constant_state_;
};

template<class T>
inline void SpecializationConstantState::set_constant(std::uint32_t constant_id, const T &data) {
    set_constant(constant_id, to_bytes(static_cast<std::uint32_t>(data)));
}

template<>
inline void SpecializationConstantState::set_constant<bool>(std::uint32_t constant_id, const bool &data) {
    set_constant(constant_id, to_bytes(static_cast<std::uint32_t>(data)));
}

class PipelineState {
public:
    void reset();
    
    void set_pipeline_layout(PipelineLayout &pipeline_layout);
    
    void set_render_pass(const RenderPass &render_pass);
    
    void set_specialization_constant(uint32_t constant_id, const std::vector<uint8_t> &data);
    
    void set_vertex_input_state(const VertexInputState &vertex_input_state);
    
    void set_input_assembly_state(const InputAssemblyState &input_assembly_state);
    
    void set_rasterization_state(const RasterizationState &rasterization_state);
    
    void set_viewport_state(const ViewportState &viewport_state);
    
    void set_multisample_state(const MultisampleState &multisample_state);
    
    void set_depth_stencil_state(const DepthStencilState &depth_stencil_state);
    
    void set_color_blend_state(const ColorBlendState &color_blend_state);
    
    void set_subpass_index(uint32_t subpass_index);
    
    [[nodiscard]] const PipelineLayout &get_pipeline_layout() const;
    
    [[nodiscard]] const RenderPass *get_render_pass() const;
    
    [[nodiscard]] const SpecializationConstantState &get_specialization_constant_state() const;
    
    [[nodiscard]] const VertexInputState &get_vertex_input_state() const;
    
    [[nodiscard]] const InputAssemblyState &get_input_assembly_state() const;
    
    [[nodiscard]] const RasterizationState &get_rasterization_state() const;
    
    [[nodiscard]] const ViewportState &get_viewport_state() const;
    
    [[nodiscard]] const MultisampleState &get_multisample_state() const;
    
    [[nodiscard]] const DepthStencilState &get_depth_stencil_state() const;
    
    [[nodiscard]] const ColorBlendState &get_color_blend_state() const;
    
    [[nodiscard]] uint32_t get_subpass_index() const;
    
    [[nodiscard]] bool is_dirty() const;
    
    void clear_dirty();
    
private:
    bool dirty_{false};
    
    PipelineLayout *pipeline_layout_{nullptr};
    
    const RenderPass *render_pass_{nullptr};
    
    SpecializationConstantState specialization_constant_state_{};
    
    VertexInputState vertex_input_state_{};
    
    InputAssemblyState input_assembly_state_{};
    
    RasterizationState rasterization_state_{};
    
    ViewportState viewport_state_{};
    
    MultisampleState multisample_state_{};
    
    DepthStencilState depth_stencil_state_{};
    
    ColorBlendState color_blend_state_{};
    
    uint32_t subpass_index_{0U};
};

}        // namespace vox
