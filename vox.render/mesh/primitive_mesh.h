//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.math/vector3.h"
#include "vox.render/mesh/model_mesh.h"

namespace vox {
/**
 * Used to generate common primitive meshes.
 */
class PrimitiveMesh {
public:
    /**
     * Create a sphere mesh.
     * @param radius - Sphere radius
     * @param segments - Number of segments
     * @param no_longer_accessible - No longer access the vertices of the mesh after creation
     * @returns Sphere model mesh
     */
    static ModelMeshPtr CreateSphere(float radius = 0.5, size_t segments = 18, bool no_longer_accessible = true);

    /**
     * Create a cuboid mesh.
     * @param width - Cuboid width
     * @param height - Cuboid height
     * @param depth - Cuboid depth
     * @param no_longer_accessible - No longer access the vertices of the mesh after creation
     * @returns Cuboid model mesh
     */
    static ModelMeshPtr CreateCuboid(float width = 1,
                                     float height = 1,
                                     float depth = 1,
                                     bool no_longer_accessible = true);

    /**
     * Create a plane mesh.
     * @param width - Plane width
     * @param height - Plane height
     * @param horizontal_segments - Plane horizontal segments
     * @param vertical_segments - Plane vertical segments
     * @param no_longer_accessible - No longer access the vertices of the mesh after creation
     * @returns Plane model mesh
     */
    static ModelMeshPtr CreatePlane(float width = 1,
                                    float height = 1,
                                    size_t horizontal_segments = 1,
                                    size_t vertical_segments = 1,
                                    bool no_longer_accessible = true);

    /**
     * Create a cylinder mesh.
     * @param radius_top - The radius of top cap
     * @param radius_bottom - The radius of bottom cap
     * @param height - The height of torso
     * @param radial_segments - Cylinder radial segments
     * @param height_segments - Cylinder height segments
     * @param no_longer_accessible - No longer access the vertices of the mesh after creation
     * @returns Cylinder model mesh
     */
    static ModelMeshPtr CreateCylinder(float radius_top = 0.5,
                                       float radius_bottom = 0.5,
                                       float height = 2,
                                       size_t radial_segments = 20,
                                       size_t height_segments = 1,
                                       bool no_longer_accessible = true);

    /**
     * Create a torus mesh.
     * @param radius - Torus radius
     * @param tube_radius - Torus tube
     * @param radial_segments - Torus radial segments
     * @param tubular_segments - Torus tubular segments
     * @param arc - Central angle
     * @param no_longer_accessible - No longer access the vertices of the mesh after creation
     * @returns Torus model mesh
     */
    static ModelMeshPtr CreateTorus(float radius = 0.5,
                                    float tube_radius = 0.1,
                                    size_t radial_segments = 30,
                                    size_t tubular_segments = 30,
                                    float arc = 360,
                                    bool no_longer_accessible = true);

    /**
     * Create a cone mesh.
     * @param radius - The radius of cap
     * @param height - The height of torso
     * @param radial_segments - Cylinder radial segments
     * @param height_segments - Cylinder height segments
     * @param no_longer_accessible - No longer access the vertices of the mesh after creation
     * @returns Cone model mesh
     */
    static ModelMeshPtr CreateCone(float radius = 0.5,
                                   float height = 2,
                                   size_t radial_segments = 20,
                                   size_t height_segments = 1,
                                   bool no_longer_accessible = true);

    /**
     * Create a capsule mesh.
     * @param radius - The radius of the two hemispherical ends
     * @param height - The height of the cylindrical part, measured between the centers of the hemispherical ends
     * @param radial_segments - Hemispherical end radial segments
     * @param height_segments - Cylindrical part height segments
     * @param no_longer_accessible - No longer access the vertices of the mesh after creation
     * @returns Capsule model mesh
     */
    static ModelMeshPtr CreateCapsule(float radius = 0.5,
                                      float height = 2,
                                      size_t radial_segments = 6,
                                      size_t height_segments = 1,
                                      bool no_longer_accessible = true);

private:
    static void CreateCapsuleCap(float radius,
                                 float height,
                                 size_t radial_segments,
                                 float cap_alpha_range,
                                 size_t offset,
                                 ssize_t pos_index,
                                 std::vector<Vector3F> &positions,
                                 std::vector<Vector3F> &normals,
                                 std::vector<Vector2F> &uvs,
                                 std::vector<uint32_t> &indices,
                                 size_t indices_offset);

    static void Initialize(const ModelMeshPtr &mesh,
                           const std::vector<Vector3F> &positions,
                           const std::vector<Vector3F> &normals,
                           std::vector<Vector2F> &uvs,
                           const std::vector<uint32_t> &indices,
                           bool no_longer_accessible = true);
};

}  // namespace vox
