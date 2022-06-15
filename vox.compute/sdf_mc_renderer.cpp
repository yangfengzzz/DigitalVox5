//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.compute/sdf_mc_renderer.h"

#include "vox.compute/sdf_mc.h"
#include "vox.render/entity.h"
#include "vox.render/scene.h"
#include "vox.render/shader/shader_manager.h"

namespace vox::compute {
SdfMarchingCubeMaterial::SdfMarchingCubeMaterial(Device &device) : BaseMaterial(device, "mc") {
    vertex_source_ = ShaderManager::GetSingleton().LoadShader("");
    fragment_source_ = ShaderManager::GetSingleton().LoadShader("");
}

SdfMarchingCubeLineMaterial::SdfMarchingCubeLineMaterial(Device &device) : BaseMaterial(device, "mc") {
    vertex_source_ = ShaderManager::GetSingleton().LoadShader("");
    fragment_source_ = ShaderManager::GetSingleton().LoadShader("");
}

SdfMarchingCubeRenderer::SdfMarchingCubeRenderer(Entity *entity) : Renderer(entity) {
    material_ = std::make_shared<SdfMarchingCubeMaterial>(entity_->Scene()->Device());
}

void SdfMarchingCubeRenderer::LineMode(bool flag) {
    is_line_mode_ = flag;
    if (is_line_mode_) {
        line_material_ = std::make_shared<SdfMarchingCubeLineMaterial>(entity_->Scene()->Device());
    }
}

void SdfMarchingCubeRenderer::Render(std::vector<RenderElement> &opaque_queue,
                                     std::vector<RenderElement> &alpha_test_queue,
                                     std::vector<RenderElement> &transparent_queue) {}

void SdfMarchingCubeRenderer::UpdateBounds(BoundingBox3F &world_bounds) {}

void SdfMarchingCubeRenderer::OnEnable() {}

void SdfMarchingCubeRenderer::OnDisable() {}

}  // namespace vox::compute