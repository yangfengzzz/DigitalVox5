//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/resource_record.h"

#include "vox.render/core/pipeline.h"
#include "vox.render/core/pipeline_layout.h"
#include "vox.render/core/render_pass.h"
#include "vox.render/resource_cache.h"
#include "vox.render/shader/shader_module.h"

namespace vox {
namespace {
inline void WriteSubpassInfo(std::ostringstream &os, const std::vector<SubpassInfo> &value) {
    utility::Write(os, value.size());
    for (const SubpassInfo &item : value) {
        utility::Write(os, item.input_attachments);
        utility::Write(os, item.output_attachments);
    }
}

inline void WriteProcesses(std::ostringstream &os, const std::vector<std::string> &value) {
    utility::Write(os, value.size());
    for (const std::string &item : value) {
        utility::Write(os, item);
    }
}
}  // namespace

void ResourceRecord::SetData(const std::vector<uint8_t> &data) { stream_.str(std::string{data.begin(), data.end()}); }

std::vector<uint8_t> ResourceRecord::GetData() {
    std::string str = stream_.str();

    return std::vector<uint8_t>{str.begin(), str.end()};
}

const std::ostringstream &ResourceRecord::GetStream() { return stream_; }

size_t ResourceRecord::RegisterShaderModule(VkShaderStageFlagBits stage,
                                            const ShaderSource &glsl_source,
                                            const std::string &entry_point,
                                            const ShaderVariant &shader_variant) {
    shader_module_indices_.push_back(shader_module_indices_.size());

    utility::Write(stream_, ResourceType::SHADER_MODULE, stage, glsl_source.GetSource(), entry_point,
                   shader_variant.GetPreamble());

    WriteProcesses(stream_, shader_variant.GetProcesses());

    return shader_module_indices_.back();
}

size_t ResourceRecord::RegisterPipelineLayout(const std::vector<ShaderModule *> &shader_modules) {
    pipeline_layout_indices_.push_back(pipeline_layout_indices_.size());

    std::vector<size_t> shader_indices(shader_modules.size());
    std::transform(shader_modules.begin(), shader_modules.end(), shader_indices.begin(),
                   [this](ShaderModule *shader_module) { return shader_module_to_index_.at(shader_module); });

    utility::Write(stream_, ResourceType::PIPELINE_LAYOUT, shader_indices);

    return pipeline_layout_indices_.back();
}

size_t ResourceRecord::RegisterRenderPass(const std::vector<Attachment> &attachments,
                                          const std::vector<LoadStoreInfo> &load_store_infos,
                                          const std::vector<SubpassInfo> &subpasses) {
    render_pass_indices_.push_back(render_pass_indices_.size());

    utility::Write(stream_, ResourceType::RENDER_PASS, attachments, load_store_infos);

    WriteSubpassInfo(stream_, subpasses);

    return render_pass_indices_.back();
}

size_t ResourceRecord::RegisterGraphicsPipeline(VkPipelineCache /*pipeline_cache*/ pipeline_cache,
                                                PipelineState &pipeline_state) {
    graphics_pipeline_indices_.push_back(graphics_pipeline_indices_.size());

    auto &pipeline_layout = pipeline_state.GetPipelineLayout();
    auto render_pass = pipeline_state.GetRenderPass();

    utility::Write(stream_, ResourceType::GRAPHICS_PIPELINE, pipeline_layout_to_index_.at(&pipeline_layout),
                   render_pass_to_index_.at(render_pass), pipeline_state.GetSubpassIndex());

    auto &specialization_constant_state =
            pipeline_state.GetSpecializationConstantState().GetSpecializationConstantState();

    utility::Write(stream_, specialization_constant_state);

    auto &vertex_input_state = pipeline_state.GetVertexInputState();

    utility::Write(stream_, vertex_input_state.attributes, vertex_input_state.bindings);

    utility::Write(stream_, pipeline_state.GetInputAssemblyState(), pipeline_state.GetRasterizationState(),
                   pipeline_state.GetViewportState(), pipeline_state.GetMultisampleState(),
                   pipeline_state.GetDepthStencilState());

    auto &color_blend_state = pipeline_state.GetColorBlendState();

    utility::Write(stream_, color_blend_state.logic_op, color_blend_state.logic_op_enable,
                   color_blend_state.attachments);

    return graphics_pipeline_indices_.back();
}

void ResourceRecord::SetShaderModule(size_t index, const ShaderModule &shader_module) {
    shader_module_to_index_[&shader_module] = index;
}

void ResourceRecord::SetPipelineLayout(size_t index, const PipelineLayout &pipeline_layout) {
    pipeline_layout_to_index_[&pipeline_layout] = index;
}

void ResourceRecord::SetRenderPass(size_t index, const RenderPass &render_pass) {
    render_pass_to_index_[&render_pass] = index;
}

void ResourceRecord::SetGraphicsPipeline(size_t index, const GraphicsPipeline &graphics_pipeline) {
    graphics_pipeline_to_index_[&graphics_pipeline] = index;
}

}  // namespace vox
