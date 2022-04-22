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
vertex_source_(""),
fragment_source_("") {
    create_cuboid();
    vp_matrix_ = std::make_unique<core::Buffer>(render_context.get_device(), sizeof(Matrix4x4F),
                                                VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                                                VMA_MEMORY_USAGE_CPU_TO_GPU);
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
    
}

void SkyboxSubpass::draw(CommandBuffer &command_buffer) {
    
}

}
