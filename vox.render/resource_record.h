//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <vector>

#include "rendering/pipeline_state.h"

namespace vox {
class GraphicsPipeline;

class PipelineLayout;

class RenderPass;

class ShaderModule;

enum class ResourceType {
    ShaderModule,
    PipelineLayout,
    RenderPass,
    GraphicsPipeline
};

/**
 * @brief Writes Vulkan objects in a memory stream.
 */
class ResourceRecord {
public:
    void set_data(const std::vector<uint8_t> &data);
    
    std::vector<uint8_t> get_data();
    
    const std::ostringstream &get_stream();
    
    size_t register_shader_module(VkShaderStageFlagBits stage,
                                  const ShaderSource &glsl_source,
                                  const std::string &entry_point,
                                  const ShaderVariant &shader_variant);
    
    size_t register_pipeline_layout(const std::vector<ShaderModule *> &shader_modules);
    
    size_t register_render_pass(const std::vector<Attachment> &attachments,
                                const std::vector<LoadStoreInfo> &load_store_infos,
                                const std::vector<SubpassInfo> &subpasses);
    
    size_t register_graphics_pipeline(VkPipelineCache pipeline_cache,
                                      PipelineState &pipeline_state);
    
    void set_shader_module(size_t index, const ShaderModule &shader_module);
    
    void set_pipeline_layout(size_t index, const PipelineLayout &pipeline_layout);
    
    void set_render_pass(size_t index, const RenderPass &render_pass);
    
    void set_graphics_pipeline(size_t index, const GraphicsPipeline &graphics_pipeline);
    
private:
    std::ostringstream stream;
    
    std::vector<size_t> shader_module_indices;
    
    std::vector<size_t> pipeline_layout_indices;
    
    std::vector<size_t> render_pass_indices;
    
    std::vector<size_t> graphics_pipeline_indices;
    
    std::unordered_map<const ShaderModule *, size_t> shader_module_to_index;
    
    std::unordered_map<const PipelineLayout *, size_t> pipeline_layout_to_index;
    
    std::unordered_map<const RenderPass *, size_t> render_pass_to_index;
    
    std::unordered_map<const GraphicsPipeline *, size_t> graphics_pipeline_to_index;
};


}        // namespace vox
