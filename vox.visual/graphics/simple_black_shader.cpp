//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "simple_black_shader.h"

#include "vox.geometry/bounding_volume.h"
#include "vox.render/entity.h"
#include "vox.render/material/base_material.h"
#include "vox.render/mesh/mesh_manager.h"
#include "vox.render/mesh/mesh_renderer.h"
#include "vox.render/scene.h"
#include "vox.render/shader/shader_manager.h"

namespace vox::visualization {
bool SimpleBlackShader::BindPointCloudNormal(const geometry::PointCloud &pointcloud, const RenderOption &option) {
    if (!pointcloud.HasPoints()) {
        LOGW("Binding failed with empty pointcloud.")
        return false;
    }
    points_.resize(pointcloud.points_.size() * 2);
    double line_length = option.point_size_ * 0.01 * pointcloud.GetAxisAlignedBoundingBox().GetMaxExtent();
    for (size_t i = 0; i < pointcloud.points_.size(); i++) {
        const auto &point_first = pointcloud.points_[i].cast<float>();
        const auto &point_second = (pointcloud.points_[i] + pointcloud.normals_[i] * line_length).cast<float>();
        points_[i * 2] = Vector3F(point_first.x(), point_first.y(), point_first.z());
        points_[i * 2 + 1] = Vector3F(point_second.x(), point_second.y(), point_second.z());
    }
    return true;
}

bool SimpleBlackShader::BindTriangleMeshWireFrame(const geometry::TriangleMesh &mesh, const RenderOption &option) {
    if (!mesh.HasTriangles()) {
        LOGW("Binding failed with empty geometry::TriangleMesh.")
        return false;
    }
    points_.resize(mesh.triangles_.size() * 3);
    is_dirty_ = true;
    for (size_t i = 0; i < mesh.triangles_.size(); i++) {
        const auto &triangle = mesh.triangles_[i];
        for (size_t j = 0; j < 3; j++) {
            size_t idx = i * 3 + j;
            size_t vi = triangle(j);
            const auto &vertex = mesh.vertices_[vi].cast<float>();
            points_[idx] = Vector3F(vertex.x(), vertex.y(), vertex.z());
        }
    }
    return true;
}

void SimpleBlackShader::OnEnable() {
    renderer_ = entity_->AddComponent<MeshRenderer>();
    auto material = std::make_shared<BaseMaterial>(entity_->Scene()->Device());
    material->vertex_source_ = ShaderManager::GetSingleton().LoadShader("");
    material->fragment_source_ = ShaderManager::GetSingleton().LoadShader("");
    renderer_->SetMaterial(material);
}

void SimpleBlackShader::OnUpdate(float delta_time) {
    if (is_dirty_) {
        auto mesh = MeshManager::GetSingleton().LoadModelMesh();
        mesh->SetPositions(points_);
        mesh->UploadData(true);
        mesh->AddSubMesh(0, 0);
        renderer_->SetMesh(mesh);
    }
    is_dirty_ = false;
}

}  // namespace vox::visualization