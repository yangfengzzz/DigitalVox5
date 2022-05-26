//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.geometry/bounding_volume.h"
#include "vox.geometry/line_set.h"
#include "vox.geometry/octree.h"
#include "vox.geometry/point_cloud.h"
#include "vox.geometry/tetra_mesh.h"
#include "vox.geometry/triangle_mesh.h"
#include "vox.math/color.h"
#include "vox.math/vector3.h"
#include "vox.render/script.h"
#include "vox.visual/graphics/render_option.h"

namespace vox::visualization {

class SimpleShader : public Script {
public:
    bool BindLineSet(const geometry::LineSet &lineset, const RenderOption &option);

    bool BindOrientedBoundingBox(const geometry::OrientedBoundingBox &bound, const RenderOption &option);

    bool BindAxisAlignedBoundingBox(const geometry::AxisAlignedBoundingBox &bound, const RenderOption &option);

    bool BindTetraMesh(const geometry::TetraMesh &tetra_mesh, const RenderOption &option);

    bool BindTriangleMesh(const geometry::TriangleMesh &mesh, const RenderOption &option);

    bool BindPointCloud(const geometry::PointCloud &pointcloud, const RenderOption &option);

    bool BindVoxelGridLine(const geometry::VoxelGrid &grid, const RenderOption &option);

    bool BindVoxelGridFace(const geometry::VoxelGrid &grid, const RenderOption &option);

    bool BindVoxelOctreeLine(const geometry::Octree &octree, const RenderOption &option);

    bool BindVoxelOctreeFace(const geometry::Octree &octree, const RenderOption &option);

private:
    std::vector<Vector3F> points_;
    std::vector<Color> colors_;
};

}  // namespace vox::visualization