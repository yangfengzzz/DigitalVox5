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
    //MARK: -
    struct DistanceFieldCollisionObject : public CollisionObject {
        bool m_test_mesh;
        Real m_invert_sdf;
        PointCloudBSH m_bvh;
        TetMeshBSH m_bvh_tets;
        TetMeshBSH m_bvh_tets_0;

        DistanceFieldCollisionObject() {
            m_test_mesh = true;
            m_invert_sdf = 1.0;
        }
        ~DistanceFieldCollisionObject() override = default;
        virtual bool CollisionTest(
                const Vector3r &x, Real tolerance, Vector3r &cp, Vector3r &n, Real &dist, Real max_dist = 0.0);
        virtual void ApproximateNormal(const Eigen::Vector3d &x, Real tolerance, Vector3r &n);

        virtual double Distance(const Eigen::Vector3d &x, Real tolerance) = 0;
        void InitTetBvh(const Vector3r *vertices,
                        unsigned int num_vertices,
                        const unsigned int *indices,
                        unsigned int num_tets,
                        Real tolerance);
    };

    //MARK: -
    struct DistanceFieldCollisionObjectWithoutGeometry : public DistanceFieldCollisionObject {
        static int type_id;

        ~DistanceFieldCollisionObjectWithoutGeometry() override = default;
        [[nodiscard]] int &GetTypeId() const override { return type_id; }
        bool CollisionTest(const Vector3r &x,
                           const Real tolerance,
                           Vector3r &cp,
                           Vector3r &n,
                           Real &dist,
                           const Real max_dist = 0.0) override {
            return false;
        }
        double Distance(const Eigen::Vector3d &x, const Real tolerance) override { return 0.0; }
    };

    //MARK: -
    struct DistanceFieldCollisionBox : public DistanceFieldCollisionObject {
        Vector3r m_box;
        static int type_id;

        ~DistanceFieldCollisionBox() override = default;
        [[nodiscard]] int &GetTypeId() const override { return type_id; }
        double Distance(const Eigen::Vector3d &x, Real tolerance) override;
    };

    //MARK: -
    struct DistanceFieldCollisionSphere : public DistanceFieldCollisionObject {
        Real m_radius;
        static int type_id;

        ~DistanceFieldCollisionSphere() override = default;
        [[nodiscard]] int &GetTypeId() const override { return type_id; }
        bool CollisionTest(
                const Vector3r &x, Real tolerance, Vector3r &cp, Vector3r &n, Real &dist, Real max_dist = 0.0) override;
        double Distance(const Eigen::Vector3d &x, Real tolerance) override;
    };

    //MARK: -
    struct DistanceFieldCollisionTorus : public DistanceFieldCollisionObject {
        Vector2r m_radii;
        static int type_id;

        ~DistanceFieldCollisionTorus() override = default;
        [[nodiscard]] int &GetTypeId() const override { return type_id; }
        double Distance(const Eigen::Vector3d &x, Real tolerance) override;
    };

    //MARK: -
    struct DistanceFieldCollisionCylinder : public DistanceFieldCollisionObject {
        Vector2r m_dim;
        static int type_id;

        ~DistanceFieldCollisionCylinder() override = default;
        [[nodiscard]] int &GetTypeId() const override { return type_id; }
        double Distance(const Eigen::Vector3d &x, Real tolerance) override;
    };

    //MARK: -
    struct DistanceFieldCollisionHollowSphere : public DistanceFieldCollisionObject {
        Real m_radius;
        Real m_thickness;
        static int type_id;

        ~DistanceFieldCollisionHollowSphere() override = default;
        [[nodiscard]] int &GetTypeId() const override { return type_id; }
        bool CollisionTest(
                const Vector3r &x, Real tolerance, Vector3r &cp, Vector3r &n, Real &dist, Real max_dist = 0.0) override;
        double Distance(const Eigen::Vector3d &x, Real tolerance) override;
    };

    //MARK: -
    struct DistanceFieldCollisionHollowBox : public DistanceFieldCollisionObject {
        Vector3r m_box;
        Real m_thickness;
        static int type_id;

        ~DistanceFieldCollisionHollowBox() override = default;
        [[nodiscard]] int &GetTypeId() const override { return type_id; }
        double Distance(const Eigen::Vector3d &x, Real tolerance) override;
    };

    struct ContactData {
        char m_type;
        unsigned int m_index_1;
        unsigned int m_index_2;
        Vector3r m_cp1;
        Vector3r m_cp2;
        Vector3r m_normal;
        Real m_dist;
        Real m_restitution;
        Real m_friction;

        // Test
        unsigned int m_element_index_1;
        unsigned int m_element_index_2;
        Vector3r m_bary1;
        Vector3r m_bary2;
    };

protected:
    void CollisionDetectionRigidBodies(RigidBody *rb1,
                                       DistanceFieldCollisionObject *co1,
                                       RigidBody *rb2,
                                       DistanceFieldCollisionObject *co2,
                                       Real restitution_coeff,
                                       Real friction_coeff,
                                       std::vector<std::vector<ContactData>> &contacts_mt);
    void CollisionDetectionRbSolid(const ParticleData &pd,
                                   unsigned int offset,
                                   unsigned int num_vert,
                                   DistanceFieldCollisionObject *co1,
                                   RigidBody *rb2,
                                   DistanceFieldCollisionObject *co2,
                                   Real restitution_coeff,
                                   Real friction_coeff,
                                   std::vector<std::vector<ContactData>> &contacts_mt);

    void CollisionDetectionSolidSolid(const ParticleData &pd,
                                      unsigned int offset,
                                      unsigned int num_vert,
                                      DistanceFieldCollisionObject *co1,
                                      TetModel *tm2,
                                      DistanceFieldCollisionObject *co2,
                                      Real restitution_coeff,
                                      Real friction_coeff,
                                      std::vector<std::vector<ContactData>> &contacts_mt);

    bool FindRefTetAt(const ParticleData &pd,
                      TetModel *tm,
                      const DistanceFieldCollisionDetection::DistanceFieldCollisionObject *co,
                      const Vector3r &x,
                      unsigned int &tet_index,
                      Vector3r &barycentric_coordinates);

public:
    DistanceFieldCollisionDetection();
    ~DistanceFieldCollisionDetection() override;

    void GetCollisionDetection(SimulationModel &model) override;

    virtual bool IsDistanceFieldCollisionObject(CollisionObject *co) const;

    void AddCollisionBox(unsigned int body_index,
                         unsigned int body_type,
                         const Vector3r *vertices,
                         unsigned int num_vertices,
                         const Vector3r &box,
                         bool test_mesh = true,
                         bool invert_sdf = false);

    void AddCollisionSphere(unsigned int body_index,
                            unsigned int body_type,
                            const Vector3r *vertices,
                            unsigned int num_vertices,
                            Real radius,
                            bool test_mesh = true,
                            bool invert_sdf = false);

    void AddCollisionTorus(unsigned int body_index,
                           unsigned int body_type,
                           const Vector3r *vertices,
                           unsigned int num_vertices,
                           const Vector2r &radii,
                           bool test_mesh = true,
                           bool invert_sdf = false);

    void AddCollisionObjectWithoutGeometry(unsigned int body_index,
                                           unsigned int body_type,
                                           const Vector3r *vertices,
                                           unsigned int num_vertices,
                                           bool test_mesh);
    void AddCollisionHollowSphere(unsigned int body_index,
                                  unsigned int body_type,
                                  const Vector3r *vertices,
                                  unsigned int num_vertices,
                                  Real radius,
                                  Real thickness,
                                  bool test_mesh = true,
                                  bool invert_sdf = false);
    void AddCollisionHollowBox(unsigned int body_index,
                               unsigned int body_type,
                               const Vector3r *vertices,
                               unsigned int num_vertices,
                               const Vector3r &box,
                               Real thickness,
                               bool test_mesh = true,
                               bool invert_sdf = false);

    /** Add collision cylinder
     *
     * @param  body_index index of corresponding body
     * @param  body_type type of corresponding body
     * @param  dim (radius, height) of cylinder
     */
    void AddCollisionCylinder(unsigned int body_index,
                              unsigned int body_type,
                              const Vector3r *vertices,
                              unsigned int num_vertices,
                              const Vector2r &dim,
                              bool test_mesh = true,
                              bool invert_sdf = false);
};
}  // namespace vox::force
