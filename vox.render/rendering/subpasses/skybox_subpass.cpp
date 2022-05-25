//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/rendering/subpasses/skybox_subpass.h"

#include "vox.render/camera.h"
#include "vox.render/mesh/mesh_manager.h"

namespace vox {
SkyboxSubpass::SkyboxSubpass(RenderContext &render_context, Scene *scene, Camera *camera)
    : Subpass{render_context, scene, camera}, vert_shader_("base/skybox.vert"), frag_shader_("base/skybox.frag") {}

void SkyboxSubpass::CreateCuboid() {
    mesh_ = MeshManager::GetSingleton().LoadModelMesh();

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
    bounds.lower_corner = Point3F(-kHalfLength, -kHalfLength, -kHalfLength);
    bounds.upper_corner = Point3F(kHalfLength, kHalfLength, kHalfLength);

    mesh_->SetPositions(positions);
    mesh_->SetIndices(indices);
    mesh_->UploadData(true);
    mesh_->AddSubMesh(0, static_cast<uint32_t>(indices.size()));
}

const std::shared_ptr<Texture> &SkyboxSubpass::TextureCubeMap() const { return cube_map_; }

void SkyboxSubpass::SetTextureCubeMap(const std::shared_ptr<Texture> &v) { cube_map_ = v; }

void SkyboxSubpass::FlipVertically() { is_flip_vertically_ = true; }

void SkyboxSubpass::Prepare() {
    auto &device = render_context_.GetDevice();
    vp_matrix_ = std::make_unique<core::Buffer>(device, sizeof(Matrix4x4F), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                                                VMA_MEMORY_USAGE_CPU_TO_GPU);
    if (is_flip_vertically_) {
        variant_.AddDefine("NEED_FLIP_Y");
    }
    device.GetResourceCache().RequestShaderModule(VK_SHADER_STAGE_VERTEX_BIT, vert_shader_, variant_);
    device.GetResourceCache().RequestShaderModule(VK_SHADER_STAGE_FRAGMENT_BIT, frag_shader_, variant_);

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
    cube_sampler_ = std::make_unique<core::Sampler>(render_context_.GetDevice(), sampler_create_info);
}

void SkyboxSubpass::Draw(CommandBuffer &command_buffer) {
    const auto kProjectionMatrix = camera_->ProjectionMatrix();
    auto view_matrix = camera_->ViewMatrix();
    view_matrix[12] = 0;
    view_matrix[13] = 0;
    view_matrix[14] = 0;
    view_matrix[15] = 1;
    auto matrix = kProjectionMatrix * view_matrix;
    std::vector<uint8_t> bytes = utility::ToBytes(matrix);
    vp_matrix_->Update(bytes);

    // pipeline state
    command_buffer.SetRasterizationState(rasterization_state_);
    command_buffer.SetMultisampleState(multisample_state_);
    command_buffer.SetDepthStencilState(depth_stencil_state_);
    command_buffer.SetColorBlendState(color_blend_state_);
    command_buffer.SetInputAssemblyState(input_assembly_state_);

    // shader
    auto &device = render_context_.GetDevice();
    auto &vert_shader_module =
            device.GetResourceCache().RequestShaderModule(VK_SHADER_STAGE_VERTEX_BIT, vert_shader_, variant_);
    auto &frag_shader_module =
            device.GetResourceCache().RequestShaderModule(VK_SHADER_STAGE_FRAGMENT_BIT, frag_shader_, variant_);
    std::vector<ShaderModule *> shader_modules{&vert_shader_module, &frag_shader_module};
    auto &pipeline_layout = PreparePipelineLayout(command_buffer, shader_modules);
    command_buffer.BindPipelineLayout(pipeline_layout);

    // uniform & texture
    command_buffer.BindBuffer(*vp_matrix_, 0, vp_matrix_->GetSize(), 0, 10, 0);
    command_buffer.BindImage(cube_map_->GetVkImageView(VK_IMAGE_VIEW_TYPE_CUBE), *cube_sampler_, 0, 11, 0);

    // vertex buffer
    command_buffer.SetVertexInputState(mesh_->VertexInputState());
    for (uint32_t j = 0; j < mesh_->VertexBufferBindings().size(); j++) {
        const auto &vertex_buffer_binding = mesh_->VertexBufferBindings()[j];
        if (vertex_buffer_binding) {
            std::vector<std::reference_wrapper<const core::Buffer>> buffers;
            buffers.emplace_back(std::ref(*vertex_buffer_binding));
            command_buffer.BindVertexBuffers(j, buffers, {0});
        }
    }
    // Draw submesh indexed if indices exists
    const auto &index_buffer_binding = mesh_->IndexBufferBinding();
    // Bind index buffer of submesh
    command_buffer.BindIndexBuffer(index_buffer_binding->Buffer(), 0, index_buffer_binding->IndexType());

    // Draw submesh using indexed data
    command_buffer.DrawIndexed(mesh_->FirstSubMesh()->Count(), mesh_->InstanceCount(), mesh_->FirstSubMesh()->Start(),
                               0, 0);
}

}  // namespace vox
