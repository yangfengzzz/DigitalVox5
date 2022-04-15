//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "resource_replay.h"

#include "logging.h"
#include "vk_common.h"
#include "rendering/pipeline_state.h"
#include "resource_cache.h"

namespace vox {
namespace {
inline void read_subpass_info(std::istringstream &is, std::vector<SubpassInfo> &value) {
    std::size_t size;
    read(is, size);
    value.resize(size);
    for (SubpassInfo &subpass : value) {
        read(is, subpass.input_attachments);
        read(is, subpass.output_attachments);
    }
}

inline void read_processes(std::istringstream &is, std::vector<std::string> &value) {
    std::size_t size;
    read(is, size);
    value.resize(size);
    for (std::string &item : value) {
        read(is, item);
    }
}
}        // namespace

ResourceReplay::ResourceReplay() {
    stream_resources_[ResourceType::SHADER_MODULE] = std::bind(&ResourceReplay::create_shader_module, this,
                                                               std::placeholders::_1, std::placeholders::_2);
    stream_resources_[ResourceType::PIPELINE_LAYOUT] = std::bind(&ResourceReplay::create_pipeline_layout, this,
                                                                 std::placeholders::_1, std::placeholders::_2);
    stream_resources_[ResourceType::RENDER_PASS] = std::bind(&ResourceReplay::create_render_pass, this,
                                                             std::placeholders::_1, std::placeholders::_2);
    stream_resources_[ResourceType::GRAPHICS_PIPELINE] = std::bind(&ResourceReplay::create_graphics_pipeline, this,
                                                                   std::placeholders::_1, std::placeholders::_2);
}

void ResourceReplay::play(ResourceCache &resource_cache, ResourceRecord &recorder) {
    std::istringstream stream{recorder.get_stream().str()};
    
    while (true) {
        // Read command id
        ResourceType resource_type;
        read(stream, resource_type);
        
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

void ResourceReplay::create_shader_module(ResourceCache &resource_cache, std::istringstream &stream) {
    VkShaderStageFlagBits stage{};
    std::string glsl_source;
    std::string entry_point;
    std::string preamble;
    std::vector<std::string> processes;
    
    read(stream,
         stage,
         glsl_source,
         entry_point,
         preamble);
    
    read_processes(stream, processes);
    
    ShaderSource shader_source{};
    shader_source.set_source(glsl_source);
    ShaderVariant shader_variant(std::move(preamble), std::move(processes));
    
    auto &shader_module = resource_cache.request_shader_module(stage, shader_source, shader_variant);
    
    shader_modules_.push_back(&shader_module);
}

void ResourceReplay::create_pipeline_layout(ResourceCache &resource_cache, std::istringstream &stream) {
    std::vector<size_t> shader_indices;
    
    read(stream,
         shader_indices);
    
    std::vector<ShaderModule *> shader_stages(shader_indices.size());
    std::transform(shader_indices.begin(), shader_indices.end(), shader_stages.begin(),
                   [&](size_t shader_index) { return shader_modules_.at(shader_index); });
    
    auto &pipeline_layout = resource_cache.request_pipeline_layout(shader_stages);
    
    pipeline_layouts_.push_back(&pipeline_layout);
}

void ResourceReplay::create_render_pass(ResourceCache &resource_cache, std::istringstream &stream) {
    std::vector<Attachment> attachments;
    std::vector<LoadStoreInfo> load_store_infos;
    std::vector<SubpassInfo> subpasses;
    
    read(stream,
         attachments,
         load_store_infos);
    
    read_subpass_info(stream, subpasses);
    
    auto &render_pass = resource_cache.request_render_pass(attachments, load_store_infos, subpasses);
    
    render_passes_.push_back(&render_pass);
}

void ResourceReplay::create_graphics_pipeline(ResourceCache &resource_cache, std::istringstream &stream) {
    size_t pipeline_layout_index{};
    size_t render_pass_index{};
    uint32_t subpass_index{};
    
    read(stream,
         pipeline_layout_index,
         render_pass_index,
         subpass_index);
    
    std::map<uint32_t, std::vector<uint8_t>> specialization_constant_state{};
    read(stream,
         specialization_constant_state);
    
    VertexInputState vertex_input_state{};
    
    read(stream,
         vertex_input_state.attributes,
         vertex_input_state.bindings);
    
    InputAssemblyState input_assembly_state{};
    RasterizationState rasterization_state{};
    ViewportState viewport_state{};
    MultisampleState multisample_state{};
    DepthStencilState depth_stencil_state{};
    
    read(stream,
         input_assembly_state,
         rasterization_state,
         viewport_state,
         multisample_state,
         depth_stencil_state);
    
    ColorBlendState color_blend_state{};
    
    read(stream,
         color_blend_state.logic_op,
         color_blend_state.logic_op_enable,
         color_blend_state.attachments);
    
    PipelineState pipeline_state{};
    pipeline_state.set_pipeline_layout(*pipeline_layouts_.at(pipeline_layout_index));
    pipeline_state.set_render_pass(*render_passes_.at(render_pass_index));
    
    for (auto &item : specialization_constant_state) {
        pipeline_state.set_specialization_constant(item.first, item.second);
    }
    
    pipeline_state.set_subpass_index(subpass_index);
    pipeline_state.set_vertex_input_state(vertex_input_state);
    pipeline_state.set_input_assembly_state(input_assembly_state);
    pipeline_state.set_rasterization_state(rasterization_state);
    pipeline_state.set_viewport_state(viewport_state);
    pipeline_state.set_multisample_state(multisample_state);
    pipeline_state.set_depth_stencil_state(depth_stencil_state);
    pipeline_state.set_color_blend_state(color_blend_state);
    
    auto &graphics_pipeline = resource_cache.request_graphics_pipeline(pipeline_state);
    
    graphics_pipelines_.push_back(&graphics_pipeline);
}

}        // namespace vox
