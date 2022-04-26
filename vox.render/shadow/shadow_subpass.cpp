//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "shadow_subpass.h"
#include "components_manager.h"
#include "shader/shader_manager.h"
#include "shadow_manager.h"
#include "camera.h"
#include "renderer.h"
#include "mesh/mesh.h"

namespace vox {
ShadowSubpass::ShadowSubpass(RenderContext &render_context, Scene *scene, Camera *camera) :
Subpass{render_context, scene, camera} {
    vertex_source_ = ShaderManager::get_singleton().load_shader("base/shadow-map.vert");
}

void ShadowSubpass::set_thread_index(uint32_t index) {
    thread_index_ = index;
}

void ShadowSubpass::set_view_projection_matrix(const Matrix4x4F &vp) {
    vp_ = vp;
}

void ShadowSubpass::set_viewport(const std::optional<VkViewport> &viewport) {
    if (viewport) {
        viewports_.resize(1);
        viewports_[0] = viewport.value();
    } else {
        viewports_.clear();
    }
}

void ShadowSubpass::prepare() {
    depth_stencil_state_.depth_compare_op = VK_COMPARE_OP_LESS_OR_EQUAL;
}

void ShadowSubpass::draw(CommandBuffer &command_buffer) {
    // pipeline state
    command_buffer.set_rasterization_state(rasterization_state_);
    multisample_state_.rasterization_samples = sample_count_;
    command_buffer.set_multisample_state(multisample_state_);
    command_buffer.set_depth_stencil_state(depth_stencil_state_);
    command_buffer.set_input_assembly_state(input_assembly_state_);
    command_buffer.set_depth_bias(-1.4f, 0.0f, -1.7f);
    
    auto &render_frame = render_context_.get_active_frame();
    auto allocation = render_frame.allocate_buffer(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, sizeof(Matrix4x4F), 0);
    allocation.update(vp_);
    command_buffer.bind_buffer(allocation.get_buffer(), allocation.get_offset(), allocation.get_size(), 0, 10, 0);
    
    if (!viewports_.empty()) {
        command_buffer.set_viewport(0, viewports_);
        
        VkExtent2D extent{ShadowManager::shadow_map_resolution_/2, ShadowManager::shadow_map_resolution_/2};
        VkRect2D scissor{};
        scissor.extent = extent;
        command_buffer.set_scissor(0, {scissor});
    } else {
        VkViewport viewport{};
        viewport.width = static_cast<float>(ShadowManager::shadow_map_resolution_);
        viewport.height = static_cast<float>(ShadowManager::shadow_map_resolution_);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        command_buffer.set_viewport(0, {viewport});
        
        VkExtent2D extent{ShadowManager::shadow_map_resolution_, ShadowManager::shadow_map_resolution_};
        VkRect2D scissor{};
        scissor.extent = extent;
        command_buffer.set_scissor(0, {scissor});
    }

    
    auto compile_macros = ShaderVariant();
    scene_->shader_data_.merge_variants(compile_macros, compile_macros);
    camera_->shader_data_.merge_variants(compile_macros, compile_macros);
    
    std::vector<RenderElement> opaque_queue;
    std::vector<RenderElement> alpha_test_queue;
    std::vector<RenderElement> transparent_queue;
    ComponentsManager::get_singleton().call_render(BoundingFrustum(vp_), opaque_queue, alpha_test_queue, transparent_queue);
    std::sort(opaque_queue.begin(), opaque_queue.end(), _compareFromNearToFar);
    std::sort(alpha_test_queue.begin(), alpha_test_queue.end(), _compareFromNearToFar);
    std::sort(transparent_queue.begin(), transparent_queue.end(), _compareFromFarToNear);
    
    draw_element(command_buffer, opaque_queue, compile_macros);
    draw_element(command_buffer, alpha_test_queue, compile_macros);
    draw_element(command_buffer, transparent_queue, compile_macros);
}

void ShadowSubpass::draw_element(CommandBuffer &command_buffer,
                                 const std::vector<RenderElement> &items,
                                 const ShaderVariant &variant) {
    auto &device = command_buffer.get_device();
    for (auto &element : items) {
        auto macros = variant;
        auto &renderer = element.renderer;
        if (renderer->cast_shadow_) {
            renderer->update_shader_data();
            renderer->shader_data_.merge_variants(macros, macros);
            
            // shader
            auto &vert_shader_module =
            device.get_resource_cache().request_shader_module(VK_SHADER_STAGE_VERTEX_BIT, *vertex_source_, macros);
            std::vector<ShaderModule *> shader_modules{&vert_shader_module};
            auto &pipeline_layout = prepare_pipeline_layout(command_buffer, shader_modules);
            command_buffer.bind_pipeline_layout(pipeline_layout);
            
            // uniform & texture
            DescriptorSetLayout &descriptor_set_layout = pipeline_layout.get_descriptor_set_layout(0);
            renderer->shader_data_.bind_data(command_buffer, descriptor_set_layout);
            
            auto &sub_mesh = element.sub_mesh;
            auto &mesh = element.mesh;
            ScopedDebugLabel submesh_debug_label{command_buffer, mesh->name_.c_str()};
            
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
}

}
