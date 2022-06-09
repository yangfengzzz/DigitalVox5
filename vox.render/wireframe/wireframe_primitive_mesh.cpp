//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/wireframe/wireframe_primitive_mesh.h"

#include "vox.render/mesh/mesh_manager.h"

namespace vox {
ModelMeshPtr WireframePrimitiveMesh::CreateCuboidWireFrame(float width, float height, float depth) {
    auto mesh = MeshManager::GetSingleton().LoadModelMesh();

    auto half_width = width / 2;
    auto half_height = height / 2;
    auto half_depth = depth / 2;

    auto positions = std::vector<Vector3F>(24);
    // Up
    positions[0] = Vector3F(-half_width, half_height, -half_depth);
    positions[1] = Vector3F(half_width, half_height, -half_depth);
    positions[2] = Vector3F(half_width, half_height, half_depth);
    positions[3] = Vector3F(-half_width, half_height, half_depth);

    // Down
    positions[4] = Vector3F(-half_width, -half_height, -half_depth);
    positions[5] = Vector3F(half_width, -half_height, -half_depth);
    positions[6] = Vector3F(half_width, -half_height, half_depth);
    positions[7] = Vector3F(-half_width, -half_height, half_depth);

    // Left
    positions[8] = Vector3F(-half_width, half_height, -half_depth);
    positions[9] = Vector3F(-half_width, half_height, half_depth);
    positions[10] = Vector3F(-half_width, -half_height, half_depth);
    positions[11] = Vector3F(-half_width, -half_height, -half_depth);

    // Right
    positions[12] = Vector3F(half_width, half_height, -half_depth);
    positions[13] = Vector3F(half_width, half_height, half_depth);
    positions[14] = Vector3F(half_width, -half_height, half_depth);
    positions[15] = Vector3F(half_width, -half_height, -half_depth);

    // Front
    positions[16] = Vector3F(-half_width, half_height, half_depth);
    positions[17] = Vector3F(half_width, half_height, half_depth);
    positions[18] = Vector3F(half_width, -half_height, half_depth);
    positions[19] = Vector3F(-half_width, -half_height, half_depth);

    // Back
    positions[20] = Vector3F(-half_width, half_height, -half_depth);
    positions[21] = Vector3F(half_width, half_height, -half_depth);
    positions[22] = Vector3F(half_width, -half_height, -half_depth);
    positions[23] = Vector3F(-half_width, -half_height, -half_depth);

    auto indices = std::vector<uint32_t>(48);
    // Up
    indices[0] = 0;
    indices[1] = 1;
    indices[2] = 1;
    indices[3] = 2;
    indices[4] = 2;
    indices[5] = 3;
    indices[6] = 3;
    indices[7] = 0;

    // Down
    indices[8] = 4;
    indices[9] = 5;
    indices[10] = 5;
    indices[11] = 6;
    indices[12] = 6;
    indices[13] = 7;
    indices[14] = 7;
    indices[15] = 4;
    // Left
    indices[16] = 8;
    indices[17] = 9;
    indices[18] = 9;
    indices[19] = 10;
    indices[20] = 10;
    indices[21] = 11;
    indices[22] = 11;
    indices[23] = 8;
    // Right
    indices[24] = 12;
    indices[25] = 13;
    indices[26] = 13;
    indices[27] = 14;
    indices[28] = 14;
    indices[29] = 15;
    indices[30] = 15;
    indices[31] = 12;
    // Front
    indices[32] = 16;
    indices[33] = 17;
    indices[34] = 17;
    indices[35] = 18;
    indices[36] = 18;
    indices[37] = 19;
    indices[38] = 19;
    indices[39] = 16;
    // Back
    indices[40] = 20;
    indices[41] = 21;
    indices[42] = 21;
    indices[43] = 22;
    indices[44] = 22;
    indices[45] = 23;
    indices[46] = 23;
    indices[47] = 20;

    mesh->SetPositions(positions);
    mesh->SetIndices(indices);

    mesh->UploadData(true);
    mesh->AddSubMesh(0, static_cast<uint32_t>(indices.size()));
    return mesh;
}

ModelMeshPtr WireframePrimitiveMesh::CreateSphereWireFrame(float radius) {
    auto mesh = MeshManager::GetSingleton().LoadModelMesh();

    auto vertex_count = 40;
    auto shift = Vector3F();

    auto positions = std::vector<Vector3F>(vertex_count * 3);
    auto indices = std::vector<uint32_t>(vertex_count * 6);
    // X
    CreateCircleWireFrame(radius, 0, vertex_count, 0, shift, positions, indices);

    // Y
    CreateCircleWireFrame(radius, vertex_count, vertex_count, 1, shift, positions, indices);

    // Z
    CreateCircleWireFrame(radius, 2 * vertex_count, vertex_count, 2, shift, positions, indices);

    mesh->SetPositions(positions);
    mesh->SetIndices(indices);

    mesh->UploadData(true);
    mesh->AddSubMesh(0, static_cast<uint32_t>(indices.size()));
    return mesh;
}

ModelMeshPtr WireframePrimitiveMesh::CreateCapsuleWireFrame(float radius, float height) {
    auto mesh = MeshManager::GetSingleton().LoadModelMesh();

    auto vertex_count = 40;
    auto shift = Vector3F();
    auto half_height = height / 2;
    auto positions = std::vector<Vector3F>(vertex_count * 4);
    auto indices = std::vector<uint32_t>(vertex_count * 8);

    // Y-Top
    shift.y = half_height;
    CreateCircleWireFrame(radius, 0, vertex_count, 1, shift, positions, indices);

    // Y-Bottom
    shift.y = -half_height;
    CreateCircleWireFrame(radius, vertex_count, vertex_count, 1, shift, positions, indices);

    // X-Elliptic
    CreateEllipticWireFrame(radius, half_height, vertex_count * 2, vertex_count, 2, positions, indices);

    // Z-Elliptic
    CreateEllipticWireFrame(radius, half_height, vertex_count * 3, vertex_count, 0, positions, indices);

    mesh->SetPositions(positions);
    mesh->SetIndices(indices);

    mesh->UploadData(true);
    mesh->AddSubMesh(0, static_cast<uint32_t>(indices.size()));
    return mesh;
}

void WireframePrimitiveMesh::CreateCircleWireFrame(float radius,
                                                   uint32_t vertex_begin,
                                                   uint32_t vertex_count,
                                                   uint8_t axis,
                                                   const Vector3F &shift,
                                                   std::vector<Vector3F> &positions,
                                                   std::vector<uint32_t> &indices) {
    auto count_reciprocal = 1.f / static_cast<float>(vertex_count);
    for (uint32_t i = 0; i < vertex_count; ++i) {
        float v = static_cast<float>(i) * count_reciprocal;
        float theta_delta = v * static_cast<float>(M_PI * 2);

        uint32_t global_index = i + vertex_begin;
        switch (axis) {
            case 0:
                positions[global_index] = Vector3F(shift.x, radius * std::cos(theta_delta) + shift.y,
                                                   radius * std::sin(theta_delta) + shift.z);
                break;
            case 1:
                positions[global_index] = Vector3F(radius * std::cos(theta_delta) + shift.x, shift.y,
                                                   radius * std::sin(theta_delta) + shift.z);
                break;
            case 2:
                positions[global_index] = Vector3F(radius * std::cos(theta_delta) + shift.x,
                                                   radius * std::sin(theta_delta) + shift.y, shift.z);
                break;
        }

        if (i < vertex_count - 1) {
            indices[2 * global_index] = global_index;
            indices[2 * global_index + 1] = global_index + 1;
        } else {
            indices[2 * global_index] = global_index;
            indices[2 * global_index + 1] = vertex_begin;
        }
    }
}

void WireframePrimitiveMesh::CreateEllipticWireFrame(float radius,
                                                     float height,
                                                     uint32_t vertex_begin,
                                                     uint32_t vertex_count,
                                                     uint8_t axis,
                                                     std::vector<Vector3F> &positions,
                                                     std::vector<uint32_t> &indices) {
    float count_reciprocal = 1.f / static_cast<float>(vertex_count);
    for (uint32_t i = 0; i < vertex_count; ++i) {
        float v = static_cast<float>(i) * count_reciprocal;
        float theta_delta = v * static_cast<float>(M_PI * 2);

        uint32_t global_index = i + vertex_begin;
        switch (axis) {
            case 0:
                positions[global_index] =
                        Vector3F(0, radius * std::sin(theta_delta) + height, radius * std::cos(theta_delta));
                break;
            case 1:
                positions[global_index] =
                        Vector3F(radius * std::cos(theta_delta), height, radius * std::sin(theta_delta));
                break;
            case 2:
                positions[global_index] =
                        Vector3F(radius * std::cos(theta_delta), radius * std::sin(theta_delta) + height, 0);
                break;
        }

        if (i == vertex_count / 2) {
            height = -height;
        }

        if (i < vertex_count - 1) {
            indices[2 * global_index] = global_index;
            indices[2 * global_index + 1] = global_index + 1;
        } else {
            indices[2 * global_index] = global_index;
            indices[2 * global_index + 1] = vertex_begin;
        }
    }
}

}  // namespace vox
