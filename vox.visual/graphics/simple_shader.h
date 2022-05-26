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
#include "vox.geometry/voxel_grid.h"
#include "vox.math/color.h"
#include "vox.math/vector3.h"
#include "vox.render/script.h"
#include "vox.visual/graphics/render_option.h"

namespace vox::visualization {
/**
 * Simple render for geometry
 */
class SimpleShader : public Script {
public:
    /// Draw line set
    /// \param lineset geometry
    /// \param option render option
    /// \return
    bool BindLineSet(const geometry::LineSet &lineset, const RenderOption &option);

    /// Draw oriented boundingBox
    /// \param bound geometry
    /// \param option render option
    /// \return
    bool BindOrientedBoundingBox(const geometry::OrientedBoundingBox &bound, const RenderOption &option);

    /// Draw axis-aligned boundingBox
    /// \param bound geometry
    /// \param option render option
    /// \return
    bool BindAxisAlignedBoundingBox(const geometry::AxisAlignedBoundingBox &bound, const RenderOption &option);

    /// Draw tetra mesh
    /// \param tetra_mesh geometry
    /// \param option render option
    /// \return
    bool BindTetraMesh(const geometry::TetraMesh &tetra_mesh, const RenderOption &option);

    /// Draw triangle mesh
    /// \param mesh geometry
    /// \param option render option
    /// \return
    bool BindTriangleMesh(const geometry::TriangleMesh &mesh, const RenderOption &option);

    /// Draw point cloud
    /// \param pointcloud geometry
    /// \param option render option
    /// \return
    bool BindPointCloud(const geometry::PointCloud &pointcloud, const RenderOption &option);

    /// Draw voxel grid line
    /// \param grid geometry
    /// \param option render option
    /// \return
    bool BindVoxelGridLine(const geometry::VoxelGrid &grid, const RenderOption &option);

    /// Draw voxel grid face
    /// \param grid geometry
    /// \param option render option
    /// \return
    bool BindVoxelGridFace(const geometry::VoxelGrid &grid, const RenderOption &option);

    /// Draw octree line
    /// \param octree geometry
    /// \param option render option
    /// \return
    bool BindVoxelOctreeLine(const geometry::Octree &octree, const RenderOption &option);

    /// Draw octree face
    /// \param octree geometry
    /// \param option render option
    /// \return
    bool BindVoxelOctreeFace(const geometry::Octree &octree, const RenderOption &option);

private:
    std::vector<Vector3F> points_;
    std::vector<Color> colors_;
};

}  // namespace vox::visualization