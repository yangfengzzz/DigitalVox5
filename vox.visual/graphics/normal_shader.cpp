//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "normal_shader.h"

#include "vox.base/logging.h"
#include "vox.render/entity.h"
#include "vox.render/material/base_material.h"
#include "vox.render/mesh/mesh_manager.h"
#include "vox.render/mesh/mesh_renderer.h"
#include "vox.render/scene.h"
#include "vox.render/shader/shader_manager.h"

namespace vox::visualization {
bool NormalShader::BindPointCloud(const geometry::PointCloud &pointcloud, const RenderOption &option) {
    if (!pointcloud.HasPoints()) {
        LOGW("Binding failed with empty pointcloud.")
        return false;
    }
    if (!pointcloud.HasNormals()) {
        LOGW("Binding failed with pointcloud with no normals.")
        return false;
    }
    points_.resize(pointcloud.points_.size());
    normals_.resize(pointcloud.points_.size());
    is_dirty_ = true;
    for (size_t i = 0; i < pointcloud.points_.size(); i++) {
        const auto &point = pointcloud.points_[i].cast<float>();
        const auto &normal = pointcloud.normals_[i].cast<float>();
        points_[i] = Vector3F(point.x(), point.y(), point.z());
        normals_[i] = Vector3F(normal.x(), normal.y(), normal.z());
    }

    return true;
}

bool NormalShader::BindTriangleMesh(const geometry::TriangleMesh &mesh, const RenderOption &option) {
    if (!mesh.HasTriangles()) {
        LOGW("Binding failed with empty triangle mesh.")
        return false;
    }
    if (!mesh.HasTriangleNormals() || !mesh.HasVertexNormals()) {
        LOGW("Binding failed because mesh has no normals.")
        LOGW("Call ComputeVertexNormals() before binding.")
        return false;
    }
    points_.resize(mesh.triangles_.size() * 3);
    normals_.resize(mesh.triangles_.size() * 3);
    is_dirty_ = true;
    for (size_t i = 0; i < mesh.triangles_.size(); i++) {
        const auto &triangle = mesh.triangles_[i];
        for (size_t j = 0; j < 3; j++) {
            size_t idx = i * 3 + j;
            size_t vi = triangle(j);
            const auto &vertex = mesh.vertices_[vi].cast<float>();
            points_[idx] = Vector3F(vertex.x(), vertex.y(), vertex.z());
            if (option.mesh_shade_option_ == RenderOption::MeshShadeOption::FlatShade) {
                auto normal = mesh.triangle_normals_[i].cast<float>();
                normals_[idx] = Vector3F(normal.x(), normal.y(), normal.z());
            } else {
                auto normal = mesh.vertex_normals_[vi].cast<float>();
                normals_[idx] = Vector3F(normal.x(), normal.y(), normal.z());
            }
        }
    }

    return true;
}

void NormalShader::OnEnable() {
    renderer_ = entity_->AddComponent<MeshRenderer>();
    auto material = std::make_shared<BaseMaterial>(entity_->Scene()->Device());
    material->vertex_source_ = ShaderManager::GetSingleton().LoadShader("");
    material->fragment_source_ = ShaderManager::GetSingleton().LoadShader("");
    renderer_->SetMaterial(material);
}

void NormalShader::OnUpdate(float delta_time) {
    if (is_dirty_) {
        auto mesh = MeshManager::GetSingleton().LoadModelMesh();
        mesh->SetPositions(points_);
        mesh->SetNormals(normals_);
        mesh->UploadData(true);
        mesh->AddSubMesh(0, 0);
        renderer_->SetMesh(mesh);
    }
    is_dirty_ = false;
}

}  // namespace vox::visualization