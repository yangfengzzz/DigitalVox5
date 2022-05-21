//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "mesh_base.h"

#include <numeric>
#include <queue>
#include <tuple>

#include "bounding_volume.h"
#include "kdtree_flann.h"
#include "point_cloud.h"
#include "qhull.h"
#include "triangle_mesh.h"

namespace vox::geometry {

MeshBase &MeshBase::Clear() {
    vertices_.clear();
    vertex_normals_.clear();
    vertex_colors_.clear();
    return *this;
}

bool MeshBase::IsEmpty() const { return !HasVertices(); }

Eigen::Vector3d MeshBase::GetMinBound() const { return ComputeMinBound(vertices_); }

Eigen::Vector3d MeshBase::GetMaxBound() const { return ComputeMaxBound(vertices_); }

Eigen::Vector3d MeshBase::GetCenter() const { return ComputeCenter(vertices_); }

AxisAlignedBoundingBox MeshBase::GetAxisAlignedBoundingBox() const {
    return AxisAlignedBoundingBox::CreateFromPoints(vertices_);
}

OrientedBoundingBox MeshBase::GetOrientedBoundingBox(bool robust) const {
    return OrientedBoundingBox::CreateFromPoints(vertices_, robust);
}

MeshBase &MeshBase::Transform(const Eigen::Matrix4d &transformation) {
    TransformPoints(transformation, vertices_);
    TransformNormals(transformation, vertex_normals_);
    return *this;
}

MeshBase &MeshBase::Translate(const Eigen::Vector3d &translation, bool relative) {
    TranslatePoints(translation, vertices_, relative);
    return *this;
}

MeshBase &MeshBase::Scale(const double scale, const Eigen::Vector3d &center) {
    ScalePoints(scale, vertices_, center);
    return *this;
}

MeshBase &MeshBase::Rotate(const Eigen::Matrix3d &R, const Eigen::Vector3d &center) {
    RotatePoints(R, vertices_, center);
    RotateNormals(R, vertex_normals_);
    return *this;
}

MeshBase &MeshBase::operator+=(const MeshBase &mesh) {
    if (mesh.IsEmpty()) return (*this);
    size_t old_vert_num = vertices_.size();
    size_t add_vert_num = mesh.vertices_.size();
    size_t new_vert_num = old_vert_num + add_vert_num;
    if ((!HasVertices() || HasVertexNormals()) && mesh.HasVertexNormals()) {
        vertex_normals_.resize(new_vert_num);
        for (size_t i = 0; i < add_vert_num; i++) vertex_normals_[old_vert_num + i] = mesh.vertex_normals_[i];
    } else {
        vertex_normals_.clear();
    }
    if ((!HasVertices() || HasVertexColors()) && mesh.HasVertexColors()) {
        vertex_colors_.resize(new_vert_num);
        for (size_t i = 0; i < add_vert_num; i++) vertex_colors_[old_vert_num + i] = mesh.vertex_colors_[i];
    } else {
        vertex_colors_.clear();
    }
    vertices_.resize(new_vert_num);
    for (size_t i = 0; i < add_vert_num; i++) vertices_[old_vert_num + i] = mesh.vertices_[i];
    return (*this);
}

MeshBase MeshBase::operator+(const MeshBase &mesh) const { return (MeshBase(*this) += mesh); }

std::tuple<std::shared_ptr<TriangleMesh>, std::vector<size_t>> MeshBase::ComputeConvexHull() const {
    return Qhull::ComputeConvexHull(vertices_);
}

}  // namespace vox::geometry
