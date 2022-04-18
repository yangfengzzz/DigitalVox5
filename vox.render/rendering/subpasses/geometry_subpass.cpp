//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "geometry_subpass.h"
#include "material/material.h"
#include "mesh/mesh.h"
#include "camera.h"
#include "renderer.h"
#include "components_manager.h"

namespace vox {
GeometrySubpass::GeometrySubpass(RenderContext &render_context, ShaderSource &&vertex_source,
                                 ShaderSource &&fragment_source, Scene *scene, Camera *camera) :
Subpass{render_context, std::move(vertex_source), std::move(fragment_source)},
camera_{camera},
scene_{scene} {
}

void GeometrySubpass::set_thread_index(uint32_t index) {
    thread_index_ = index;
}

void GeometrySubpass::draw(CommandBuffer &command_buffer) {
    auto compile_macros = ShaderVariant();
    
    std::vector<RenderElement> opaque_queue;
    std::vector<RenderElement> alpha_test_queue;
    std::vector<RenderElement> transparent_queue;
    ComponentsManager::get_singleton().call_render(camera_, opaque_queue, alpha_test_queue, transparent_queue);
    std::sort(opaque_queue.begin(), opaque_queue.end(), _compareFromNearToFar);
    std::sort(alpha_test_queue.begin(), alpha_test_queue.end(), _compareFromNearToFar);
    std::sort(transparent_queue.begin(), transparent_queue.end(), _compareFromFarToNear);
    
    draw_element(command_buffer, opaque_queue, compile_macros);
    draw_element(command_buffer, alpha_test_queue, compile_macros);
    draw_element(command_buffer, transparent_queue, compile_macros);
}

void GeometrySubpass::draw_element(CommandBuffer &command_buffer,
                                   const std::vector<RenderElement> &items,
                                   const ShaderVariant &variant) {
    for (auto &element : items) {
        auto &device = command_buffer.get_device();
        
        auto &renderer = element.renderer;
        auto &material = element.material;
        auto &sub_mesh = element.sub_mesh;
        auto &mesh = element.mesh;
        ScopedDebugLabel submesh_debug_label{command_buffer, mesh->name_.c_str()};
        
        // pipeline state
        command_buffer.set_rasterization_state(material->rasterization_state_);
        auto multisample = material->multisample_state_;
        multisample.rasterization_samples = sample_count_;
        command_buffer.set_multisample_state(multisample);
        command_buffer.set_depth_stencil_state(material->depth_stencil_state_);
        command_buffer.set_color_blend_state(material->color_blend_state_);
        command_buffer.set_input_assembly_state(material->input_assembly_state_);
        
        // shader
        auto &vert_shader_module =
        device.get_resource_cache().request_shader_module(VK_SHADER_STAGE_VERTEX_BIT, get_vertex_shader(), variant);
        auto &frag_shader_module =
        device.get_resource_cache().request_shader_module(VK_SHADER_STAGE_FRAGMENT_BIT, get_fragment_shader(), variant);
        std::vector<ShaderModule *> shader_modules{&vert_shader_module, &frag_shader_module};
        auto &pipeline_layout = prepare_pipeline_layout(command_buffer, shader_modules);
        command_buffer.bind_pipeline_layout(pipeline_layout);
        
        // uniform & texture
        DescriptorSetLayout &descriptor_set_layout = pipeline_layout.get_descriptor_set_layout(0);
        scene_->shader_data_.bind_data(command_buffer, descriptor_set_layout);
        camera_->shader_data_.bind_data(command_buffer, descriptor_set_layout);
        renderer->shader_data_.bind_data(command_buffer, descriptor_set_layout);
        material->shader_data_.bind_data(command_buffer, descriptor_set_layout);
        
        // vertex buffer
        command_buffer.set_vertex_input_state(mesh->vertex_input_state());
        for (uint32_t j = 0; j < mesh->vertex_buffer_bindings().size(); j++) {
            const auto &vertex_buffer_binding = mesh->vertex_buffer_bindings()[j];
            if (vertex_buffer_binding) {
                std::vector<std::reference_wrapper<const core::Buffer>> buffers;
                buffers.emplace_back(std::ref(*vertex_buffer_binding));
                command_buffer.bind_vertex_buffers(j, buffers, {0});
            }
        }
        // Draw submesh indexed if indices exists
        const auto &index_buffer_binding = mesh->index_buffer_binding();
        if (index_buffer_binding) {
            // Bind index buffer of submesh
            command_buffer.bind_index_buffer(index_buffer_binding->buffer(), 0, index_buffer_binding->index_type());
            
            // Draw submesh using indexed data
            command_buffer.draw_indexed(sub_mesh->count(), mesh->instance_count(), sub_mesh->start(), 0, 0);
        } else {
            // Draw submesh using vertices only
            command_buffer.draw(sub_mesh->count(), mesh->instance_count(), 0, 0);
        }
    }
}

PipelineLayout &GeometrySubpass::prepare_pipeline_layout(CommandBuffer &command_buffer,
                                                         const std::vector<ShaderModule *> &shader_modules) {
    // Sets any specified resource modes
    for (auto &shader_module : shader_modules) {
        for (auto &resource_mode : resource_mode_map_) {
            shader_module->set_resource_mode(resource_mode.first, resource_mode.second);
        }
    }
    
    return command_buffer.get_device().get_resource_cache().request_pipeline_layout(shader_modules);
}

}
