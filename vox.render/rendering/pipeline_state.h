//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <vector>

#include "vox.render/core/pipeline_layout.h"
#include "vox.render/core/render_pass.h"
#include "vox.render/vk_common.h"

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

    VkCullModeFlags cull_mode{VK_CULL_MODE_FRONT_BIT};

    VkFrontFace front_face{VK_FRONT_FACE_CLOCKWISE};

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
    VkCompareOp depth_compare_op{VK_COMPARE_OP_LESS};

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

    VkColorComponentFlags color_write_mask{VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                                           VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT};
};

struct ColorBlendState {
    VkBool32 logic_op_enable{VK_FALSE};

    VkLogicOp logic_op{VK_LOGIC_OP_CLEAR};

    std::vector<ColorBlendAttachmentState> attachments;
};

/// Helper class to create specialization constants for a Vulkan pipeline. The state tracks a pipeline globally, and not
/// per shader. Two shaders using the same constant_id will have the same data.
class SpecializationConstantState {
public:
    void Reset();

    [[nodiscard]] bool IsDirty() const;

    void ClearDirty();

    template <class T>
    void SetConstant(uint32_t constant_id, const T &data);

    void SetConstant(uint32_t constant_id, const std::vector<uint8_t> &data);

    void SetSpecializationConstantState(const std::map<uint32_t, std::vector<uint8_t>> &state);

    [[nodiscard]] const std::map<uint32_t, std::vector<uint8_t>> &GetSpecializationConstantState() const;

private:
    bool dirty_{false};
    // Map tracking state of the Specialization Constants
    std::map<uint32_t, std::vector<uint8_t>> specialization_constant_state_;
};

template <class T>
inline void SpecializationConstantState::SetConstant(std::uint32_t constant_id, const T &data) {
    SetConstant(constant_id, utility::ToBytes(static_cast<std::uint32_t>(data)));
}

template <>
inline void SpecializationConstantState::SetConstant<bool>(std::uint32_t constant_id, const bool &data) {
    SetConstant(constant_id, utility::ToBytes(static_cast<std::uint32_t>(data)));
}

class PipelineState {
public:
    void Reset();

    void SetPipelineLayout(PipelineLayout &pipeline_layout);

    void SetRenderPass(const RenderPass &render_pass);

    void SetSpecializationConstant(uint32_t constant_id, const std::vector<uint8_t> &data);

    void SetVertexInputState(const VertexInputState &vertex_input_state);

    void SetInputAssemblyState(const InputAssemblyState &input_assembly_state);

    void SetRasterizationState(const RasterizationState &rasterization_state);

    void SetViewportState(const ViewportState &viewport_state);

    void SetMultisampleState(const MultisampleState &multisample_state);

    void SetDepthStencilState(const DepthStencilState &depth_stencil_state);

    void SetColorBlendState(const ColorBlendState &color_blend_state);

    void SetSubpassIndex(uint32_t subpass_index);

    [[nodiscard]] const PipelineLayout &GetPipelineLayout() const;

    [[nodiscard]] const RenderPass *GetRenderPass() const;

    [[nodiscard]] const SpecializationConstantState &GetSpecializationConstantState() const;

    [[nodiscard]] const VertexInputState &GetVertexInputState() const;

    [[nodiscard]] const InputAssemblyState &GetInputAssemblyState() const;

    [[nodiscard]] const RasterizationState &GetRasterizationState() const;

    [[nodiscard]] const ViewportState &GetViewportState() const;

    [[nodiscard]] const MultisampleState &GetMultisampleState() const;

    [[nodiscard]] const DepthStencilState &GetDepthStencilState() const;

    [[nodiscard]] const ColorBlendState &GetColorBlendState() const;

    [[nodiscard]] uint32_t GetSubpassIndex() const;

    [[nodiscard]] bool IsDirty() const;

    void ClearDirty();

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

}  // namespace vox
