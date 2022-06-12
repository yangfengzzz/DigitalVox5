//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <array>
#include <cmath>
#include <list>
#include <memory>
#include <vector>

#include "vox.compute/common.h"
#include "vox.compute/direct_position_based_solver_for_stiff_rods_interface.h"

namespace vox::compute {
struct SimulationModel;

//MARK: -
struct Constraint {
public:
    /** indices of the linked bodies */
    std::vector<unsigned int> m_bodies;

    explicit Constraint(const unsigned int number_of_bodies) { m_bodies.resize(number_of_bodies); }

    [[nodiscard]] unsigned int NumberOfBodies() const { return static_cast<unsigned int>(m_bodies.size()); }
    virtual ~Constraint() = default;
    [[nodiscard]] virtual int &GetTypeId() const = 0;

    virtual bool InitConstraintBeforeProjection(SimulationModel &model) { return true; };
    virtual bool UpdateConstraint(SimulationModel &model) { return true; };
    virtual bool SolvePositionConstraint(SimulationModel &model, const unsigned int iter) { return true; };
    virtual bool SolveVelocityConstraint(SimulationModel &model, const unsigned int iter) { return true; };
};

//MARK: -
struct BallJoint : public Constraint {
public:
    static int type_id;
    Eigen::Matrix<Real, 3, 4, Eigen::DontAlign> m_joint_info;

    BallJoint() : Constraint(2) {}
    [[nodiscard]] int &GetTypeId() const override { return type_id; }

    bool InitConstraint(SimulationModel &model, unsigned int rb_index1, unsigned int rb_index2, const Vector3r &pos);
    bool UpdateConstraint(SimulationModel &model) override;
    bool SolvePositionConstraint(SimulationModel &model, unsigned int iter) override;
};

//MARK: -
struct BallOnLineJoint : public Constraint {
public:
    static int type_id;
    Eigen::Matrix<Real, 3, 10, Eigen::DontAlign> m_joint_info;

    BallOnLineJoint() : Constraint(2) {}
    [[nodiscard]] int &GetTypeId() const override { return type_id; }

    bool InitConstraint(SimulationModel &model,
                        unsigned int rb_index1,
                        unsigned int rb_index2,
                        const Vector3r &pos,
                        const Vector3r &dir);
    bool UpdateConstraint(SimulationModel &model) override;
    bool SolvePositionConstraint(SimulationModel &model, unsigned int iter) override;
};

//MARK: -
struct HingeJoint : public Constraint {
public:
    static int type_id;
    Eigen::Matrix<Real, 4, 7, Eigen::DontAlign> m_joint_info;

    HingeJoint() : Constraint(2) {}
    [[nodiscard]] int &GetTypeId() const override { return type_id; }

    bool InitConstraint(SimulationModel &model,
                        unsigned int rb_index1,
                        unsigned int rb_index2,
                        const Vector3r &pos,
                        const Vector3r &axis);
    bool UpdateConstraint(SimulationModel &model) override;
    bool SolvePositionConstraint(SimulationModel &model, unsigned int iter) override;
};

//MARK: -
struct UniversalJoint : public Constraint {
public:
    static int type_id;
    Eigen::Matrix<Real, 3, 8, Eigen::DontAlign> m_joint_info;

    UniversalJoint() : Constraint(2) {}
    [[nodiscard]] int &GetTypeId() const override { return type_id; }

    bool InitConstraint(SimulationModel &model,
                        unsigned int rb_index1,
                        unsigned int rb_index2,
                        const Vector3r &pos,
                        const Vector3r &axis1,
                        const Vector3r &axis2);
    bool UpdateConstraint(SimulationModel &model) override;
    bool SolvePositionConstraint(SimulationModel &model, unsigned int iter) override;
};

//MARK: -
struct SliderJoint : public Constraint {
public:
    static int type_id;
    Eigen::Matrix<Real, 4, 6, Eigen::DontAlign> m_joint_info;

    SliderJoint() : Constraint(2) {}
    [[nodiscard]] int &GetTypeId() const override { return type_id; }

    bool InitConstraint(SimulationModel &model, unsigned int rb_index1, unsigned int rb_index2, const Vector3r &axis);
    bool UpdateConstraint(SimulationModel &model) override;
    bool SolvePositionConstraint(SimulationModel &model, unsigned int iter) override;
};

//MARK: -
struct MotorJoint : public Constraint {
public:
    Real m_target;
    std::vector<Real> m_target_sequence;
    MotorJoint() : Constraint(2) { m_target = 0.0; }

    [[nodiscard]] virtual Real GetTarget() const { return m_target; }
    virtual void SetTarget(const Real val) { m_target = val; }

    virtual std::vector<Real> &GetTargetSequence() { return m_target_sequence; }
    virtual void SetTargetSequence(const std::vector<Real> &val) { m_target_sequence = val; }

    [[nodiscard]] bool GetRepeatSequence() const { return m_repeat_sequence; }
    void SetRepeatSequence(bool val) { m_repeat_sequence = val; }

private:
    bool m_repeat_sequence{};
};

//MARK: -
struct TargetPositionMotorSliderJoint : public MotorJoint {
public:
    static int type_id;
    Eigen::Matrix<Real, 4, 6, Eigen::DontAlign> m_joint_info;

    TargetPositionMotorSliderJoint() : MotorJoint() {}
    [[nodiscard]] int &GetTypeId() const override { return type_id; }

    bool InitConstraint(SimulationModel &model, unsigned int rb_index1, unsigned int rb_index2, const Vector3r &axis);
    bool UpdateConstraint(SimulationModel &model) override;
    bool SolvePositionConstraint(SimulationModel &model, unsigned int iter) override;
};

//MARK: -
struct TargetVelocityMotorSliderJoint : public MotorJoint {
public:
    static int type_id;
    Eigen::Matrix<Real, 4, 6, Eigen::DontAlign> m_joint_info;

    TargetVelocityMotorSliderJoint() : MotorJoint() {}
    [[nodiscard]] int &GetTypeId() const override { return type_id; }

    bool InitConstraint(SimulationModel &model, unsigned int rb_index1, unsigned int rb_index2, const Vector3r &axis);
    bool UpdateConstraint(SimulationModel &model) override;
    bool SolvePositionConstraint(SimulationModel &model, unsigned int iter) override;
    bool SolveVelocityConstraint(SimulationModel &model, unsigned int iter) override;
};

//MARK: -
struct TargetAngleMotorHingeJoint : public MotorJoint {
public:
    static int type_id;
    Eigen::Matrix<Real, 4, 8, Eigen::DontAlign> m_joint_info;
    TargetAngleMotorHingeJoint() : MotorJoint() {}
    [[nodiscard]] int &GetTypeId() const override { return type_id; }

    void SetTarget(const Real val) override {
        m_target = std::max<Real>(val, -M_PI);
        m_target = std::min<Real>(m_target, M_PI);
    }

    bool InitConstraint(SimulationModel &model,
                        unsigned int rb_index1,
                        unsigned int rb_index2,
                        const Vector3r &pos,
                        const Vector3r &axis);
    bool UpdateConstraint(SimulationModel &model) override;
    bool SolvePositionConstraint(SimulationModel &model, unsigned int iter) override;

private:
    std::vector<Real> m_target_sequence;
};

//MARK: -
struct TargetVelocityMotorHingeJoint : public MotorJoint {
public:
    static int type_id;
    Eigen::Matrix<Real, 4, 8, Eigen::DontAlign> m_joint_info;
    TargetVelocityMotorHingeJoint() : MotorJoint() {}
    [[nodiscard]] int &GetTypeId() const override { return type_id; }

    bool InitConstraint(SimulationModel &model,
                        unsigned int rb_index1,
                        unsigned int rb_index2,
                        const Vector3r &pos,
                        const Vector3r &axis);
    bool UpdateConstraint(SimulationModel &model) override;
    bool SolvePositionConstraint(SimulationModel &model, unsigned int iter) override;
    bool SolveVelocityConstraint(SimulationModel &model, unsigned int iter) override;
};

//MARK: -
struct DamperJoint : public Constraint {
public:
    static int type_id;
    Real m_stiffness{};
    Eigen::Matrix<Real, 4, 6, Eigen::DontAlign> m_joint_info;
    Real m_lambda{};

    DamperJoint() : Constraint(2) {}
    [[nodiscard]] int &GetTypeId() const override { return type_id; }

    bool InitConstraint(SimulationModel &model,
                        unsigned int rb_index1,
                        unsigned int rb_index2,
                        const Vector3r &axis,
                        Real stiffness);
    bool UpdateConstraint(SimulationModel &model) override;
    bool SolvePositionConstraint(SimulationModel &model, unsigned int iter) override;
};

//MARK: -
struct RigidBodyParticleBallJoint : public Constraint {
public:
    static int type_id;
    Eigen::Matrix<Real, 3, 2, Eigen::DontAlign> m_joint_info;

    RigidBodyParticleBallJoint() : Constraint(2) {}
    [[nodiscard]] int &GetTypeId() const override { return type_id; }

    bool InitConstraint(SimulationModel &model, unsigned int rb_index, unsigned int particle_index);
    bool UpdateConstraint(SimulationModel &model) override;
    bool SolvePositionConstraint(SimulationModel &model, unsigned int iter) override;
};

//MARK: -
struct RigidBodySpring : public Constraint {
public:
    static int type_id;
    Eigen::Matrix<Real, 3, 4, Eigen::DontAlign> m_joint_info;
    Real m_rest_length{};
    Real m_stiffness{};
    Real m_lambda{};

    RigidBodySpring() : Constraint(2) {}
    [[nodiscard]] int &GetTypeId() const override { return type_id; }

    bool InitConstraint(SimulationModel &model,
                        unsigned int rb_index1,
                        unsigned int rb_index2,
                        const Vector3r &pos1,
                        const Vector3r &pos2,
                        Real stiffness);
    bool UpdateConstraint(SimulationModel &model) override;
    bool SolvePositionConstraint(SimulationModel &model, unsigned int iter) override;
};

//MARK: -
struct DistanceJoint : public Constraint {
public:
    static int type_id;
    Eigen::Matrix<Real, 3, 4, Eigen::DontAlign> m_joint_info;
    Real m_rest_length{};

    DistanceJoint() : Constraint(2) {}
    [[nodiscard]] int &GetTypeId() const override { return type_id; }

    bool InitConstraint(SimulationModel &model,
                        unsigned int rb_index1,
                        unsigned int rb_index2,
                        const Vector3r &pos1,
                        const Vector3r &pos2);
    bool UpdateConstraint(SimulationModel &model) override;
    bool SolvePositionConstraint(SimulationModel &model, unsigned int iter) override;
};

//MARK: -
struct DistanceConstraint : public Constraint {
public:
    static int type_id;
    Real m_rest_length{};
    Real m_stiffness{};

    DistanceConstraint() : Constraint(2) {}
    [[nodiscard]] int &GetTypeId() const override { return type_id; }

    virtual bool InitConstraint(SimulationModel &model, unsigned int particle1, unsigned int particle2, Real stiffness);
    bool SolvePositionConstraint(SimulationModel &model, unsigned int iter) override;
};

//MARK: -
struct DistanceConstraint_XPBD : public Constraint {
public:
    static int type_id;
    Real m_rest_length{};
    Real m_lambda{};
    Real m_stiffness{};

    DistanceConstraint_XPBD() : Constraint(2) {}
    [[nodiscard]] int &GetTypeId() const override { return type_id; }

    virtual bool InitConstraint(SimulationModel &model, unsigned int particle1, unsigned int particle2, Real stiffness);
    bool SolvePositionConstraint(SimulationModel &model, unsigned int iter) override;
};

//MARK: -
struct DihedralConstraint : public Constraint {
public:
    static int type_id;
    Real m_rest_angle{};
    Real m_stiffness{};

    DihedralConstraint() : Constraint(4) {}
    [[nodiscard]] int &GetTypeId() const override { return type_id; }

    virtual bool InitConstraint(SimulationModel &model,
                                unsigned int particle1,
                                unsigned int particle2,
                                unsigned int particle3,
                                unsigned int particle4,
                                Real stiffness);
    bool SolvePositionConstraint(SimulationModel &model, unsigned int iter) override;
};

//MARK: -
struct IsometricBendingConstraint : public Constraint {
public:
    static int type_id;
    Real m_stiffness{};
    Matrix4r m_Q;

    IsometricBendingConstraint() : Constraint(4) {}
    [[nodiscard]] int &GetTypeId() const override { return type_id; }

    virtual bool InitConstraint(SimulationModel &model,
                                unsigned int particle1,
                                unsigned int particle2,
                                unsigned int particle3,
                                unsigned int particle4,
                                Real stiffness);
    bool SolvePositionConstraint(SimulationModel &model, unsigned int iter) override;
};

//MARK: -
struct IsometricBendingConstraint_XPBD : public Constraint {
public:
    static int type_id;
    Real m_stiffness{};
    Matrix4r m_Q;
    Real m_lambda{};

    IsometricBendingConstraint_XPBD() : Constraint(4) {}
    [[nodiscard]] int &GetTypeId() const override { return type_id; }

    virtual bool InitConstraint(SimulationModel &model,
                                unsigned int particle1,
                                unsigned int particle2,
                                unsigned int particle3,
                                unsigned int particle4,
                                Real stiffness);
    bool SolvePositionConstraint(SimulationModel &model, unsigned int iter) override;
};

//MARK: -
struct FEMTriangleConstraint : public Constraint {
public:
    static int type_id;
    Real m_area{};
    Matrix2r m_inv_rest_mat;
    Real m_xx_stiffness{};
    Real m_xy_stiffness{};
    Real m_yy_stiffness{};
    Real m_xy_poisson_ratio{};
    Real m_yx_poisson_ratio{};

    FEMTriangleConstraint() : Constraint(3) {}
    [[nodiscard]] int &GetTypeId() const override { return type_id; }

    virtual bool InitConstraint(SimulationModel &model,
                                unsigned int particle1,
                                unsigned int particle2,
                                unsigned int particle3,
                                Real xx_stiffness,
                                Real yy_stiffness,
                                Real xy_stiffness,
                                Real xy_poisson_ratio,
                                Real yx_poisson_ratio);
    bool SolvePositionConstraint(SimulationModel &model, unsigned int iter) override;
};

//MARK: -
struct StrainTriangleConstraint : public Constraint {
public:
    static int type_id;
    Matrix2r m_inv_rest_mat;
    Real m_xx_stiffness{};
    Real m_xy_stiffness{};
    Real m_yy_stiffness{};
    bool m_normalize_stretch{};
    bool m_normalize_shear{};

    StrainTriangleConstraint() : Constraint(3) {}
    [[nodiscard]] int &GetTypeId() const override { return type_id; }

    virtual bool InitConstraint(SimulationModel &model,
                                unsigned int particle1,
                                unsigned int particle2,
                                unsigned int particle3,
                                Real xx_stiffness,
                                Real yy_stiffness,
                                Real xy_stiffness,
                                bool normalize_stretch,
                                bool normalize_shear);
    bool SolvePositionConstraint(SimulationModel &model, unsigned int iter) override;
};

//MARK: -
struct VolumeConstraint : public Constraint {
public:
    static int type_id;
    Real m_stiffness{};
    Real m_rest_volume{};

    VolumeConstraint() : Constraint(4) {}
    [[nodiscard]] int &GetTypeId() const override { return type_id; }

    virtual bool InitConstraint(SimulationModel &model,
                                unsigned int particle1,
                                unsigned int particle2,
                                unsigned int particle3,
                                unsigned int particle4,
                                Real stiffness);
    bool SolvePositionConstraint(SimulationModel &model, unsigned int iter) override;
};

//MARK: -
struct VolumeConstraint_XPBD : public Constraint {
public:
    static int type_id;
    Real m_stiffness{};
    Real m_rest_volume{};
    Real m_lambda{};

    VolumeConstraint_XPBD() : Constraint(4) {}
    [[nodiscard]] int &GetTypeId() const override { return type_id; }

    virtual bool InitConstraint(SimulationModel &model,
                                unsigned int particle1,
                                unsigned int particle2,
                                unsigned int particle3,
                                unsigned int particle4,
                                Real stiffness);
    bool SolvePositionConstraint(SimulationModel &model, unsigned int iter) override;
};

//MARK: -
struct FEMTetConstraint : public Constraint {
public:
    static int type_id;
    Real m_stiffness{};
    Real m_poisson_ratio{};
    Real m_volume{};
    Matrix3r m_inv_rest_mat;

    FEMTetConstraint() : Constraint(4) {}
    [[nodiscard]] int &GetTypeId() const override { return type_id; }

    virtual bool InitConstraint(SimulationModel &model,
                                unsigned int particle1,
                                unsigned int particle2,
                                unsigned int particle3,
                                unsigned int particle4,
                                Real stiffness,
                                Real poisson_ratio);
    bool SolvePositionConstraint(SimulationModel &model, unsigned int iter) override;
};

//MARK: -
struct StrainTetConstraint : public Constraint {
public:
    static int type_id;
    Real m_stretch_stiffness{};
    Real m_shear_stiffness{};
    Matrix3r m_inv_rest_mat;
    bool m_normalize_stretch{};
    bool m_normalize_shear{};

    StrainTetConstraint() : Constraint(4) {}
    [[nodiscard]] int &GetTypeId() const override { return type_id; }

    virtual bool InitConstraint(SimulationModel &model,
                                unsigned int particle1,
                                unsigned int particle2,
                                unsigned int particle3,
                                unsigned int particle4,
                                Real stretch_stiffness,
                                Real shear_stiffness,
                                bool normalize_stretch,
                                bool normalize_shear);
    bool SolvePositionConstraint(SimulationModel &model, unsigned int iter) override;
};

//MARK: -
struct ShapeMatchingConstraint : public Constraint {
public:
    static int type_id;
    Real m_stiffness{};
    Vector3r m_rest_cm;
    Real *m_w;
    Vector3r *m_x0;
    Vector3r *m_x;
    Vector3r *m_corr;
    unsigned int *m_num_clusters;

    explicit ShapeMatchingConstraint(const unsigned int number_of_particles) : Constraint(number_of_particles) {
        m_x = new Vector3r[number_of_particles];
        m_x0 = new Vector3r[number_of_particles];
        m_corr = new Vector3r[number_of_particles];
        m_w = new Real[number_of_particles];
        m_num_clusters = new unsigned int[number_of_particles];
    }
    ~ShapeMatchingConstraint() override {
        delete[] m_x;
        delete[] m_x0;
        delete[] m_corr;
        delete[] m_w;
        delete[] m_num_clusters;
    }
    [[nodiscard]] int &GetTypeId() const override { return type_id; }

    virtual bool InitConstraint(SimulationModel &model,
                                const unsigned int particle_indices[],
                                const unsigned int num_clusters[],
                                Real stiffness);
    bool SolvePositionConstraint(SimulationModel &model, unsigned int iter) override;
};

//MARK: - ==
struct RigidBodyContactConstraint {
public:
    static int type_id;
    /** indices of the linked bodies */
    std::array<unsigned int, 2> m_bodies{};
    Real m_stiffness{};
    Real m_friction_coeff{};
    Real m_sum_impulses{};
    Eigen::Matrix<Real, 3, 5, Eigen::DontAlign> m_constraint_info;

    RigidBodyContactConstraint() = default;
    ~RigidBodyContactConstraint() = default;
    [[nodiscard]] virtual int &GetTypeId() const { return type_id; }

    bool InitConstraint(SimulationModel &model,
                        unsigned int rb_index_1,
                        unsigned int rb_index2,
                        const Vector3r &cp_1,
                        const Vector3r &cp2,
                        const Vector3r &normal,
                        Real dist,
                        Real restitution_coeff,
                        Real stiffness,
                        Real friction_coeff);
    virtual bool SolveVelocityConstraint(SimulationModel &model, unsigned int iter);
};

//MARK: - ==
struct ParticleRigidBodyContactConstraint {
public:
    static int type_id;
    /** indices of the linked bodies */
    std::array<unsigned int, 2> m_bodies{};
    Real m_stiffness{};
    Real m_friction_coeff{};
    Real m_sum_impulses{};
    Eigen::Matrix<Real, 3, 5, Eigen::DontAlign> m_constraint_info;

    ParticleRigidBodyContactConstraint() = default;
    ~ParticleRigidBodyContactConstraint() = default;
    [[nodiscard]] virtual int &GetTypeId() const { return type_id; }

    bool InitConstraint(SimulationModel &model,
                        unsigned int particle_index,
                        unsigned int rb_index,
                        const Vector3r &cp1,
                        const Vector3r &cp2,
                        const Vector3r &normal,
                        Real dist,
                        Real restitution_coeff,
                        Real stiffness,
                        Real friction_coeff);
    virtual bool SolveVelocityConstraint(SimulationModel &model, unsigned int iter);
};

//MARK: - ==
struct ParticleTetContactConstraint {
public:
    static int type_id;
    /** indices of the linked bodies */
    std::array<unsigned int, 2> m_bodies{};
    unsigned int m_solid_index{};
    unsigned int m_tet_index{};
    Vector3r m_bary;
    Real m_lambda{};
    Real m_friction_coeff{};
    Eigen::Matrix<Real, 3, 3, Eigen::DontAlign> m_constraint_info;
    Real m_inv_masses[4]{};
    std::array<Vector3r, 4> m_x;
    std::array<Vector3r, 4> m_v;

    ParticleTetContactConstraint() = default;
    ~ParticleTetContactConstraint() = default;
    [[nodiscard]] virtual int &GetTypeId() const { return type_id; }

    bool InitConstraint(SimulationModel &model,
                        unsigned int particle_index,
                        unsigned int solid_index,
                        unsigned int tet_index,
                        const Vector3r &bary,
                        const Vector3r &cp1,
                        const Vector3r &cp2,
                        const Vector3r &normal,
                        Real dist,
                        Real friction_coeff);
    virtual bool SolvePositionConstraint(SimulationModel &model, unsigned int iter);
    virtual bool SolveVelocityConstraint(SimulationModel &model, unsigned int iter);
};

//MARK: -
struct StretchShearConstraint : public Constraint {
public:
    static int type_id;
    Real m_rest_length{};
    Real m_shearing_stiffness_1{};
    Real m_shearing_stiffness_2{};
    Real m_stretching_stiffness{};

    StretchShearConstraint() : Constraint(3) {}
    [[nodiscard]] int &GetTypeId() const override { return type_id; }

    virtual bool InitConstraint(SimulationModel &model,
                                unsigned int particle1,
                                unsigned int particle2,
                                unsigned int quaternion1,
                                Real stretching_stiffness,
                                Real shearing_stiffness_1,
                                Real shearing_stiffness_2);
    bool SolvePositionConstraint(SimulationModel &model, unsigned int iter) override;
};

//MARK: -
struct BendTwistConstraint : public Constraint {
public:
    static int type_id;
    Quaternionr m_rest_darboux_vector;
    Real m_bending_stiffness_1{};
    Real m_bending_stiffness_2{};
    Real m_twisting_stiffness{};

    BendTwistConstraint() : Constraint(2) {}
    [[nodiscard]] int &GetTypeId() const override { return type_id; }

    virtual bool InitConstraint(SimulationModel &model,
                                unsigned int quaternion1,
                                unsigned int quaternion2,
                                Real twisting_stiffness,
                                Real bending_stiffness_1,
                                Real bending_stiffness_2);
    bool SolvePositionConstraint(SimulationModel &model, unsigned int iter) override;
};

//MARK: -
struct StretchBendingTwistingConstraint : public Constraint {
    using Matrix6r = Eigen::Matrix<Real, 6, 6, Eigen::DontAlign>;
    using Vector6r = Eigen::Matrix<Real, 6, 1, Eigen::DontAlign>;

public:
    static int type_id;
    Eigen::Matrix<Real, 3, 4, Eigen::DontAlign> m_constraint_info;

    Real m_average_radius{};
    Real m_average_segment_length{};
    Vector3r m_rest_darboux_vector;
    Vector3r m_stiffness_coefficient_k;
    Vector3r m_stretch_compliance;
    Vector3r m_bending_and_torsion_compliance;
    Vector6r m_lambda_sum;

    StretchBendingTwistingConstraint() : Constraint(2) {}

    [[nodiscard]] int &GetTypeId() const override { return type_id; }

    bool InitConstraint(SimulationModel &model,
                        unsigned int segment_index_1,
                        unsigned int segment_index_2,
                        const Vector3r &pos,
                        Real average_radius,
                        Real average_segment_length,
                        Real youngs_modulus,
                        Real torsion_modulus);
    bool InitConstraintBeforeProjection(SimulationModel &model) override;
    bool UpdateConstraint(SimulationModel &model) override;
    bool SolvePositionConstraint(SimulationModel &model, unsigned int iter) override;
};

//MARK: -
struct Node;
struct Interval;
struct SimulationModel;
using Vector6r = Eigen::Matrix<Real, 6, 1, Eigen::DontAlign>;

struct DirectPositionBasedSolverForStiffRodsConstraint : public Constraint {
    struct RodSegmentImpl : public RodSegment {
    public:
        RodSegmentImpl(SimulationModel &model, unsigned int idx) : m_model(model), m_segment_idx(idx){};

        bool IsDynamic() override;
        Real Mass() override;
        const Vector3r &InertiaTensor() override;
        const Vector3r &Position() override;
        const Quaternionr &Rotation() override;

        SimulationModel &m_model;
        unsigned int m_segment_idx;
    };

    struct RodConstraintImpl : public RodConstraint {
    public:
        std::vector<unsigned int> m_segments;
        Eigen::Matrix<Real, 3, 4, Eigen::DontAlign> m_constraint_info;

        Real m_average_radius;
        Real m_average_segment_length;
        Vector3r m_rest_darboux_vector;
        Vector3r m_stiffness_coefficient_k;
        Vector3r m_stretch_compliance;
        Vector3r m_bending_and_torsion_compliance;

        unsigned int SegmentIndex(unsigned int i) override {
            if (i < static_cast<unsigned int>(m_segments.size())) return m_segments[i];
            return 0u;
        };

        Eigen::Matrix<Real, 3, 4, Eigen::DontAlign> &GetConstraintInfo() override { return m_constraint_info; }
        Real GetAverageSegmentLength() override { return m_average_segment_length; }
        Vector3r &GetRestDarbouxVector() override { return m_rest_darboux_vector; }
        Vector3r &GetStiffnessCoefficientK() override { return m_stiffness_coefficient_k; };
        Vector3r &GetStretchCompliance() override { return m_stretch_compliance; }
        Vector3r &GetBendingAndTorsionCompliance() override { return m_bending_and_torsion_compliance; }
    };

public:
    static int type_id;

    DirectPositionBasedSolverForStiffRodsConstraint()
        : Constraint(2),
          root(nullptr),
          number_of_intervals(0),
          intervals(nullptr),
          forward(nullptr),
          backward(nullptr) {}
    ~DirectPositionBasedSolverForStiffRodsConstraint() override;

    [[nodiscard]] int &GetTypeId() const override { return type_id; }

    bool InitConstraint(SimulationModel &model,
                        const std::vector<std::pair<unsigned int, unsigned int>> &constraint_segment_indices,
                        const std::vector<Vector3r> &constraint_positions,
                        const std::vector<Real> &average_radii,
                        const std::vector<Real> &average_segment_lengths,
                        const std::vector<Real> &youngs_moduli,
                        const std::vector<Real> &torsion_moduli);

    bool InitConstraintBeforeProjection(SimulationModel &model) override;
    bool UpdateConstraint(SimulationModel &model) override;
    bool SolvePositionConstraint(SimulationModel &model, unsigned int iter) override;

protected:
    /** root node */
    Node *root;
    /** intervals of constraints */
    Interval *intervals;
    /** number of intervals */
    int number_of_intervals;
    /** list to process nodes with increasing row index in the system matrix H (from the leaves to the root) */
    std::list<Node *> *forward;
    /** list to process nodes starting with the highest row index to row index zero in the matrix H (from the root to
     * the leaves) */
    std::list<Node *> *backward;

    std::vector<RodConstraintImpl> m_constraints;
    std::vector<RodConstraint *> m_rod_constraints;

    std::vector<RodSegmentImpl> m_segments;
    std::vector<RodSegment *> m_rod_segments;

    std::vector<Vector6r> m_right_hand_side;
    std::vector<Vector6r> m_lambda_sums;
    std::vector<std::vector<Matrix3r>> m_bending_and_torsion_jacobians;
    std::vector<Vector3r> m_corr_x;
    std::vector<Quaternionr> m_corr_q;

    void DeleteNodes();
};
}  // namespace vox::compute
