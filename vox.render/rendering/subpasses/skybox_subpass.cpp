//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "skybox_subpass.h"
#include "mesh/primitive_mesh.h"
#include "camera.h"

namespace vox {
SkyboxSubpass::SkyboxSubpass(RenderContext &render_context, Scene *scene, Camera *camera) :
Subpass{render_context, scene, camera},
vert_shader_module_(render_context.get_device().get_resource_cache().request_shader_module(VK_SHADER_STAGE_VERTEX_BIT,
                                                                                           ShaderSource(""), ShaderVariant())),
frag_shader_module_(render_context.get_device().get_resource_cache().request_shader_module(VK_SHADER_STAGE_VERTEX_BIT,
                                                                                           ShaderSource(""), ShaderVariant())) {
}

void SkyboxSubpass::create_sphere(float radius) {
    mesh_ = PrimitiveMesh::create_sphere(render_context_.get_device(), radius);
    type_ = SkyBoxType::SPHERE;
}

void SkyboxSubpass::create_cuboid() {
    mesh_ = PrimitiveMesh::create_cuboid(render_context_.get_device(), 1, 1, 1);
    type_ = SkyBoxType::CUBOID;
}

const std::shared_ptr<Image> &SkyboxSubpass::texture_cube_map() const {
    return cube_map_;
}

void SkyboxSubpass::set_texture_cube_map(const std::shared_ptr<Image> &v) {
    cube_map_ = v;
}

void SkyboxSubpass::prepare() {
    vp_matrix_ = std::make_unique<core::Buffer>(render_context_.get_device(), sizeof(Matrix4x4F),
                                                VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                                                VMA_MEMORY_USAGE_CPU_TO_GPU);
    
    depth_stencil_state_.depth_write_enable = false;
    depth_stencil_state_.depth_compare_op = VK_COMPARE_OP_LESS_OR_EQUAL;
    
    // Create a default sampler
    VkSamplerCreateInfo sampler_create_info = {};
    sampler_create_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    sampler_create_info.magFilter = VK_FILTER_LINEAR;
    sampler_create_info.minFilter = VK_FILTER_LINEAR;
    sampler_create_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    sampler_create_info.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    sampler_create_info.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    sampler_create_info.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    sampler_create_info.mipLodBias = 0.0f;
    sampler_create_info.compareOp = VK_COMPARE_OP_NEVER;
    sampler_create_info.minLod = 0.0f;
    // Max level-of-detail should match mip level count
    sampler_create_info.maxLod = std::numeric_limits<float>::max();
    // Only enable anisotropic filtering if enabled on the devicec
    sampler_create_info.maxAnisotropy = render_context_.get_device().get_gpu().get_features().samplerAnisotropy ?
    render_context_.get_device().get_gpu().get_properties().limits.maxSamplerAnisotropy : 1.0f;
    sampler_create_info.anisotropyEnable = render_context_.get_device().get_gpu().get_features().samplerAnisotropy;
    sampler_create_info.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
    cube_sampler_ = std::make_unique<core::Sampler>(render_context_.get_device(), sampler_create_info);
}

void SkyboxSubpass::draw(CommandBuffer &command_buffer) {
    const auto kProjectionMatrix = camera_->projection_matrix();
    auto view_matrix = camera_->view_matrix();
    if (type_ == SkyBoxType::CUBOID) {
        view_matrix[12] = 0;
        view_matrix[13] = 0;
        view_matrix[14] = 0;
        view_matrix[15] = 1;
    }
    auto matrix = kProjectionMatrix * view_matrix;
    std::vector<uint8_t> bytes = to_bytes(matrix);
    vp_matrix_->update(bytes.data(), bytes.size());
    
    // pipeline state
    command_buffer.set_depth_stencil_state(depth_stencil_state_);
    
    // shader
    std::vector<ShaderModule *> shader_modules{&vert_shader_module_, &frag_shader_module_};
    auto &pipeline_layout = prepare_pipeline_layout(command_buffer, shader_modules);
    command_buffer.bind_pipeline_layout(pipeline_layout);
    
    // uniform & texture
    command_buffer.bind_buffer(*vp_matrix_, 0, vp_matrix_->get_size(), 0, 10, 0);
    command_buffer.bind_image(cube_map_->get_vk_image_view(), *cube_sampler_, 0, 11, 0);
    
    // vertex buffer
    command_buffer.set_vertex_input_state(mesh_->vertex_input_state());
    for (uint32_t j = 0; j < mesh_->vertex_buffer_bindings().size(); j++) {
        const auto &vertex_buffer_binding = mesh_->vertex_buffer_bindings()[j];
        if (vertex_buffer_binding) {
            std::vector<std::reference_wrapper<const core::Buffer>> buffers;
            buffers.emplace_back(std::ref(*vertex_buffer_binding));
            command_buffer.bind_vertex_buffers(j, buffers, {0});
        }
    }
    // Draw submesh indexed if indices exists
    const auto &index_buffer_binding = mesh_->index_buffer_binding();
    // Bind index buffer of submesh
    command_buffer.bind_index_buffer(index_buffer_binding->buffer(), 0, index_buffer_binding->index_type());
    
    // Draw submesh using indexed data
    command_buffer.draw_indexed(mesh_->sub_mesh()->count(), mesh_->instance_count(), mesh_->sub_mesh()->start(), 0, 0);
}

}
