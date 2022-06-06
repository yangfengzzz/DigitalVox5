//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "simulator/cloth/simple_mesh.h"
#include "vox.cloth/foundation/PxMat44.h"
#include "vox.cloth/foundation/PxVec2.h"
#include "vox.cloth/foundation/PxVec3.h"
#include "vox.cloth/NvClothExt/ClothFabricCooker.h"

namespace vox::cloth {

struct ClothMeshData {
    template <typename T>
    static nv::cloth::BoundedData ToBoundedData(T& vector) {
        nv::cloth::BoundedData d;
        d.data = &vector[0];
        d.stride = sizeof(vector[0]);
        d.count = (physx::PxU32)vector.size();

        return d;
    }

    struct Triangle {
        Triangle() = default;

        Triangle(uint32_t _a, uint32_t _b, uint32_t _c) : a(_a), b(_b), c(_c) {}

        uint32_t a{}, b{}, c{};

        Triangle operator+(uint32_t offset) const { return {a + offset, b + offset, c + offset}; }
    };

    struct Quad {
        Quad() = default;

        Quad(uint32_t _a, uint32_t _b, uint32_t _c, uint32_t _d) : a(_a), b(_b), c(_c), d(_d) {}

        uint32_t a{}, b{}, c{}, d{};

        Quad operator+(uint32_t offset) const { return {a + offset, b + offset, c + offset, d + offset}; }
    };

    std::vector<physx::PxVec3> m_vertices;
    std::vector<physx::PxVec2> m_uvs;
    std::vector<Triangle> m_triangles;
    std::vector<Quad> m_quads;
    std::vector<physx::PxReal> m_inv_masses;

    SimpleMesh m_mesh;

    void Clear();

    void GeneratePlaneCloth(float width,
                            float height,
                            int segments_x,
                            int segments_y,
                            bool create_quads = false,
                            const physx::PxMat44& transform = physx::PxIdentity,
                            bool alternating_diagonals = true,
                            int zigzag = 0);

    void GenerateCylinderWave(float radius_top,
                              float radius_bottom,
                              float height,
                              float frequency,
                              float ampitude_top,
                              float ampitude_bottom,
                              int segments_x,
                              int segments_y,
                              const physx::PxMat44& transform = physx::PxIdentity,
                              bool attach_top = false,
                              bool attach_bottom = false,
                              bool create_quads = false,
                              int missing_x_segments = 0);

    void AttachClothPlaneByAngles(int segments_x, int segments_y, bool attach_by_width = true);

    void AttachClothPlaneBySide(int segments_x, int segments_y, bool attach_by_width = true);

    bool ReadClothFromFile(const std::string& vertices_path,
                           const std::string& indices_path,
                           const physx::PxMat44& transform = physx::PxIdentity);

    // positions as float (3 elements per position)
    template <typename PositionType = float, typename IndexType = uint16_t>
    bool InitializeFromData(nv::cloth::BoundedData positions,
                            nv::cloth::BoundedData indices,
                            const physx::PxMat44& transform = physx::PxMat44(physx::PxIdentity));

    void AttachClothUsingTopVertices(float threshold_y = 0.5f);

    void SetInvMasses(float inv_mass);

    void SetInvMassesFromDensity(float density);  // Todo

    nv::cloth::ClothMeshDesc GetClothMeshDesc();

    SimpleMesh GetRenderMesh() const;

    void Merge(const ClothMeshData& other);
};

}  // namespace vox::cloth