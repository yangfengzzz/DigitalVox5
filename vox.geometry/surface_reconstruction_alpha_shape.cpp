//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "logging.h"
#include "point_cloud.h"
#include "qhull.h"
#include "tetra_mesh.h"
#include "triangle_mesh.h"

namespace vox::geometry {

std::shared_ptr<TriangleMesh> TriangleMesh::CreateFromPointCloudAlphaShape(const PointCloud& pcd,
                                                                           double alpha,
                                                                           std::shared_ptr<TetraMesh> tetra_mesh,
                                                                           std::vector<size_t>* pt_map) {
    std::vector<size_t> pt_map_computed;
    if (tetra_mesh == nullptr) {
        LOGD("[CreateFromPointCloudAlphaShape] "
             "ComputeDelaunayTetrahedralization")
        std::tie(tetra_mesh, pt_map_computed) = Qhull::ComputeDelaunayTetrahedralization(pcd.points_);
        pt_map = &pt_map_computed;
        LOGD("[CreateFromPointCloudAlphaShape] done "
             "ComputeDelaunayTetrahedralization")
    }

    LOGD("[CreateFromPointCloudAlphaShape] init triangle mesh")
    auto mesh = std::make_shared<TriangleMesh>();
    mesh->vertices_ = tetra_mesh->vertices_;
    if (pcd.HasNormals()) {
        mesh->vertex_normals_.resize(mesh->vertices_.size());
        for (size_t idx = 0; idx < (*pt_map).size(); ++idx) {
            mesh->vertex_normals_[idx] = pcd.normals_[(*pt_map)[idx]];
        }
    }
    if (pcd.HasColors()) {
        mesh->vertex_colors_.resize(mesh->vertices_.size());
        for (size_t idx = 0; idx < (*pt_map).size(); ++idx) {
            mesh->vertex_colors_[idx] = pcd.colors_[(*pt_map)[idx]];
        }
    }
    LOGD("[CreateFromPointCloudAlphaShape] done init triangle mesh")

    std::vector<double> vsqn(tetra_mesh->vertices_.size());
    for (size_t vidx = 0; vidx < vsqn.size(); ++vidx) {
        vsqn[vidx] = tetra_mesh->vertices_[vidx].squaredNorm();
    }

    LOGD("[CreateFromPointCloudAlphaShape] add triangles from tetras that "
         "satisfy constraint")
    const auto& verts = tetra_mesh->vertices_;
    for (auto& tetra : tetra_mesh->tetras_) {
        // clang-format off
        Eigen::Matrix4d tmp;
        tmp << verts[tetra(0)](0), verts[tetra(0)](1), verts[tetra(0)](2), 1,
                verts[tetra(1)](0), verts[tetra(1)](1), verts[tetra(1)](2), 1,
                verts[tetra(2)](0), verts[tetra(2)](1), verts[tetra(2)](2), 1,
                verts[tetra(3)](0), verts[tetra(3)](1), verts[tetra(3)](2), 1;
        double a = tmp.determinant();
        tmp << vsqn[tetra(0)], verts[tetra(0)](0), verts[tetra(0)](1), verts[tetra(0)](2),
                vsqn[tetra(1)], verts[tetra(1)](0), verts[tetra(1)](1), verts[tetra(1)](2),
                vsqn[tetra(2)], verts[tetra(2)](0), verts[tetra(2)](1), verts[tetra(2)](2),
                vsqn[tetra(3)], verts[tetra(3)](0), verts[tetra(3)](1), verts[tetra(3)](2);
        double c = tmp.determinant();
        tmp << vsqn[tetra(0)], verts[tetra(0)](1), verts[tetra(0)](2), 1,
                vsqn[tetra(1)], verts[tetra(1)](1), verts[tetra(1)](2), 1,
                vsqn[tetra(2)], verts[tetra(2)](1), verts[tetra(2)](2), 1,
                vsqn[tetra(3)], verts[tetra(3)](1), verts[tetra(3)](2), 1;
        double dx = tmp.determinant();
        tmp << vsqn[tetra(0)], verts[tetra(0)](0), verts[tetra(0)](2), 1,
                vsqn[tetra(1)], verts[tetra(1)](0), verts[tetra(1)](2), 1,
                vsqn[tetra(2)], verts[tetra(2)](0), verts[tetra(2)](2), 1,
                vsqn[tetra(3)], verts[tetra(3)](0), verts[tetra(3)](2), 1;
        double dy = tmp.determinant();
        tmp << vsqn[tetra(0)], verts[tetra(0)](0), verts[tetra(0)](1), 1,
                vsqn[tetra(1)], verts[tetra(1)](0), verts[tetra(1)](1), 1,
                vsqn[tetra(2)], verts[tetra(2)](0), verts[tetra(2)](1), 1,
                vsqn[tetra(3)], verts[tetra(3)](0), verts[tetra(3)](1), 1;
        double dz = tmp.determinant();
        // clang-format on
        if (a == 0) {
            LOGW("[CreateFromPointCloudAlphaShape] invalid tetra in "
                 "TetraMesh")
        } else {
            double r = std::sqrt(dx * dx + dy * dy + dz * dz - 4 * a * c) / (2 * std::abs(a));

            if (r <= alpha) {
                mesh->triangles_.push_back(TriangleMesh::GetOrderedTriangle(tetra(0), tetra(1), tetra(2)));
                mesh->triangles_.push_back(TriangleMesh::GetOrderedTriangle(tetra(0), tetra(1), tetra(3)));
                mesh->triangles_.push_back(TriangleMesh::GetOrderedTriangle(tetra(0), tetra(2), tetra(3)));
                mesh->triangles_.push_back(TriangleMesh::GetOrderedTriangle(tetra(1), tetra(2), tetra(3)));
            }
        }
    }
    LOGD("[CreateFromPointCloudAlphaShape] done add triangles from tetras "
         "that satisfy constraint")

    LOGD("[CreateFromPointCloudAlphaShape] remove triangles within "
         "the mesh")
    std::unordered_map<Eigen::Vector3i, int, utility::hash_eigen<Eigen::Vector3i>> triangle_count;
    for (const auto& triangle : mesh->triangles_) {
        if (triangle_count.count(triangle) == 0) {
            triangle_count[triangle] = 1;
        } else {
            triangle_count[triangle] += 1;
        }
    }

    size_t to_idx = 0;
    for (size_t tidx = 0; tidx < mesh->triangles_.size(); ++tidx) {
        Eigen::Vector3i triangle = mesh->triangles_[tidx];
        if (triangle_count[triangle] == 1) {
            mesh->triangles_[to_idx] = triangle;
            to_idx++;
        }
    }
    mesh->triangles_.resize(to_idx);
    LOGD("[CreateFromPointCloudAlphaShape] done remove triangles within "
         "the mesh")

    LOGD("[CreateFromPointCloudAlphaShape] remove duplicate triangles and "
         "unreferenced vertices")
    mesh->RemoveDuplicatedTriangles();
    mesh->RemoveUnreferencedVertices();
    LOGD("[CreateFromPointCloudAlphaShape] done remove duplicate triangles "
         "and unreferenced vertices")

    return mesh;
}

}  // namespace vox::geometry
