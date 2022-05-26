//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.geometry/point_cloud.h"
#include "vox.geometry/triangle_mesh.h"
#include "vox.math/vector3.h"
#include "vox.render/script.h"
#include "vox.visual/graphics/render_option.h"

namespace vox::visualization {
class SimpleBlackShader : public Script {
public:
    bool BindPointCloudNormal(const geometry::PointCloud &pointcloud, const RenderOption &option);

    bool BindTriangleMeshWireFrame(const geometry::TriangleMesh &mesh, const RenderOption &option);

public:
    void OnEnable() override;

    void OnUpdate(float delta_time) override;

private:
    std::vector<Vector3F> points_{};
    MeshRenderer *renderer_{nullptr};
    bool is_dirty_ = false;
};
}  // namespace vox::visualization