//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "color_picker_subpass.h"
#include "renderer.h"
#include "camera.h"
#include "mesh/mesh.h"
#include "components_manager.h"
#include "logging.h"
#include "shader/shader_manager.h"

namespace vox {
ColorPickerMaterial::ColorPickerMaterial(Device &device) :
BaseMaterial(device) {
    vertex_source_ = ShaderManager::GetSingleton().LoadShader("base/unlit.vert");
    fragment_source_ = ShaderManager::GetSingleton().LoadShader("base/editor/color_picker.frag");
}

ColorPickerSubpass::ColorPickerSubpass(RenderContext &render_context, Scene *scene, Camera *camera) :
Subpass{render_context, scene, camera},
material_(render_context_.GetDevice()) {
}

void ColorPickerSubpass::AddExclusiveRenderer(Renderer* renderer) {
    exclusive_list_.emplace_back(renderer);
}

void ColorPickerSubpass::ClearExclusiveList() {
    exclusive_list_.clear();
}

Color ColorPickerSubpass::IdToColor(uint32_t id) {
    if (id >= 0xffffff) {
        LOGE("Framebuffer Picker encounter primitive's id greater than {}", std::to_string(0xffffff))
    }
    
    return {(id & 0xff) / 255.f, ((id & 0xff00) >> 8) / 255.f, ((id & 0xff0000) >> 16) / 255.f, 1.f};
}

uint32_t ColorPickerSubpass::ColorToId(const std::array<uint8_t, 4> &color) {
    return color[0] | (color[1] << 8) | (color[2] << 16);
}

std::pair<Renderer *, MeshPtr> ColorPickerSubpass::GetObjectByColor(const std::array<uint8_t, 4> &color) {
    auto iter = primitives_map_.find(ColorToId(color));
    if (iter != primitives_map_.end()) {
        return iter->second;
    } else {
        return std::make_pair(nullptr, nullptr);
    }
}

void ColorPickerSubpass::Prepare() {
    
}

void ColorPickerSubpass::Draw(CommandBuffer &command_buffer) {
    current_id_ = 0;
    primitives_map_.clear();
    
    auto compile_variant = ShaderVariant();
    scene_->shader_data_.MergeVariants(compile_variant, compile_variant);
    camera_->shader_data_.MergeVariants(compile_variant, compile_variant);
    
    std::vector<RenderElement> opaque_queue;
    std::vector<RenderElement> alpha_test_queue;
    std::vector<RenderElement> transparent_queue;
    ComponentsManager::GetSingleton().call_render(camera_, opaque_queue, alpha_test_queue, transparent_queue);
    std::sort(opaque_queue.begin(), opaque_queue.end(), CompareFromNearToFar);
    std::sort(alpha_test_queue.begin(), alpha_test_queue.end(), CompareFromNearToFar);
    std::sort(transparent_queue.begin(), transparent_queue.end(), CompareFromFarToNear);

    DrawElement(command_buffer, opaque_queue, compile_variant);
    DrawElement(command_buffer, alpha_test_queue, compile_variant);
    DrawElement(command_buffer, transparent_queue, compile_variant);
}

void ColorPickerSubpass::DrawElement(CommandBuffer &command_buffer,
                                      const std::vector<RenderElement> &items,
                                      const ShaderVariant &variant) {
    auto &device = command_buffer.GetDevice();
    for (auto &element : items) {
        auto exclusive = std::find(exclusive_list_.begin(), exclusive_list_.end(), element.renderer);
        if (exclusive != exclusive_list_.end()) {
            continue;
        }
        auto macros = variant;
        auto &renderer = element.renderer;
        renderer->update_shader_data();
        renderer->shader_data_.MergeVariants(macros, macros);
        auto &sub_mesh = element.sub_mesh;
        auto &mesh = element.mesh;
        ScopedDebugLabel submesh_debug_label{command_buffer, mesh->name_.c_str()};
        
        primitives_map_[current_id_] = std::make_pair(renderer, mesh);
        auto &render_frame = render_context_.GetActiveFrame();
        auto allocation = render_frame.AllocateBuffer(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, sizeof(Color), 0);
        allocation.Update(IdToColor(current_id_));
        current_id_ += 1;
        
        // pipeline state
        material_.multisample_state_.rasterization_samples = sample_count_;
        command_buffer.SetMultisampleState(material_.multisample_state_);
        command_buffer.SetDepthStencilState(material_.depth_stencil_state_);
        command_buffer.SetColorBlendState(material_.color_blend_state_);
        command_buffer.SetInputAssemblyState(material_.input_assembly_state_);
        command_buffer.SetRasterizationState(material_.rasterization_state_);
        
        // shader
        auto &vert_shader_module = device.GetResourceCache().RequestShaderModule(VK_SHADER_STAGE_VERTEX_BIT,
                                                                                 *material_.vertex_source_, macros);
        auto &frag_shader_module = device.GetResourceCache().RequestShaderModule(VK_SHADER_STAGE_FRAGMENT_BIT,
                                                                                 *material_.fragment_source_, macros);
        std::vector<ShaderModule *> shader_modules{&vert_shader_module, &frag_shader_module};
        auto &pipeline_layout = PreparePipelineLayout(command_buffer, shader_modules);
        command_buffer.BindPipelineLayout(pipeline_layout);
        
        // uniform & texture
        DescriptorSetLayout &descriptor_set_layout = pipeline_layout.GetDescriptorSetLayout(0);
        scene_->shader_data_.BindData(command_buffer, descriptor_set_layout);
        camera_->shader_data_.BindData(command_buffer, descriptor_set_layout);
        renderer->shader_data_.BindData(command_buffer, descriptor_set_layout);
        command_buffer.BindBuffer(allocation.GetBuffer(), allocation.GetOffset(), allocation.GetSize(), 0, 10, 0);
        
        // vertex buffer
        command_buffer.SetVertexInputState(mesh->vertex_input_state());
        for (uint32_t j = 0; j < mesh->vertex_buffer_count(); j++) {
            const auto kVertexBufferBinding = mesh->vertex_buffer(j);
            if (kVertexBufferBinding) {
                std::vector<std::reference_wrapper<const core::Buffer>> buffers;
                buffers.emplace_back(std::ref(*kVertexBufferBinding));
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
