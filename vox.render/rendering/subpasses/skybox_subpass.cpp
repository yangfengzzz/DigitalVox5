//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "skybox_subpass.h"
#include "mesh/model_mesh.h"
#include "camera.h"

namespace vox {
SkyboxSubpass::SkyboxSubpass(RenderContext &render_context, Scene *scene, Camera *camera) :
Subpass{render_context, scene, camera},
vert_shader_("base/skybox.vert"),
frag_shader_("base/skybox.frag") {
}

void SkyboxSubpass::create_cuboid() {
    mesh_ = std::make_shared<ModelMesh>(render_context_.get_device());
    
    const float kHalfLength = 0.5f;
    auto positions = std::vector<Vector3F>(24);
    // Up
    positions[0] = Vector3F(-kHalfLength, kHalfLength, -kHalfLength);
    positions[1] = Vector3F(kHalfLength, kHalfLength, -kHalfLength);
    positions[2] = Vector3F(kHalfLength, kHalfLength, kHalfLength);
    positions[3] = Vector3F(-kHalfLength, kHalfLength, kHalfLength);
    // Down
    positions[4] = Vector3F(-kHalfLength, -kHalfLength, -kHalfLength);
    positions[5] = Vector3F(kHalfLength, -kHalfLength, -kHalfLength);
    positions[6] = Vector3F(kHalfLength, -kHalfLength, kHalfLength);
    positions[7] = Vector3F(-kHalfLength, -kHalfLength, kHalfLength);
    // Left
    positions[8] = Vector3F(-kHalfLength, kHalfLength, -kHalfLength);
    positions[9] = Vector3F(-kHalfLength, kHalfLength, kHalfLength);
    positions[10] = Vector3F(-kHalfLength, -kHalfLength, kHalfLength);
    positions[11] = Vector3F(-kHalfLength, -kHalfLength, -kHalfLength);
    // Right
    positions[12] = Vector3F(kHalfLength, kHalfLength, -kHalfLength);
    positions[13] = Vector3F(kHalfLength, kHalfLength, kHalfLength);
    positions[14] = Vector3F(kHalfLength, -kHalfLength, kHalfLength);
    positions[15] = Vector3F(kHalfLength, -kHalfLength, -kHalfLength);
    // Front
    positions[16] = Vector3F(-kHalfLength, kHalfLength, kHalfLength);
    positions[17] = Vector3F(kHalfLength, kHalfLength, kHalfLength);
    positions[18] = Vector3F(kHalfLength, -kHalfLength, kHalfLength);
    positions[19] = Vector3F(-kHalfLength, -kHalfLength, kHalfLength);
    // Back
    positions[20] = Vector3F(-kHalfLength, kHalfLength, -kHalfLength);
    positions[21] = Vector3F(kHalfLength, kHalfLength, -kHalfLength);
    positions[22] = Vector3F(kHalfLength, -kHalfLength, -kHalfLength);
    positions[23] = Vector3F(-kHalfLength, -kHalfLength, -kHalfLength);
    
    auto indices = std::vector<uint16_t>(36);
    // Up
    indices[0] = 0;
    indices[1] = 2;
    indices[2] = 1;
    indices[3] = 2;
    indices[4] = 0;
    indices[5] = 3;
    // Down
    indices[6] = 4;
    indices[7] = 6;
    indices[8] = 7;
    indices[9] = 6;
    indices[10] = 4;
    indices[11] = 5;
    // Left
    indices[12] = 8;
    indices[13] = 10;
    indices[14] = 9;
    indices[15] = 10;
    indices[16] = 8;
    indices[17] = 11;
    // Right
    indices[18] = 12;
    indices[19] = 14;
    indices[20] = 15;
    indices[21] = 14;
    indices[22] = 12;
    indices[23] = 13;
    // Front
    indices[24] = 16;
    indices[25] = 18;
    indices[26] = 17;
    indices[27] = 18;
    indices[28] = 16;
    indices[29] = 19;
    // Back
    indices[30] = 20;
    indices[31] = 22;
    indices[32] = 23;
    indices[33] = 22;
    indices[34] = 20;
    indices[35] = 21;
    
    auto &bounds = mesh_->bounds_;
    bounds.lowerCorner = Point3F(-kHalfLength, -kHalfLength, -kHalfLength);
    bounds.upperCorner = Point3F(kHalfLength, kHalfLength, kHalfLength);
    
    mesh_->set_positions(positions);
    mesh_->set_indices(indices);
    mesh_->upload_data(true);
    mesh_->add_sub_mesh(0, static_cast<uint32_t>(indices.size()));
}

const std::shared_ptr<Image> &SkyboxSubpass::texture_cube_map() const {
    return cube_map_;
}

void SkyboxSubpass::set_texture_cube_map(const std::shared_ptr<Image> &v) {
    cube_map_ = v;
}

void SkyboxSubpass::flip_vertically() {
    is_flip_vertically_ = true;
}

void SkyboxSubpass::prepare() {
    auto &device = render_context_.get_device();
    vp_matrix_ = std::make_unique<core::Buffer>(device, sizeof(Matrix4x4F),
                                                VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                                                VMA_MEMORY_USAGE_CPU_TO_GPU);
    if (is_flip_vertically_) {
        variant_.add_define("NEED_FLIP_Y");
    }
    device.get_resource_cache().request_shader_module(VK_SHADER_STAGE_VERTEX_BIT,
                                                      vert_shader_, variant_);
    device.get_resource_cache().request_shader_module(VK_SHADER_STAGE_FRAGMENT_BIT,
                                                      frag_shader_, variant_);
    
    depth_stencil_state_.depth_write_enable = false;
    depth_stencil_state_.depth_compare_op = VK_COMPARE_OP_LESS_OR_EQUAL;
    color_blend_state_.attachments.resize(1);
    rasterization_state_.cull_mode = VK_CULL_MODE_BACK_BIT;
    
    // Create a default sampler
    VkSamplerCreateInfo sampler_create_info = {};
    sampler_create_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    sampler_create_info.magFilter = VK_FILTER_LINEAR;
    sampler_create_info.minFilter = VK_FILTER_LINEAR;
    sampler_create_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    sampler_create_info.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    sampler_create_info.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    sampler_create_info.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    sampler_create_info.mipLodBias = 0.0f;
    sampler_create_info.compareOp = VK_COMPARE_OP_LESS;
    sampler_create_info.minLod = 0.0f;
    // Max level-of-detail should match mip level count
    sampler_create_info.maxLod = 1.0f;
    sampler_create_info.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
    sampler_create_info.unnormalizedCoordinates = false;
    cube_sampler_ = std::make_unique<core::Sampler>(render_context_.get_device(), sampler_create_info);
}

void SkyboxSubpass::draw(CommandBuffer &command_buffer) {
    const auto kProjectionMatrix = camera_->projection_matrix();
    auto view_matrix = camera_->view_matrix();
    view_matrix[12] = 0;
    view_matrix[13] = 0;
    view_matrix[14] = 0;
    view_matrix[15] = 1;
    auto matrix = kProjectionMatrix * view_matrix;
    std::vector<uint8_t> bytes = to_bytes(matrix);
    vp_matrix_->update(bytes);
    
    // pipeline state
    command_buffer.set_rasterization_state(rasterization_state_);
    command_buffer.set_multisample_state(multisample_state_);
    command_buffer.set_depth_stencil_state(depth_stencil_state_);
    command_buffer.set_color_blend_state(color_blend_state_);
    command_buffer.set_input_assembly_state(input_assembly_state_);
    
    // shader
    auto &device = render_context_.get_device();
    auto &vert_shader_module = device.get_resource_cache().request_shader_module(VK_SHADER_STAGE_VERTEX_BIT,
                                                                                 vert_shader_, variant_);
    auto &frag_shader_module = device.get_resource_cache().request_shader_module(VK_SHADER_STAGE_FRAGMENT_BIT,
                                                                                 frag_shader_, variant_);
    std::vector<ShaderModule *> shader_modules{&vert_shader_module, &frag_shader_module};
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
