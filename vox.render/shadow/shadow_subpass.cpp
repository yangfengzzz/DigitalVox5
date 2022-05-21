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
    vertex_source_ = ShaderManager::GetSingleton().LoadShader("base/shadow/shadow-map.vert");
}

void ShadowSubpass::SetThreadIndex(uint32_t index) {
    thread_index_ = index;
}

void ShadowSubpass::SetViewProjectionMatrix(const Matrix4x4F &vp) {
    vp_ = vp;
}

void ShadowSubpass::SetViewport(const std::optional<VkViewport> &viewport) {
    if (viewport) {
        viewports_.resize(1);
        viewports_[0] = viewport.value();
    } else {
        viewports_.clear();
    }
}

void ShadowSubpass::Prepare() {
    rasterization_state_.depth_bias_enable = VK_TRUE;
    depth_stencil_state_.depth_compare_op = VK_COMPARE_OP_LESS_OR_EQUAL;
}

void ShadowSubpass::Draw(CommandBuffer &command_buffer) {
    // pipeline state
    command_buffer.SetRasterizationState(rasterization_state_);
    multisample_state_.rasterization_samples = sample_count_;
    command_buffer.SetMultisampleState(multisample_state_);
    command_buffer.SetDepthStencilState(depth_stencil_state_);
    command_buffer.SetInputAssemblyState(input_assembly_state_);
    command_buffer.SetDepthBias(0.01, 0.01, 1.0);
    
    auto &render_frame = render_context_.GetActiveFrame();
    auto allocation = render_frame.AllocateBuffer(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, sizeof(Matrix4x4F), 0);
    allocation.Update(vp_);
    command_buffer.BindBuffer(allocation.GetBuffer(), allocation.GetOffset(), allocation.GetSize(), 0, 10, 0);
    
    if (!viewports_.empty()) {
        command_buffer.SetViewport(0, viewports_);
    } else {
        VkViewport viewport{};
        viewport.width = static_cast<float>(ShadowManager::shadow_map_resolution_);
        viewport.height = static_cast<float>(ShadowManager::shadow_map_resolution_);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        command_buffer.SetViewport(0, {viewport});
    }

    
    auto compile_macros = ShaderVariant();
    scene_->shader_data_.MergeVariants(compile_macros, compile_macros);
    camera_->shader_data_.MergeVariants(compile_macros, compile_macros);
    
    std::vector<RenderElement> opaque_queue;
    std::vector<RenderElement> alpha_test_queue;
    std::vector<RenderElement> transparent_queue;
    ComponentsManager::GetSingleton().call_render(BoundingFrustum(vp_), opaque_queue, alpha_test_queue, transparent_queue);
    std::sort(opaque_queue.begin(), opaque_queue.end(), CompareFromNearToFar);
    std::sort(alpha_test_queue.begin(), alpha_test_queue.end(), CompareFromNearToFar);
    std::sort(transparent_queue.begin(), transparent_queue.end(), CompareFromFarToNear);

    DrawElement(command_buffer, opaque_queue, compile_macros);
    DrawElement(command_buffer, alpha_test_queue, compile_macros);
    DrawElement(command_buffer, transparent_queue, compile_macros);
}

void ShadowSubpass::DrawElement(CommandBuffer &command_buffer,
                                 const std::vector<RenderElement> &items,
                                 const ShaderVariant &variant) {
    auto &device = command_buffer.GetDevice();
    for (auto &element : items) {
        auto macros = variant;
        auto &renderer = element.renderer;
        if (renderer->cast_shadow_) {
            renderer->update_shader_data();
            renderer->shader_data_.MergeVariants(macros, macros);
            
            // shader
            auto &vert_shader_module =
                    device.GetResourceCache().RequestShaderModule(VK_SHADER_STAGE_VERTEX_BIT, *vertex_source_, macros);
            std::vector<ShaderModule *> shader_modules{&vert_shader_module};
            auto &pipeline_layout = PreparePipelineLayout(command_buffer, shader_modules);
            command_buffer.BindPipelineLayout(pipeline_layout);
            
            // uniform & texture
            DescriptorSetLayout &descriptor_set_layout = pipeline_layout.GetDescriptorSetLayout(0);
            renderer->shader_data_.BindData(command_buffer, descriptor_set_layout);
            
            auto &sub_mesh = element.sub_mesh;
            auto &mesh = element.mesh;
            ScopedDebugLabel submesh_debug_label{command_buffer, mesh->name_.c_str()};
            
            // vertex buffer
            command_buffer.SetVertexInputState(mesh->vertex_input_state());
            for (uint32_t j = 0; j < mesh->vertex_buffer_count(); j++) {
                const auto vertex_buffer_binding = mesh->vertex_buffer(j);
                if (vertex_buffer_binding) {
                    std::vector<std::reference_wrapper<const core::Buffer>> buffers;
                    buffers.emplace_back(std::ref(*vertex_buffer_binding));
                    command_buffer.BindVertexBuffers(j, buffers, {0});
                }
            }
            // Draw submesh indexed if indices exists
            const auto &index_buffer_binding = mesh->index_buffer_binding();
            if (index_buffer_binding) {
                // Bind index buffer of submesh
                command_buffer.BindIndexBuffer(index_buffer_binding->buffer(), 0, index_buffer_binding->index_type());
                
                // Draw submesh using indexed data
                command_buffer.DrawIndexed(sub_mesh->count(), mesh->instance_count(), sub_mesh->start(), 0, 0);
            } else {
                // Draw submesh using vertices only
                command_buffer.Draw(sub_mesh->count(), mesh->instance_count(), 0, 0);
            }
        }
    }
}

}
