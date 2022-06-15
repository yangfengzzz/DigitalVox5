//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.render/renderer.h"

namespace vox::compute {
class SdfMarchingCubeRenderer : public Renderer {
public:
    void Render(std::vector<RenderElement> &opaque_queue,
                std::vector<RenderElement> &alpha_test_queue,
                std::vector<RenderElement> &transparent_queue) override;

    void UpdateBounds(BoundingBox3F &world_bounds) override;

private:
    struct VertexData {
        Vector4F position;
        Vector4F normal;
    };

    void OnEnable() override;

    void OnDisable() override;
};

}  // namespace vox::compute