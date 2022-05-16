//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <string>

namespace arc::geometry {

/// \class Geometry
///
/// \brief The base geometry class.
class Geometry {
public:
    /// \enum GeometryType
    ///
    /// \brief Specifies possible geometry types.
    enum class GeometryType {
        /// Unspecified geometry type.
        UNSPECIFIED = 0,
        /// PointCloud
        POINT_CLOUD = 1,
        /// VoxelGrid
        VOXEL_GRID = 2,
        /// Octree
        OCTREE = 3,
        /// LineSet
        LINE_SET = 4,
        /// MeshBase
        MESH_BASE = 5,
        /// TriangleMesh
        TRIANGLE_MESH = 6,
        /// HalfEdgeTriangleMesh
        HALF_EDGE_TRIANGLE_MESH = 7,
        /// Image
        IMAGE = 8,
        /// RGBDImage
        RGBD_IMAGE = 9,
        /// TetraMesh
        TETRA_MESH = 10,
        /// OrientedBoundingBox
        ORIENTED_BOUNDING_BOX = 11,
        /// AxisAlignedBoundingBox
        AXIS_ALIGNED_BOUNDING_BOX = 12,
    };

public:
    virtual ~Geometry() = default;

protected:
    /// \brief Parameterized Constructor.
    ///
    /// \param type Specifies the type of geometry of the object constructed.
    /// \param dimension Specifies whether the dimension is 2D or 3D.
    Geometry(GeometryType type, int dimension)
        : geometry_type_(type), dimension_(dimension) {}

public:
    /// Clear all elements in the geometry.
    virtual Geometry& Clear() = 0;

    /// Returns `true` iff the geometry is empty.
    [[nodiscard]] virtual bool IsEmpty() const = 0;

    /// Returns one of registered geometry types.
    [[nodiscard]] GeometryType GetGeometryType() const { return geometry_type_; }

    /// Returns whether the geometry is 2D or 3D.
    [[nodiscard]] int Dimension() const { return dimension_; }

    [[nodiscard]] std::string GetName() const { return name_; }

    void SetName(const std::string& name) { name_ = name; }

private:
    /// Type of geometry from GeometryType.
    GeometryType geometry_type_ = GeometryType::UNSPECIFIED;
    /// Number of dimensions of the geometry.
    int dimension_ = 3;
    std::string name_;
};

}  // namespace arc
