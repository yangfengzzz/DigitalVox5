//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/mesh/primitive_mesh.h"

#include "vox.render/mesh/mesh_manager.h"

namespace vox {
ModelMeshPtr PrimitiveMesh::CreateSphere(float radius, size_t segments, bool no_longer_accessible) {
    auto mesh = MeshManager::GetSingleton().LoadModelMesh();
    segments = std::max(size_t(2), segments);

    const auto kCount = segments + 1;
    const auto kVertexCount = kCount * kCount;
    const auto kRectangleCount = segments * segments;
    auto indices = std::vector<uint32_t>(kRectangleCount * 6);
    const float kThetaRange = M_PI;
    const auto kAlphaRange = kThetaRange * 2.f;
    const auto kCountReciprocal = 1.f / static_cast<float>(kCount);
    const auto kSegmentsReciprocal = 1.f / static_cast<float>(segments);

    auto positions = std::vector<Vector3F>(kVertexCount);
    auto normals = std::vector<Vector3F>(kVertexCount);
    auto uvs = std::vector<Vector2F>(kVertexCount);

    for (size_t i = 0; i < kVertexCount; ++i) {
        const auto kX = i % kCount;
        const auto kY = size_t(float(i) * kCountReciprocal) | 0;
        const auto kU = static_cast<float>(kX) * kSegmentsReciprocal;
        const auto kV = static_cast<float>(kY) * kSegmentsReciprocal;
        const auto kAlphaDelta = kU * kAlphaRange;
        const auto kThetaDelta = kV * kThetaRange;
        const auto kSinTheta = std::sin(kThetaDelta);

        const auto kPosX = -radius * std::cos(kAlphaDelta) * kSinTheta;
        const auto kPosY = radius * std::cos(kThetaDelta);
        const auto kPosZ = radius * std::sin(kAlphaDelta) * kSinTheta;

        // Position
        positions[i] = Vector3F(kPosX, kPosY, kPosZ);
        // Normal
        normals[i] = Vector3F(kPosX, kPosY, kPosZ);
        // Texcoord
        uvs[i] = Vector2F(kU, kV);
    }

    size_t offset = 0;
    for (size_t i = 0; i < kRectangleCount; ++i) {
        const auto kX = i % segments;
        const auto kY = size_t(float(i) * kSegmentsReciprocal) | 0;

        const auto kA = kY * kCount + kX;
        const auto kB = kA + 1;
        const auto kC = kA + kCount;
        const auto kD = kC + 1;

        indices[offset++] = static_cast<uint32_t>(kB);
        indices[offset++] = static_cast<uint32_t>(kA);
        indices[offset++] = static_cast<uint32_t>(kD);
        indices[offset++] = static_cast<uint32_t>(kA);
        indices[offset++] = static_cast<uint32_t>(kC);
        indices[offset++] = static_cast<uint32_t>(kD);
    }

    auto &bounds = mesh->bounds_;
    bounds.lower_corner = Point3F(-radius, -radius, -radius);
    bounds.upper_corner = Point3F(radius, radius, radius);

    PrimitiveMesh::Initialize(mesh, positions, normals, uvs, indices, no_longer_accessible);
    return mesh;
}

ModelMeshPtr PrimitiveMesh::CreateCuboid(float width, float height, float depth, bool no_longer_accessible) {
    auto mesh = MeshManager::GetSingleton().LoadModelMesh();

    const auto kHalfWidth = width / 2;
    const auto kHalfHeight = height / 2;
    const auto kHalfDepth = depth / 2;

    auto positions = std::vector<Vector3F>(24);
    auto normals = std::vector<Vector3F>(24);
    auto uvs = std::vector<Vector2F>(24);

    // Up
    positions[0] = Vector3F(-kHalfWidth, kHalfHeight, -kHalfDepth);
    positions[1] = Vector3F(kHalfWidth, kHalfHeight, -kHalfDepth);
    positions[2] = Vector3F(kHalfWidth, kHalfHeight, kHalfDepth);
    positions[3] = Vector3F(-kHalfWidth, kHalfHeight, kHalfDepth);
    normals[0] = Vector3F(0, 1, 0);
    normals[1] = Vector3F(0, 1, 0);
    normals[2] = Vector3F(0, 1, 0);
    normals[3] = Vector3F(0, 1, 0);
    uvs[0] = Vector2F(0, 0);
    uvs[1] = Vector2F(1, 0);
    uvs[2] = Vector2F(1, 1);
    uvs[3] = Vector2F(0, 1);
    // Down
    positions[4] = Vector3F(-kHalfWidth, -kHalfHeight, -kHalfDepth);
    positions[5] = Vector3F(kHalfWidth, -kHalfHeight, -kHalfDepth);
    positions[6] = Vector3F(kHalfWidth, -kHalfHeight, kHalfDepth);
    positions[7] = Vector3F(-kHalfWidth, -kHalfHeight, kHalfDepth);
    normals[4] = Vector3F(0, -1, 0);
    normals[5] = Vector3F(0, -1, 0);
    normals[6] = Vector3F(0, -1, 0);
    normals[7] = Vector3F(0, -1, 0);
    uvs[4] = Vector2F(0, 1);
    uvs[5] = Vector2F(1, 1);
    uvs[6] = Vector2F(1, 0);
    uvs[7] = Vector2F(0, 0);
    // Left
    positions[8] = Vector3F(-kHalfWidth, kHalfHeight, -kHalfDepth);
    positions[9] = Vector3F(-kHalfWidth, kHalfHeight, kHalfDepth);
    positions[10] = Vector3F(-kHalfWidth, -kHalfHeight, kHalfDepth);
    positions[11] = Vector3F(-kHalfWidth, -kHalfHeight, -kHalfDepth);
    normals[8] = Vector3F(-1, 0, 0);
    normals[9] = Vector3F(-1, 0, 0);
    normals[10] = Vector3F(-1, 0, 0);
    normals[11] = Vector3F(-1, 0, 0);
    uvs[8] = Vector2F(0, 0);
    uvs[9] = Vector2F(1, 0);
    uvs[10] = Vector2F(1, 1);
    uvs[11] = Vector2F(0, 1);
    // Right
    positions[12] = Vector3F(kHalfWidth, kHalfHeight, -kHalfDepth);
    positions[13] = Vector3F(kHalfWidth, kHalfHeight, kHalfDepth);
    positions[14] = Vector3F(kHalfWidth, -kHalfHeight, kHalfDepth);
    positions[15] = Vector3F(kHalfWidth, -kHalfHeight, -kHalfDepth);
    normals[12] = Vector3F(1, 0, 0);
    normals[13] = Vector3F(1, 0, 0);
    normals[14] = Vector3F(1, 0, 0);
    normals[15] = Vector3F(1, 0, 0);
    uvs[12] = Vector2F(1, 0);
    uvs[13] = Vector2F(0, 0);
    uvs[14] = Vector2F(0, 1);
    uvs[15] = Vector2F(1, 1);
    // Front
    positions[16] = Vector3F(-kHalfWidth, kHalfHeight, kHalfDepth);
    positions[17] = Vector3F(kHalfWidth, kHalfHeight, kHalfDepth);
    positions[18] = Vector3F(kHalfWidth, -kHalfHeight, kHalfDepth);
    positions[19] = Vector3F(-kHalfWidth, -kHalfHeight, kHalfDepth);
    normals[16] = Vector3F(0, 0, 1);
    normals[17] = Vector3F(0, 0, 1);
    normals[18] = Vector3F(0, 0, 1);
    normals[19] = Vector3F(0, 0, 1);
    uvs[16] = Vector2F(0, 0);
    uvs[17] = Vector2F(1, 0);
    uvs[18] = Vector2F(1, 1);
    uvs[19] = Vector2F(0, 1);
    // Back
    positions[20] = Vector3F(-kHalfWidth, kHalfHeight, -kHalfDepth);
    positions[21] = Vector3F(kHalfWidth, kHalfHeight, -kHalfDepth);
    positions[22] = Vector3F(kHalfWidth, -kHalfHeight, -kHalfDepth);
    positions[23] = Vector3F(-kHalfWidth, -kHalfHeight, -kHalfDepth);
    normals[20] = Vector3F(0, 0, -1);
    normals[21] = Vector3F(0, 0, -1);
    normals[22] = Vector3F(0, 0, -1);
    normals[23] = Vector3F(0, 0, -1);
    uvs[20] = Vector2F(1, 0);
    uvs[21] = Vector2F(0, 0);
    uvs[22] = Vector2F(0, 1);
    uvs[23] = Vector2F(1, 1);

    auto indices = std::vector<uint32_t>(36);
    // Up
    indices[0] = 0;
    indices[1] = 2;
    indices[2] = 1;
    indices[3] = 2;
    indices[4] = 0;
    indices[5] = 3;
    // Down
    indices[6] = 4;
    indices[7] = 6;
    indices[8] = 7;
    indices[9] = 6;
    indices[10] = 4;
    indices[11] = 5;
    // Left
    indices[12] = 8;
    indices[13] = 10;
    indices[14] = 9;
    indices[15] = 10;
    indices[16] = 8;
    indices[17] = 11;
    // Right
    indices[18] = 12;
    indices[19] = 14;
    indices[20] = 15;
    indices[21] = 14;
    indices[22] = 12;
    indices[23] = 13;
    // Front
    indices[24] = 16;
    indices[25] = 18;
    indices[26] = 17;
    indices[27] = 18;
    indices[28] = 16;
    indices[29] = 19;
    // Back
    indices[30] = 20;
    indices[31] = 22;
    indices[32] = 23;
    indices[33] = 22;
    indices[34] = 20;
    indices[35] = 21;

    auto &bounds = mesh->bounds_;
    bounds.lower_corner = Point3F(-kHalfWidth, -kHalfHeight, -kHalfDepth);
    bounds.upper_corner = Point3F(kHalfWidth, kHalfHeight, kHalfDepth);

    PrimitiveMesh::Initialize(mesh, positions, normals, uvs, indices, no_longer_accessible);
    return mesh;
}

ModelMeshPtr PrimitiveMesh::CreatePlane(
        float width, float height, size_t horizontal_segments, size_t vertical_segments, bool no_longer_accessible) {
    auto mesh = MeshManager::GetSingleton().LoadModelMesh();
    horizontal_segments = std::max(size_t(1), horizontal_segments);
    vertical_segments = std::max(size_t(1), vertical_segments);

    const auto kHorizontalCount = horizontal_segments + 1;
    const auto kVerticalCount = vertical_segments + 1;
    const auto kHalfWidth = width / 2;
    const auto kHalfHeight = height / 2;
    const auto kGridWidth = width / static_cast<float>(horizontal_segments);
    const auto kGridHeight = height / static_cast<float>(vertical_segments);
    const auto kVertexCount = kHorizontalCount * kVerticalCount;
    const auto kRectangleCount = vertical_segments * horizontal_segments;
    auto indices = std::vector<uint32_t>(kRectangleCount * 6);
    const auto kHorizontalCountReciprocal = 1.f / static_cast<float>(kHorizontalCount);
    const auto kHorizontalSegmentsReciprocal = 1.f / static_cast<float>(horizontal_segments);
    const auto kVerticalSegmentsReciprocal = 1.f / static_cast<float>(vertical_segments);

    auto positions = std::vector<Vector3F>(kVertexCount);
    auto normals = std::vector<Vector3F>(kVertexCount);
    auto uvs = std::vector<Vector2F>(kVertexCount);

    for (size_t i = 0; i < kVertexCount; ++i) {
        const auto kX = i % kHorizontalCount;
        const auto kZ = size_t(float(i) * kHorizontalCountReciprocal) | 0;

        // Position
        positions[i] = Vector3F(static_cast<float>(kX) * kGridWidth - kHalfWidth, 0,
                                static_cast<float>(kZ) * kGridHeight - kHalfHeight);
        // Normal
        normals[i] = Vector3F(0, 1, 0);
        // Texcoord
        uvs[i] = Vector2F(static_cast<float>(kX) * kHorizontalSegmentsReciprocal,
                          static_cast<float>(kZ) * kVerticalSegmentsReciprocal);
    }

    size_t offset = 0;
    for (size_t i = 0; i < kRectangleCount; ++i) {
        const auto kX = i % horizontal_segments;
        const auto kY = size_t(float(i) * kHorizontalSegmentsReciprocal) | 0;

        const auto kA = kY * kHorizontalCount + kX;
        const auto kB = kA + 1;
        const auto kC = kA + kHorizontalCount;
        const auto kD = kC + 1;

        indices[offset++] = static_cast<uint32_t>(kA);
        indices[offset++] = static_cast<uint32_t>(kC);
        indices[offset++] = static_cast<uint32_t>(kB);
        indices[offset++] = static_cast<uint32_t>(kC);
        indices[offset++] = static_cast<uint32_t>(kD);
        indices[offset++] = static_cast<uint32_t>(kB);
    }

    auto &bounds = mesh->bounds_;
    bounds.lower_corner = Point3F(-kHalfWidth, 0, -kHalfHeight);
    bounds.upper_corner = Point3F(kHalfWidth, 0, kHalfHeight);

    PrimitiveMesh::Initialize(mesh, positions, normals, uvs, indices, no_longer_accessible);
    return mesh;
}

ModelMeshPtr PrimitiveMesh::CreateCylinder(float radius_top,
                                           float radius_bottom,
                                           float height,
                                           size_t radial_segments,
                                           size_t height_segments,
                                           bool no_longer_accessible) {
    auto mesh = MeshManager::GetSingleton().LoadModelMesh();

    const auto kRadialCount = radial_segments + 1;
    const auto kVerticalCount = height_segments + 1;
    const auto kHalfHeight = height * 0.5f;
    const float kUnitHeight = height / static_cast<float>(height_segments);
    const auto kTorsoVertexCount = kRadialCount * kVerticalCount;
    const auto kTorsoRectangleCount = radial_segments * height_segments;
    const auto kCapTriangleCount = radial_segments * 2;
    const auto kTotalVertexCount = kTorsoVertexCount + 2 + kCapTriangleCount;
    auto indices = std::vector<uint32_t>(kTorsoRectangleCount * 6 + kCapTriangleCount * 3);
    const float kRadialCountReciprocal = 1.f / static_cast<float>(kRadialCount);
    const float kRadialSegmentsReciprocal = 1.f / static_cast<float>(radial_segments);
    const float kHeightSegmentsReciprocal = 1.f / static_cast<float>(height_segments);

    auto positions = std::vector<Vector3F>(kTotalVertexCount);
    auto normals = std::vector<Vector3F>(kTotalVertexCount);
    auto uvs = std::vector<Vector2F>(kTotalVertexCount);

    size_t indices_offset = 0;

    // Create torso
    const float kThetaStart = M_PI;
    const float kThetaRange = M_PI * 2;
    const auto kRadiusDiff = radius_bottom - radius_top;
    const auto kSlope = kRadiusDiff / height;
    const float kRadiusSlope = kRadiusDiff / static_cast<float>(height_segments);

    for (size_t i = 0; i < kTorsoVertexCount; ++i) {
        const auto kX = i % kRadialCount;
        const auto kY = size_t(float(i) * kRadialCountReciprocal) | 0;
        const auto kU = static_cast<float>(kX) * kRadialSegmentsReciprocal;
        const auto kV = static_cast<float>(kY) * kHeightSegmentsReciprocal;
        const auto kTheta = kThetaStart + kU * kThetaRange;
        const auto kSinTheta = std::sin(kTheta);
        const auto kCosTheta = std::cos(kTheta);
        const auto kRadius = radius_bottom - static_cast<float>(kY) * kRadiusSlope;

        const auto kPosX = kRadius * kSinTheta;
        const auto kPosY = static_cast<float>(kY) * kUnitHeight - kHalfHeight;
        const auto kPosZ = kRadius * kCosTheta;

        // Position
        positions[i] = Vector3F(kPosX, kPosY, kPosZ);
        // Normal
        normals[i] = Vector3F(kSinTheta, kSlope, kCosTheta);
        // Texcoord
        uvs[i] = Vector2F(kU, 1 - kV);
    }

    for (size_t i = 0; i < kTorsoRectangleCount; ++i) {
        const auto kX = i % radial_segments;
        const auto kY = size_t(float(i) * kRadialSegmentsReciprocal) | 0;

        const auto kA = kY * kRadialCount + kX;
        const auto kB = kA + 1;
        const auto kC = kA + kRadialCount;
        const auto kD = kC + 1;

        indices[indices_offset++] = static_cast<uint32_t>(kB);
        indices[indices_offset++] = static_cast<uint32_t>(kC);
        indices[indices_offset++] = static_cast<uint32_t>(kA);
        indices[indices_offset++] = static_cast<uint32_t>(kB);
        indices[indices_offset++] = static_cast<uint32_t>(kD);
        indices[indices_offset++] = static_cast<uint32_t>(kC);
    }

    // Bottom position
    positions[kTorsoVertexCount] = Vector3F(0, -kHalfHeight, 0);
    // Bottom normal
    normals[kTorsoVertexCount] = Vector3F(0, -1, 0);
    // Bottom texcoord
    uvs[kTorsoVertexCount] = Vector2F(0.5, 0.5);

    // Top position
    positions[kTorsoVertexCount + 1] = Vector3F(0, kHalfHeight, 0);
    // Top normal
    normals[kTorsoVertexCount + 1] = Vector3F(0, 1, 0);
    // Top texcoord
    uvs[kTorsoVertexCount + 1] = Vector2F(0.5, 0.5);

    // Add cap vertices
    auto offset = kTorsoVertexCount + 2;

    const auto kDiameterTopReciprocal = 1.f / (radius_top * 2);
    const auto kDiameterBottomReciprocal = 1.f / (radius_bottom * 2);
    const auto kPositionStride = kRadialCount * height_segments;
    for (size_t i = 0; i < radial_segments; ++i) {
        const auto kCurPosBottom = positions[i];
        float cur_pos_x = kCurPosBottom.x;
        float cur_pos_z = kCurPosBottom.z;

        // Bottom position
        positions[offset] = Vector3F(cur_pos_x, -kHalfHeight, cur_pos_z);
        // Bottom normal
        normals[offset] = Vector3F(0, -1, 0);
        // Bottom texcoord
        uvs[offset++] =
                Vector2F(cur_pos_x * kDiameterBottomReciprocal + 0.5f, 0.5f - cur_pos_z * kDiameterBottomReciprocal);

        const auto &cur_pos_top = positions[i + kPositionStride];
        cur_pos_x = cur_pos_top.x;
        cur_pos_z = cur_pos_top.z;

        // Top position
        positions[offset] = Vector3F(cur_pos_x, kHalfHeight, cur_pos_z);
        // Top normal
        normals[offset] = Vector3F(0, 1, 0);
        // Top texcoord
        uvs[offset++] = Vector2F(cur_pos_x * kDiameterTopReciprocal + 0.5f, cur_pos_z * kDiameterTopReciprocal + 0.5f);
    }

    // Add cap indices
    const auto kTopCapIndex = kTorsoVertexCount + 1;
    const auto kBottomIndiceIndex = kTorsoVertexCount + 2;
    const auto kTopIndiceIndex = kBottomIndiceIndex + 1;
    for (size_t i = 0; i < radial_segments; ++i) {
        const auto kFirstStride = i * 2;
        const auto kSecondStride = i == radial_segments - 1 ? 0 : kFirstStride + 2;

        // Bottom
        indices[indices_offset++] = static_cast<uint32_t>(kTorsoVertexCount);
        indices[indices_offset++] = static_cast<uint32_t>(kBottomIndiceIndex + kSecondStride);
        indices[indices_offset++] = static_cast<uint32_t>(kBottomIndiceIndex + kFirstStride);

        // Top
        indices[indices_offset++] = static_cast<uint32_t>(kTopCapIndex);
        indices[indices_offset++] = static_cast<uint32_t>(kTopIndiceIndex + kFirstStride);
        indices[indices_offset++] = static_cast<uint32_t>(kTopIndiceIndex + kSecondStride);
    }

    auto &bounds = mesh->bounds_;
    const auto kRadiusMax = std::max(radius_top, radius_bottom);
    bounds.lower_corner = Point3F(-kRadiusMax, -kHalfHeight, -kRadiusMax);
    bounds.upper_corner = Point3F(kRadiusMax, kHalfHeight, kRadiusMax);

    PrimitiveMesh::Initialize(mesh, positions, normals, uvs, indices, no_longer_accessible);
    return mesh;
}

ModelMeshPtr PrimitiveMesh::CreateTorus(float radius,
                                        float tube_radius,
                                        size_t radial_segments,
                                        size_t tubular_segments,
                                        float arc,
                                        bool no_longer_accessible) {
    auto mesh = MeshManager::GetSingleton().LoadModelMesh();

    const auto kVertexCount = (radial_segments + 1) * (tubular_segments + 1);
    const auto kRectangleCount = radial_segments * tubular_segments;
    auto indices = std::vector<uint32_t>(kRectangleCount * 6);

    auto positions = std::vector<Vector3F>(kVertexCount);
    auto normals = std::vector<Vector3F>(kVertexCount);
    auto uvs = std::vector<Vector2F>(kVertexCount);

    arc = (arc / 180.f) * static_cast<float>(M_PI);

    size_t offset = 0;
    for (size_t i = 0; i <= radial_segments; i++) {
        for (size_t j = 0; j <= tubular_segments; j++) {
            const auto kU = static_cast<float>(j) / static_cast<float>(tubular_segments) * arc;
            const auto kV =
                    static_cast<float>(i) / static_cast<float>(radial_segments) * static_cast<float>(M_PI) * 2.f;
            const auto kCosV = std::cos(kV);
            const auto kSinV = std::sin(kV);
            const auto kCosU = std::cos(kU);
            const auto kSinU = std::sin(kU);

            const auto kPosition = Vector3F((radius + tube_radius * kCosV) * kCosU,
                                            (radius + tube_radius * kCosV) * kSinU, tube_radius * kSinV);
            positions[offset] = kPosition;

            const auto kCenterX = radius * kCosU;
            const auto kCenterY = radius * kSinU;
            normals[offset] = Vector3F(kPosition.x - kCenterX, kPosition.y - kCenterY, kPosition.z).normalized();

            uvs[offset++] = Vector2F(static_cast<float>(j) / static_cast<float>(tubular_segments),
                                     static_cast<float>(i) / static_cast<float>(radial_segments));
        }
    }

    offset = 0;
    for (size_t i = 1; i <= radial_segments; i++) {
        for (size_t j = 1; j <= tubular_segments; j++) {
            const auto kA = (tubular_segments + 1) * i + j - 1;
            const auto kB = (tubular_segments + 1) * (i - 1) + j - 1;
            const auto kC = (tubular_segments + 1) * (i - 1) + j;
            const auto kD = (tubular_segments + 1) * i + j;

            indices[offset++] = static_cast<uint32_t>(kA);
            indices[offset++] = static_cast<uint32_t>(kB);
            indices[offset++] = static_cast<uint32_t>(kD);

            indices[offset++] = static_cast<uint32_t>(kB);
            indices[offset++] = static_cast<uint32_t>(kC);
            indices[offset++] = static_cast<uint32_t>(kD);
        }
    }

    auto &bounds = mesh->bounds_;
    const auto kOuterRadius = radius + tube_radius;
    bounds.lower_corner = Point3F(-kOuterRadius, -kOuterRadius, -tube_radius);
    bounds.upper_corner = Point3F(kOuterRadius, kOuterRadius, tube_radius);

    PrimitiveMesh::Initialize(mesh, positions, normals, uvs, indices, no_longer_accessible);
    return mesh;
}

ModelMeshPtr PrimitiveMesh::CreateCone(
        float radius, float height, size_t radial_segments, size_t height_segments, bool no_longer_accessible) {
    auto mesh = MeshManager::GetSingleton().LoadModelMesh();

    const auto kRadialCount = radial_segments + 1;
    const auto kVerticalCount = height_segments + 1;
    const auto kHalfHeight = height * 0.5f;
    const auto kUnitHeight = height / static_cast<float>(height_segments);
    const auto kTorsoVertexCount = kRadialCount * kVerticalCount;
    const auto kTorsoRectangleCount = radial_segments * height_segments;
    const auto kTotalVertexCount = kTorsoVertexCount + 1 + radial_segments;
    auto indices = std::vector<uint32_t>(kTorsoRectangleCount * 6 + radial_segments * 3);
    const auto kRadialCountReciprocal = 1.f / static_cast<float>(kRadialCount);
    const auto kRadialSegmentsReciprocal = 1.f / static_cast<float>(radial_segments);
    const auto kHeightSegmentsReciprocal = 1.f / static_cast<float>(height_segments);

    auto positions = std::vector<Vector3F>(kTotalVertexCount);
    auto normals = std::vector<Vector3F>(kTotalVertexCount);
    auto uvs = std::vector<Vector2F>(kTotalVertexCount);

    size_t indices_offset = 0;

    // Create torso
    const auto kThetaStart = static_cast<float>(M_PI);
    const auto kThetaRange = static_cast<float>(M_PI * 2);
    const auto kSlope = radius / height;

    for (size_t i = 0; i < kTorsoVertexCount; ++i) {
        const auto kX = i % kRadialCount;
        const auto kY = size_t(float(i) * kRadialCountReciprocal) | 0;
        const auto kU = static_cast<float>(kX) * kRadialSegmentsReciprocal;
        const auto kV = static_cast<float>(kY) * kHeightSegmentsReciprocal;
        const auto kTheta = kThetaStart + kU * kThetaRange;
        const auto kSinTheta = std::sin(kTheta);
        const auto kCosTheta = std::cos(kTheta);
        const auto kCurRadius = radius - static_cast<float>(kY) * radius;

        const auto kPosX = kCurRadius * kSinTheta;
        const auto kPosY = static_cast<float>(kY) * kUnitHeight - kHalfHeight;
        const auto kPosZ = kCurRadius * kCosTheta;

        // Position
        positions[i] = Vector3F(kPosX, kPosY, kPosZ);
        // Normal
        normals[i] = Vector3F(kSinTheta, kSlope, kCosTheta);
        // Texcoord
        uvs[i] = Vector2F(kU, 1 - kV);
    }

    for (size_t i = 0; i < kTorsoRectangleCount; ++i) {
        const auto kX = i % radial_segments;
        const auto kY = size_t(float(i) * kRadialSegmentsReciprocal) | 0;

        const auto kA = kY * kRadialCount + kX;
        const auto kB = kA + 1;
        const auto kC = kA + kRadialCount;
        const auto kD = kC + 1;

        indices[indices_offset++] = static_cast<uint32_t>(kB);
        indices[indices_offset++] = static_cast<uint32_t>(kC);
        indices[indices_offset++] = static_cast<uint32_t>(kA);
        indices[indices_offset++] = static_cast<uint32_t>(kB);
        indices[indices_offset++] = static_cast<uint32_t>(kD);
        indices[indices_offset++] = static_cast<uint32_t>(kC);
    }

    // Bottom position
    positions[kTorsoVertexCount] = Vector3F(0, -kHalfHeight, 0);
    // Bottom normal
    normals[kTorsoVertexCount] = Vector3F(0, -1, 0);
    // Bottom texcoord
    uvs[kTorsoVertexCount] = Vector2F(0.5, 0.5);

    // Add bottom cap vertices
    size_t offset = kTorsoVertexCount + 1;
    const auto kDiameterBottomReciprocal = 1.f / (radius * 2);
    for (size_t i = 0; i < radial_segments; ++i) {
        const auto &cur_pos = positions[i];
        const auto kCurPosX = cur_pos.x;
        const auto kCurPosZ = cur_pos.z;

        // Bottom position
        positions[offset] = Vector3F(kCurPosX, -kHalfHeight, kCurPosZ);
        // Bottom normal
        normals[offset] = Vector3F(0, -1, 0);
        // Bottom texcoord
        uvs[offset++] =
                Vector2F(kCurPosX * kDiameterBottomReciprocal + 0.5f, 0.5f - kCurPosZ * kDiameterBottomReciprocal);
    }

    const auto kBottomIndiceIndex = kTorsoVertexCount + 1;
    for (size_t i = 0; i < radial_segments; ++i) {
        const auto kFirstStride = i;
        const auto kSecondStride = i == radial_segments - 1 ? 0 : kFirstStride + 1;

        // Bottom
        indices[indices_offset++] = static_cast<uint32_t>(kTorsoVertexCount);
        indices[indices_offset++] = static_cast<uint32_t>(kBottomIndiceIndex + kSecondStride);
        indices[indices_offset++] = static_cast<uint32_t>(kBottomIndiceIndex + kFirstStride);
    }

    auto &bounds = mesh->bounds_;
    bounds.lower_corner = Point3F(-radius, -kHalfHeight, -radius);
    bounds.upper_corner = Point3F(radius, kHalfHeight, radius);

    PrimitiveMesh::Initialize(mesh, positions, normals, uvs, indices, no_longer_accessible);
    return mesh;
}

ModelMeshPtr PrimitiveMesh::CreateCapsule(
        float radius, float height, size_t radial_segments, size_t height_segments, bool no_longer_accessible) {
    auto mesh = MeshManager::GetSingleton().LoadModelMesh();

    radial_segments = std::max(size_t(2), radial_segments);

    const auto kRadialCount = radial_segments + 1;
    const auto kVerticalCount = height_segments + 1;
    const auto kHalfHeight = height * 0.5f;
    const auto kUnitHeight = height / static_cast<float>(height_segments);
    const auto kTorsoVertexCount = kRadialCount * kVerticalCount;
    const auto kTorsoRectangleCount = radial_segments * height_segments;

    const auto kCapVertexCount = kRadialCount * kRadialCount;
    const auto kCapRectangleCount = radial_segments * radial_segments;

    const auto kTotalVertexCount = kTorsoVertexCount + 2 * kCapVertexCount;
    auto indices = std::vector<uint32_t>((kTorsoRectangleCount + 2 * kCapRectangleCount) * 6);

    const auto kRadialCountReciprocal = 1.f / static_cast<float>(kRadialCount);
    const auto kRadialSegmentsReciprocal = 1.f / static_cast<float>(radial_segments);
    const auto kHeightSegmentsReciprocal = 1.f / static_cast<float>(height_segments);

    const auto kHalfPi = static_cast<float>(M_PI / 2);
    const auto kDoublePi = static_cast<float>(M_PI * 2);

    auto positions = std::vector<Vector3F>(kTotalVertexCount);
    auto normals = std::vector<Vector3F>(kTotalVertexCount);
    auto uvs = std::vector<Vector2F>(kTotalVertexCount);

    size_t indices_offset = 0;

    // create torso
    for (size_t i = 0; i < kTorsoVertexCount; ++i) {
        const auto kX = i % kRadialCount;
        const auto kY = size_t(float(i) * kRadialCountReciprocal) | 0;
        const auto kU = static_cast<float>(kX) * kRadialSegmentsReciprocal;
        const auto kV = static_cast<float>(kY) * kHeightSegmentsReciprocal;
        const auto kTheta = -kHalfPi + kU * kDoublePi;
        const auto kSinTheta = std::sin(kTheta);
        const auto kCosTheta = std::cos(kTheta);

        positions[i] =
                Vector3F(radius * kSinTheta, static_cast<float>(kY) * kUnitHeight - kHalfHeight, radius * kCosTheta);
        normals[i] = Vector3F(kSinTheta, 0, kCosTheta);
        uvs[i] = Vector2F(kU, 1 - kV);
    }

    for (size_t i = 0; i < kTorsoRectangleCount; ++i) {
        const auto kX = i % radial_segments;
        const auto kY = size_t(float(i) * kRadialSegmentsReciprocal) | 0;

        const auto kA = kY * kRadialCount + kX;
        const auto kB = kA + 1;
        const auto kC = kA + kRadialCount;
        const auto kD = kC + 1;

        indices[indices_offset++] = static_cast<uint32_t>(kB);
        indices[indices_offset++] = static_cast<uint32_t>(kC);
        indices[indices_offset++] = static_cast<uint32_t>(kA);
        indices[indices_offset++] = static_cast<uint32_t>(kB);
        indices[indices_offset++] = static_cast<uint32_t>(kD);
        indices[indices_offset++] = static_cast<uint32_t>(kC);
    }

    PrimitiveMesh::CreateCapsuleCap(radius, height, radial_segments, kDoublePi, kTorsoVertexCount, 1, positions,
                                    normals, uvs, indices, indices_offset);

    PrimitiveMesh::CreateCapsuleCap(radius, height, radial_segments, -kDoublePi, kTorsoVertexCount + kCapVertexCount,
                                    -1, positions, normals, uvs, indices, indices_offset + 6 * kCapRectangleCount);

    auto &bounds = mesh->bounds_;
    bounds.lower_corner = Point3F(-radius, -radius - kHalfHeight, -radius);
    bounds.upper_corner = Point3F(radius, radius + kHalfHeight, radius);

    PrimitiveMesh::Initialize(mesh, positions, normals, uvs, indices, no_longer_accessible);
    return mesh;
}

void PrimitiveMesh::CreateCapsuleCap(float radius,
                                     float height,
                                     size_t radial_segments,
                                     float cap_alpha_range,
                                     size_t offset,
                                     ssize_t pos_index,
                                     std::vector<Vector3F> &positions,
                                     std::vector<Vector3F> &normals,
                                     std::vector<Vector2F> &uvs,
                                     std::vector<uint32_t> &indices,
                                     size_t indices_offset) {
    const auto kRadialCount = radial_segments + 1;
    const auto kHalfHeight = height * 0.5f;
    const auto kCapVertexCount = kRadialCount * kRadialCount;
    const auto kCapRectangleCount = radial_segments * radial_segments;
    const auto kRadialCountReciprocal = 1.f / static_cast<float>(kRadialCount);
    const auto kRadialSegmentsReciprocal = 1.f / static_cast<float>(radial_segments);

    for (size_t i = 0; i < kCapVertexCount; ++i) {
        const auto kX = i % kRadialCount;
        const auto kY = size_t(float(i) * kRadialCountReciprocal) | 0;
        const auto kU = static_cast<float>(kX) * kRadialSegmentsReciprocal;
        const auto kV = static_cast<float>(kY) * kRadialSegmentsReciprocal;
        const auto kAlphaDelta = kU * cap_alpha_range;
        const auto kThetaDelta = static_cast<float>(kV * M_PI) / 2.f;
        const auto kSinTheta = std::sin(kThetaDelta);

        const auto kPosX = -radius * std::cos(kAlphaDelta) * kSinTheta;
        const auto kPosY = (radius * std::cos(kThetaDelta) + kHalfHeight) * static_cast<float>(pos_index);
        const auto kPosZ = radius * std::sin(kAlphaDelta) * kSinTheta;

        const auto kIndex = i + offset;
        positions[kIndex] = Vector3F(kPosX, kPosY, kPosZ);
        normals[kIndex] = Vector3F(kPosX, kPosY, kPosZ);
        uvs[kIndex] = Vector2F(kU, kV);
    }

    for (size_t i = 0; i < kCapRectangleCount; ++i) {
        const auto kX = i % radial_segments;
        const auto kY = size_t(float(i) * kRadialSegmentsReciprocal) | 0;

        const auto kA = kY * kRadialCount + kX + offset;
        const auto kB = kA + 1;
        const auto kC = kA + kRadialCount;
        const auto kD = kC + 1;

        indices[indices_offset++] = static_cast<uint32_t>(kB);
        indices[indices_offset++] = static_cast<uint32_t>(kA);
        indices[indices_offset++] = static_cast<uint32_t>(kD);
        indices[indices_offset++] = static_cast<uint32_t>(kA);
        indices[indices_offset++] = static_cast<uint32_t>(kC);
        indices[indices_offset++] = static_cast<uint32_t>(kD);
    }
}

void PrimitiveMesh::Initialize(const ModelMeshPtr &mesh,
                               const std::vector<Vector3F> &positions,
                               const std::vector<Vector3F> &normals,
                               std::vector<Vector2F> &uvs,
                               const std::vector<uint32_t> &indices,
                               bool no_longer_accessible) {
    mesh->SetPositions(positions);
    mesh->SetNormals(normals);
    mesh->SetUvs(uvs);
    mesh->SetIndices(indices);

    mesh->UploadData(no_longer_accessible);
    mesh->AddSubMesh(0, static_cast<uint32_t>(indices.size()));
}

}  // namespace vox
