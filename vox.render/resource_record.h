//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <vector>

#include "vox.render/rendering/pipeline_state.h"

namespace vox {
class GraphicsPipeline;

class PipelineLayout;

class RenderPass;

class ShaderModule;

enum class ResourceType { SHADER_MODULE, PIPELINE_LAYOUT, RENDER_PASS, GRAPHICS_PIPELINE };

/**
 * @brief Writes Vulkan objects in a memory stream.
 */
class ResourceRecord {
public:
    void SetData(const std::vector<uint8_t> &data);

    std::vector<uint8_t> GetData();

    const std::ostringstream &GetStream();

    size_t RegisterShaderModule(VkShaderStageFlagBits stage,
                                const ShaderSource &glsl_source,
                                const std::string &entry_point,
                                const ShaderVariant &shader_variant);

    size_t RegisterPipelineLayout(const std::vector<ShaderModule *> &shader_modules);

    size_t RegisterRenderPass(const std::vector<Attachment> &attachments,
                              const std::vector<LoadStoreInfo> &load_store_infos,
                              const std::vector<SubpassInfo> &subpasses);

    size_t RegisterGraphicsPipeline(VkPipelineCache pipeline_cache, PipelineState &pipeline_state);

    void SetShaderModule(size_t index, const ShaderModule &shader_module);

    void SetPipelineLayout(size_t index, const PipelineLayout &pipeline_layout);

    void SetRenderPass(size_t index, const RenderPass &render_pass);

    void SetGraphicsPipeline(size_t index, const GraphicsPipeline &graphics_pipeline);

private:
    std::ostringstream stream_;

    std::vector<size_t> shader_module_indices_;

    std::vector<size_t> pipeline_layout_indices_;

    std::vector<size_t> render_pass_indices_;

    std::vector<size_t> graphics_pipeline_indices_;

    std::unordered_map<const ShaderModule *, size_t> shader_module_to_index_;

    std::unordered_map<const PipelineLayout *, size_t> pipeline_layout_to_index_;

    std::unordered_map<const RenderPass *, size_t> render_pass_to_index_;

    std::unordered_map<const GraphicsPipeline *, size_t> graphics_pipeline_to_index_;
};

}  // namespace vox
