//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/resource_replay.h"

#include "vox.base/logging.h"
#include "vox.render/rendering/pipeline_state.h"
#include "vox.render/resource_cache.h"
#include "vox.render/vk_common.h"

namespace vox {
namespace {
inline void ReadSubpassInfo(std::istringstream &is, std::vector<SubpassInfo> &value) {
    std::size_t size;
    utility::Read(is, size);
    value.resize(size);
    for (SubpassInfo &subpass : value) {
        utility::Read(is, subpass.input_attachments);
        utility::Read(is, subpass.output_attachments);
    }
}

inline void ReadProcesses(std::istringstream &is, std::vector<std::string> &value) {
    std::size_t size;
    utility::Read(is, size);
    value.resize(size);
    for (std::string &item : value) {
        utility::Read(is, item);
    }
}
}  // namespace

ResourceReplay::ResourceReplay() {
    stream_resources_[ResourceType::SHADER_MODULE] =
            std::bind(&ResourceReplay::CreateShaderModule, this, std::placeholders::_1, std::placeholders::_2);
    stream_resources_[ResourceType::PIPELINE_LAYOUT] =
            std::bind(&ResourceReplay::CreatePipelineLayout, this, std::placeholders::_1, std::placeholders::_2);
    stream_resources_[ResourceType::RENDER_PASS] =
            std::bind(&ResourceReplay::CreateRenderPass, this, std::placeholders::_1, std::placeholders::_2);
    stream_resources_[ResourceType::GRAPHICS_PIPELINE] =
            std::bind(&ResourceReplay::CreateGraphicsPipeline, this, std::placeholders::_1, std::placeholders::_2);
}

void ResourceReplay::Play(ResourceCache &resource_cache, ResourceRecord &recorder) {
    std::istringstream stream{recorder.GetStream().str()};

    while (true) {
        // Read command id
        ResourceType resource_type;
        utility::Read(stream, resource_type);

        if (stream.eof()) {
            break;
        }

        // Find command function for the given command id
        auto cmd_it = stream_resources_.find(resource_type);

        // Check if command replayer supports the given command
        if (cmd_it != stream_resources_.end()) {
            // Run command function
            cmd_it->second(resource_cache, stream);
        } else {
            LOGE("Replay command not supported.")
        }
    }
}

void ResourceReplay::CreateShaderModule(ResourceCache &resource_cache, std::istringstream &stream) {
    VkShaderStageFlagBits stage{};
    std::string glsl_source;
    std::string entry_point;
    std::string preamble;
    std::vector<std::string> processes;

    utility::Read(stream, stage, glsl_source, entry_point, preamble);

    ReadProcesses(stream, processes);

    ShaderSource shader_source{};
    shader_source.SetSource(glsl_source);
    ShaderVariant shader_variant(std::move(preamble), std::move(processes));

    auto &shader_module = resource_cache.RequestShaderModule(stage, shader_source, shader_variant);

    shader_modules_.push_back(&shader_module);
}

void ResourceReplay::CreatePipelineLayout(ResourceCache &resource_cache, std::istringstream &stream) {
    std::vector<size_t> shader_indices;

    utility::Read(stream, shader_indices);

    std::vector<ShaderModule *> shader_stages(shader_indices.size());
    std::transform(shader_indices.begin(), shader_indices.end(), shader_stages.begin(),
                   [&](size_t shader_index) { return shader_modules_.at(shader_index); });

    auto &pipeline_layout = resource_cache.RequestPipelineLayout(shader_stages);

    pipeline_layouts_.push_back(&pipeline_layout);
}

void ResourceReplay::CreateRenderPass(ResourceCache &resource_cache, std::istringstream &stream) {
    std::vector<Attachment> attachments;
    std::vector<LoadStoreInfo> load_store_infos;
    std::vector<SubpassInfo> subpasses;

    utility::Read(stream, attachments, load_store_infos);

    ReadSubpassInfo(stream, subpasses);

    auto &render_pass = resource_cache.RequestRenderPass(attachments, load_store_infos, subpasses);

    render_passes_.push_back(&render_pass);
}

void ResourceReplay::CreateGraphicsPipeline(ResourceCache &resource_cache, std::istringstream &stream) {
    size_t pipeline_layout_index{};
    size_t render_pass_index{};
    uint32_t subpass_index{};

    utility::Read(stream, pipeline_layout_index, render_pass_index, subpass_index);

    std::map<uint32_t, std::vector<uint8_t>> specialization_constant_state{};
    utility::Read(stream, specialization_constant_state);

    VertexInputState vertex_input_state{};

    utility::Read(stream, vertex_input_state.attributes, vertex_input_state.bindings);

    InputAssemblyState input_assembly_state{};
    RasterizationState rasterization_state{};
    ViewportState viewport_state{};
    MultisampleState multisample_state{};
    DepthStencilState depth_stencil_state{};

    utility::Read(stream, input_assembly_state, rasterization_state, viewport_state, multisample_state,
                  depth_stencil_state);

    ColorBlendState color_blend_state{};

    utility::Read(stream, color_blend_state.logic_op, color_blend_state.logic_op_enable, color_blend_state.attachments);

    PipelineState pipeline_state{};
    pipeline_state.SetPipelineLayout(*pipeline_layouts_.at(pipeline_layout_index));
    pipeline_state.SetRenderPass(*render_passes_.at(render_pass_index));

    for (auto &item : specialization_constant_state) {
        pipeline_state.SetSpecializationConstant(item.first, item.second);
    }

    pipeline_state.SetSubpassIndex(subpass_index);
    pipeline_state.SetVertexInputState(vertex_input_state);
    pipeline_state.SetInputAssemblyState(input_assembly_state);
    pipeline_state.SetRasterizationState(rasterization_state);
    pipeline_state.SetViewportState(viewport_state);
    pipeline_state.SetMultisampleState(multisample_state);
    pipeline_state.SetDepthStencilState(depth_stencil_state);
    pipeline_state.SetColorBlendState(color_blend_state);

    auto &graphics_pipeline = resource_cache.RequestGraphicsPipeline(pipeline_state);

    graphics_pipelines_.push_back(&graphics_pipeline);
}

}  // namespace vox
