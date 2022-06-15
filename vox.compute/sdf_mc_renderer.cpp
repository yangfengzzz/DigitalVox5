//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.compute/sdf_mc_renderer.h"
#include "vox.compute/sdf_mc_manager.h"

namespace vox::compute {
void SdfMarchingCubeRenderer::Render(std::vector<RenderElement> &opaque_queue,
                                     std::vector<RenderElement> &alpha_test_queue,
                                     std::vector<RenderElement> &transparent_queue) {}

void SdfMarchingCubeRenderer::UpdateBounds(BoundingBox3F &world_bounds) {}

void SdfMarchingCubeRenderer::OnEnable() {

}

void SdfMarchingCubeRenderer::OnDisable() {

}

}  // namespace vox::compute