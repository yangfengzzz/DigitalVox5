//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.force//collision_detection.h"
#include "vox.force/aabb.h"
#include "vox.force/bounding_sphere_hierarchy.h"
#include "vox.force/common.h"

namespace vox::force {
/** Distance field collision detection. */
class DistanceFieldCollisionDetection : public CollisionDetection {
public:
    struct DistanceFieldCollisionObject : public CollisionObject {
        bool m_testMesh;
        Real m_invertSDF;
        PointCloudBSH m_bvh;
        TetMeshBSH m_bvhTets;
        TetMeshBSH m_bvhTets0;

        DistanceFieldCollisionObject() {
            m_testMesh = true;
            m_invertSDF = 1.0;
        }
        ~DistanceFieldCollisionObject() override = default;
        virtual bool collisionTest(
                const Vector3r &x, Real tolerance, Vector3r &cp, Vector3r &n, Real &dist, Real maxDist = 0.0);
        virtual void approximateNormal(const Eigen::Vector3d &x, Real tolerance, Vector3r &n);

        virtual double distance(const Eigen::Vector3d &x, Real tolerance) = 0;
        void initTetBVH(const Vector3r *vertices,
                        unsigned int numVertices,
                        const unsigned int *indices,
                        unsigned int numTets,
                        Real tolerance);
    };

    struct DistanceFieldCollisionObjectWithoutGeometry : public DistanceFieldCollisionObject {
        static int TYPE_ID;

        ~DistanceFieldCollisionObjectWithoutGeometry() override = default;
        [[nodiscard]] int &getTypeId() const override { return TYPE_ID; }
        bool collisionTest(const Vector3r &x,
                           const Real tolerance,
                           Vector3r &cp,
                           Vector3r &n,
                           Real &dist,
                           const Real maxDist = 0.0) override {
            return false;
        }
        double distance(const Eigen::Vector3d &x, const Real tolerance) override { return 0.0; }
    };

    struct DistanceFieldCollisionBox : public DistanceFieldCollisionObject {
        Vector3r m_box;
        static int TYPE_ID;

        ~DistanceFieldCollisionBox() override = default;
        [[nodiscard]] int &getTypeId() const override { return TYPE_ID; }
        double distance(const Eigen::Vector3d &x, Real tolerance) override;
    };

    struct DistanceFieldCollisionSphere : public DistanceFieldCollisionObject {
        Real m_radius;
        static int TYPE_ID;

        ~DistanceFieldCollisionSphere() override = default;
        [[nodiscard]] int &getTypeId() const override { return TYPE_ID; }
        bool collisionTest(
                const Vector3r &x, Real tolerance, Vector3r &cp, Vector3r &n, Real &dist, Real maxDist = 0.0) override;
        double distance(const Eigen::Vector3d &x, Real tolerance) override;
    };

    struct DistanceFieldCollisionTorus : public DistanceFieldCollisionObject {
        Vector2r m_radii;
        static int TYPE_ID;

        ~DistanceFieldCollisionTorus() override = default;
        [[nodiscard]] int &getTypeId() const override { return TYPE_ID; }
        double distance(const Eigen::Vector3d &x, Real tolerance) override;
    };

    struct DistanceFieldCollisionCylinder : public DistanceFieldCollisionObject {
        Vector2r m_dim;
        static int TYPE_ID;

        ~DistanceFieldCollisionCylinder() override = default;
        int &getTypeId() const override { return TYPE_ID; }
        double distance(const Eigen::Vector3d &x, Real tolerance) override;
    };

    struct DistanceFieldCollisionHollowSphere : public DistanceFieldCollisionObject {
        Real m_radius;
        Real m_thickness;
        static int TYPE_ID;

        ~DistanceFieldCollisionHollowSphere() override = default;
        [[nodiscard]] int &getTypeId() const override { return TYPE_ID; }
        bool collisionTest(
                const Vector3r &x, Real tolerance, Vector3r &cp, Vector3r &n, Real &dist, Real maxDist = 0.0) override;
        double distance(const Eigen::Vector3d &x, Real tolerance) override;
    };

    struct DistanceFieldCollisionHollowBox : public DistanceFieldCollisionObject {
        Vector3r m_box;
        Real m_thickness;
        static int TYPE_ID;

        ~DistanceFieldCollisionHollowBox() override = default;
        [[nodiscard]] int &getTypeId() const override { return TYPE_ID; }
        double distance(const Eigen::Vector3d &x, Real tolerance) override;
    };

    struct ContactData {
        char m_type;
        unsigned int m_index1;
        unsigned int m_index2;
        Vector3r m_cp1;
        Vector3r m_cp2;
        Vector3r m_normal;
        Real m_dist;
        Real m_restitution;
        Real m_friction;

        // Test
        unsigned int m_elementIndex1;
        unsigned int m_elementIndex2;
        Vector3r m_bary1;
        Vector3r m_bary2;
    };

protected:
    void collisionDetectionRigidBodies(RigidBody *rb1,
                                       DistanceFieldCollisionObject *co1,
                                       RigidBody *rb2,
                                       DistanceFieldCollisionObject *co2,
                                       Real restitutionCoeff,
                                       Real frictionCoeff,
                                       std::vector<std::vector<ContactData>> &contacts_mt);
    void collisionDetectionRBSolid(const ParticleData &pd,
                                   unsigned int offset,
                                   unsigned int numVert,
                                   DistanceFieldCollisionObject *co1,
                                   RigidBody *rb2,
                                   DistanceFieldCollisionObject *co2,
                                   Real restitutionCoeff,
                                   Real frictionCoeff,
                                   std::vector<std::vector<ContactData>> &contacts_mt);

    void collisionDetectionSolidSolid(const ParticleData &pd,
                                      unsigned int offset,
                                      unsigned int numVert,
                                      DistanceFieldCollisionObject *co1,
                                      TetModel *tm2,
                                      DistanceFieldCollisionObject *co2,
                                      Real restitutionCoeff,
                                      Real frictionCoeff,
                                      std::vector<std::vector<ContactData>> &contacts_mt);

    bool findRefTetAt(const ParticleData &pd,
                      TetModel *tm,
                      const DistanceFieldCollisionDetection::DistanceFieldCollisionObject *co,
                      const Vector3r &X,
                      unsigned int &tetIndex,
                      Vector3r &barycentricCoordinates);

public:
    DistanceFieldCollisionDetection();
    ~DistanceFieldCollisionDetection() override;

    void collisionDetection(SimulationModel &model) override;

    virtual bool isDistanceFieldCollisionObject(CollisionObject *co) const;

    void addCollisionBox(unsigned int bodyIndex,
                         unsigned int bodyType,
                         const Vector3r *vertices,
                         unsigned int numVertices,
                         const Vector3r &box,
                         bool testMesh = true,
                         bool invertSDF = false);
    void addCollisionSphere(unsigned int bodyIndex,
                            unsigned int bodyType,
                            const Vector3r *vertices,
                            unsigned int numVertices,
                            Real radius,
                            bool testMesh = true,
                            bool invertSDF = false);
    void addCollisionTorus(unsigned int bodyIndex,
                           unsigned int bodyType,
                           const Vector3r *vertices,
                           unsigned int numVertices,
                           const Vector2r &radii,
                           bool testMesh = true,
                           bool invertSDF = false);
    void addCollisionObjectWithoutGeometry(unsigned int bodyIndex,
                                           unsigned int bodyType,
                                           const Vector3r *vertices,
                                           unsigned int numVertices,
                                           bool testMesh);
    void addCollisionHollowSphere(unsigned int bodyIndex,
                                  unsigned int bodyType,
                                  const Vector3r *vertices,
                                  unsigned int numVertices,
                                  Real radius,
                                  Real thickness,
                                  bool testMesh = true,
                                  bool invertSDF = false);
    void addCollisionHollowBox(unsigned int bodyIndex,
                               unsigned int bodyType,
                               const Vector3r *vertices,
                               unsigned int numVertices,
                               const Vector3r &box,
                               Real thickness,
                               bool testMesh = true,
                               bool invertSDF = false);

    /** Add collision cylinder
     *
     * @param  bodyIndex index of corresponding body
     * @param  bodyType type of corresponding body
     * @param  dim (radius, height) of cylinder
     */
    void addCollisionCylinder(unsigned int bodyIndex,
                              unsigned int bodyType,
                              const Vector3r *vertices,
                              unsigned int numVertices,
                              const Vector2r &dim,
                              bool testMesh = true,
                              bool invertSDF = false);
};
}  // namespace vox::force