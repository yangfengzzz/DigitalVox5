//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.render/material/base_material.h"
#include "vox.render/renderer.h"

namespace vox::compute {
class SdfMarchingCube;

class SdfMarchingCubeMaterial : public BaseMaterial {
public:
    explicit SdfMarchingCubeMaterial(Device &device);
};

class SdfMarchingCubeLineMaterial : public BaseMaterial {
public:
    explicit SdfMarchingCubeLineMaterial(Device &device);
};

class SdfMarchingCubeRenderer : public Renderer {
public:
    explicit SdfMarchingCubeRenderer(Entity *entity);

    void LineMode(bool flag);

    void Render(std::vector<RenderElement> &opaque_queue,
                std::vector<RenderElement> &alpha_test_queue,
                std::vector<RenderElement> &transparent_queue) override;

    void UpdateBounds(BoundingBox3F &world_bounds) override;

private:
    SdfMarchingCube *sdf_mc_{nullptr};
    struct VertexData {
        Vector4F position;
        Vector4F normal;
    };
    std::shared_ptr<SdfMarchingCubeMaterial> material_{nullptr};
    std::shared_ptr<SdfMarchingCubeLineMaterial> line_material_{nullptr};
    bool is_line_mode_{false};

    void OnEnable() override;

    void OnDisable() override;
};

}  // namespace vox::compute