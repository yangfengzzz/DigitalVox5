//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/rendering/subpasses/geometry_subpass.h"

#include "vox.render/camera.h"
#include "vox.render/components_manager.h"
#include "vox.render/material/material.h"
#include "vox.render/mesh/mesh.h"
#include "vox.render/renderer.h"
#include "vox.render/shader/internal_variant_name.h"
#include "vox.render/shadow/shadow_manager.h"

namespace vox {
GeometrySubpass::GeometrySubpass(RenderContext &render_context, Scene *scene, Camera *camera)
    : Subpass{render_context, scene, camera} {}

GeometrySubpass::RenderMode GeometrySubpass::GetRenderMode() const { return mode_; }

void GeometrySubpass::SetRenderMode(RenderMode mode) { mode_ = mode; }

void GeometrySubpass::AddRenderElement(const RenderElement &element) { elements_.emplace_back(element); }

void GeometrySubpass::ClearAllRenderElement() { elements_.clear(); }

void GeometrySubpass::SetThreadIndex(uint32_t index) { thread_index_ = index; }

void GeometrySubpass::Prepare() {}

void GeometrySubpass::Draw(CommandBuffer &command_buffer) {
    auto compile_variant = ShaderVariant();
    scene_->shader_data.MergeVariants(compile_variant, compile_variant);
    if (camera_) {
        camera_->shader_data_.MergeVariants(compile_variant, compile_variant);
    }

    if (mode_ == RenderMode::MANUAL) {
        DrawElement(command_buffer, elements_, compile_variant);
    } else {
        std::vector<RenderElement> opaque_queue;
        std::vector<RenderElement> alpha_test_queue;
        std::vector<RenderElement> transparent_queue;
        ComponentsManager::GetSingleton().CallRender(camera_, opaque_queue, alpha_test_queue, transparent_queue);
        std::sort(opaque_queue.begin(), opaque_queue.end(), CompareFromNearToFar);
        std::sort(alpha_test_queue.begin(), alpha_test_queue.end(), CompareFromNearToFar);
        std::sort(transparent_queue.begin(), transparent_queue.end(), CompareFromFarToNear);

        DrawElement(command_buffer, opaque_queue, compile_variant);
        DrawElement(command_buffer, alpha_test_queue, compile_variant);
        DrawElement(command_buffer, transparent_queue, compile_variant);
    }
}

void GeometrySubpass::DrawElement(CommandBuffer &command_buffer,
                                  const std::vector<RenderElement> &items,
                                  const ShaderVariant &variant) {
    auto &device = command_buffer.GetDevice();
    for (auto &element : items) {
        auto macros = variant;
        auto &renderer = element.renderer;
        uint32_t shadow_count = ShadowManager::GetSingleton().ShadowCount();
        if (renderer->receive_shadow_ && shadow_count != 0) {
            renderer->shader_data_.AddDefine(SHADOW_MAP_COUNT + std::to_string(shadow_count));
        }
        renderer->UpdateShaderData();
        renderer->shader_data_.MergeVariants(macros, macros);

        auto &material = element.material;
        material->shader_data_.MergeVariants(macros, macros);

        auto &sub_mesh = element.sub_mesh;
        auto &mesh = element.mesh;
        ScopedDebugLabel submesh_debug_label{command_buffer, mesh->name_.c_str()};

        // pipeline state
        material->rasterization_state_.depth_bias_enable = VK_TRUE;
        command_buffer.SetRasterizationState(material->rasterization_state_);
        command_buffer.SetDepthBias(0.01, 0.01, 1.0);

        auto multisample = material->multisample_state_;
        multisample.rasterization_samples = sample_count_;
        command_buffer.SetMultisampleState(multisample);
        command_buffer.SetDepthStencilState(material->depth_stencil_state_);
        command_buffer.SetColorBlendState(material->color_blend_state_);
        command_buffer.SetInputAssemblyState(material->input_assembly_state_);

        // shader
        auto &vert_shader_module = device.GetResourceCache().RequestShaderModule(VK_SHADER_STAGE_VERTEX_BIT,
                                                                                 *material->vertex_source_, macros);
        auto &frag_shader_module = device.GetResourceCache().RequestShaderModule(VK_SHADER_STAGE_FRAGMENT_BIT,
                                                                                 *material->fragment_source_, macros);
        std::vector<ShaderModule *> shader_modules{&vert_shader_module, &frag_shader_module};
        auto &pipeline_layout = PreparePipelineLayout(command_buffer, shader_modules);
        command_buffer.BindPipelineLayout(pipeline_layout);

        // uniform & texture
        DescriptorSetLayout &descriptor_set_layout = pipeline_layout.GetDescriptorSetLayout(0);
        scene_->shader_data.BindData(command_buffer, descriptor_set_layout);
        camera_->shader_data_.BindData(command_buffer, descriptor_set_layout);
        renderer->shader_data_.BindData(command_buffer, descriptor_set_layout);
        material->shader_data_.BindData(command_buffer, descriptor_set_layout);

        // vertex buffer
        command_buffer.SetVertexInputState(mesh->VertexInputState());
        for (uint32_t j = 0; j < mesh->VertexBufferCount(); j++) {
            const auto kVertexBufferBinding = mesh->VertexBuffer(j);
            if (kVertexBufferBinding) {
                std::vector<std::reference_wrapper<const core::Buffer>> buffers;
                buffers.emplace_back(std::ref(*kVertexBufferBinding));
                command_buffer.BindVertexBuffers(j, buffers, {0});
            }
        }
        // Draw submesh indexed if indices exists
        const auto &index_buffer_binding = mesh->IndexBufferBinding();
        if (index_buffer_binding) {
            // Bind index buffer of submesh
            command_buffer.BindIndexBuffer(index_buffer_binding->Buffer(), 0, index_buffer_binding->IndexType());

            // Draw submesh using indexed data
            command_buffer.DrawIndexed(sub_mesh->Count(), mesh->InstanceCount(), sub_mesh->Start(), 0, 0);
        } else {
            // Draw submesh using vertices only
            command_buffer.Draw(sub_mesh->Count(), mesh->InstanceCount(), 0, 0);
        }
    }
}

}  // namespace vox
