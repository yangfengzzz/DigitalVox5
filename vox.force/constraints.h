//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.force/common.h"
#include <array>
#include <cmath>
#include <list>
#include <memory>
#include <vector>

#include "vox.force/direct_position_based_solver_for_stiff_rods_interface.h"

namespace vox::force {
struct SimulationModel;

struct Constraint {
public:
    /** indices of the linked bodies */
    std::vector<unsigned int> m_bodies;

    explicit Constraint(const unsigned int numberOfBodies) { m_bodies.resize(numberOfBodies); }

    [[nodiscard]] unsigned int numberOfBodies() const { return static_cast<unsigned int>(m_bodies.size()); }
    virtual ~Constraint() = default;
    [[nodiscard]] virtual int &getTypeId() const = 0;

    virtual bool initConstraintBeforeProjection(SimulationModel &model) { return true; };
    virtual bool updateConstraint(SimulationModel &model) { return true; };
    virtual bool solvePositionConstraint(SimulationModel &model, const unsigned int iter) { return true; };
    virtual bool solveVelocityConstraint(SimulationModel &model, const unsigned int iter) { return true; };
};

struct BallJoint : public Constraint {
public:
    static int type_id;
    Eigen::Matrix<Real, 3, 4, Eigen::DontAlign> m_jointInfo;

    BallJoint() : Constraint(2) {}
    [[nodiscard]] int &getTypeId() const override { return type_id; }

    bool initConstraint(SimulationModel &model,
                        unsigned int rbIndex1,
                        unsigned int rbIndex2,
                        const Vector3r &pos);
    bool updateConstraint(SimulationModel &model) override;
    bool solvePositionConstraint(SimulationModel &model, unsigned int iter) override;
};

struct BallOnLineJoint : public Constraint {
public:
    static int type_id;
    Eigen::Matrix<Real, 3, 10, Eigen::DontAlign> m_jointInfo;

    BallOnLineJoint() : Constraint(2) {}
    [[nodiscard]] int &getTypeId() const override { return type_id; }

    bool initConstraint(SimulationModel &model,
                        unsigned int rbIndex1,
                        unsigned int rbIndex2,
                        const Vector3r &pos,
                        const Vector3r &dir);
    bool updateConstraint(SimulationModel &model) override;
    bool solvePositionConstraint(SimulationModel &model, unsigned int iter) override;
};

struct HingeJoint : public Constraint {
public:
    static int type_id;
    Eigen::Matrix<Real, 4, 7, Eigen::DontAlign> m_jointInfo;

    HingeJoint() : Constraint(2) {}
    [[nodiscard]] int &getTypeId() const override { return type_id; }

    bool initConstraint(SimulationModel &model,
                        unsigned int rbIndex1,
                        unsigned int rbIndex2,
                        const Vector3r &pos,
                        const Vector3r &axis);
    bool updateConstraint(SimulationModel &model) override;
    bool solvePositionConstraint(SimulationModel &model, unsigned int iter) override;
};

struct UniversalJoint : public Constraint {
public:
    static int type_id;
    Eigen::Matrix<Real, 3, 8, Eigen::DontAlign> m_jointInfo;

    UniversalJoint() : Constraint(2) {}
    [[nodiscard]] int &getTypeId() const override { return type_id; }

    bool initConstraint(SimulationModel &model,
                        unsigned int rbIndex1,
                        unsigned int rbIndex2,
                        const Vector3r &pos,
                        const Vector3r &axis1,
                        const Vector3r &axis2);
    bool updateConstraint(SimulationModel &model) override;
    bool solvePositionConstraint(SimulationModel &model, unsigned int iter) override;
};

struct SliderJoint : public Constraint {
public:
    static int type_id;
    Eigen::Matrix<Real, 4, 6, Eigen::DontAlign> m_jointInfo;

    SliderJoint() : Constraint(2) {}
    [[nodiscard]] int &getTypeId() const override { return type_id; }

    bool initConstraint(SimulationModel &model,
                        unsigned int rbIndex1,
                        unsigned int rbIndex2,
                        const Vector3r &axis);
    bool updateConstraint(SimulationModel &model) override;
    bool solvePositionConstraint(SimulationModel &model, unsigned int iter) override;
};

struct MotorJoint : public Constraint {
public:
    Real m_target;
    std::vector<Real> m_targetSequence;
    MotorJoint() : Constraint(2) { m_target = 0.0; }

    [[nodiscard]] virtual Real getTarget() const { return m_target; }
    virtual void setTarget(const Real val) { m_target = val; }

    virtual std::vector<Real> &getTargetSequence() { return m_targetSequence; }
    virtual void setTargetSequence(const std::vector<Real> &val) { m_targetSequence = val; }

    [[nodiscard]] bool getRepeatSequence() const { return m_repeatSequence; }
    void setRepeatSequence(bool val) { m_repeatSequence = val; }

private:
    bool m_repeatSequence{};
};

struct TargetPositionMotorSliderJoint : public MotorJoint {
public:
    static int type_id;
    Eigen::Matrix<Real, 4, 6, Eigen::DontAlign> m_jointInfo;

    TargetPositionMotorSliderJoint() : MotorJoint() {}
    [[nodiscard]] int &getTypeId() const override { return type_id; }

    bool initConstraint(SimulationModel &model,
                        unsigned int rbIndex1,
                        unsigned int rbIndex2,
                        const Vector3r &axis);
    bool updateConstraint(SimulationModel &model) override;
    bool solvePositionConstraint(SimulationModel &model, unsigned int iter) override;
};

struct TargetVelocityMotorSliderJoint : public MotorJoint {
public:
    static int type_id;
    Eigen::Matrix<Real, 4, 6, Eigen::DontAlign> m_jointInfo;

    TargetVelocityMotorSliderJoint() : MotorJoint() {}
    [[nodiscard]] int &getTypeId() const override { return type_id; }

    bool initConstraint(SimulationModel &model,
                        unsigned int rbIndex1,
                        unsigned int rbIndex2,
                        const Vector3r &axis);
    bool updateConstraint(SimulationModel &model) override;
    bool solvePositionConstraint(SimulationModel &model, unsigned int iter) override;
    bool solveVelocityConstraint(SimulationModel &model, unsigned int iter) override;
};

struct TargetAngleMotorHingeJoint : public MotorJoint {
public:
    static int type_id;
    Eigen::Matrix<Real, 4, 8, Eigen::DontAlign> m_jointInfo;
    TargetAngleMotorHingeJoint() : MotorJoint() {}
    [[nodiscard]] int &getTypeId() const override { return type_id; }

    void setTarget(const Real val) override {
        const Real pi = (Real)M_PI;
        m_target = std::max(val, -pi);
        m_target = std::min(m_target, pi);
    }

    bool initConstraint(SimulationModel &model,
                        unsigned int rbIndex1,
                        unsigned int rbIndex2,
                        const Vector3r &pos,
                        const Vector3r &axis);
    bool updateConstraint(SimulationModel &model) override;
    bool solvePositionConstraint(SimulationModel &model, unsigned int iter) override;

private:
    std::vector<Real> m_targetSequence;
};

struct TargetVelocityMotorHingeJoint : public MotorJoint {
public:
    static int type_id;
    Eigen::Matrix<Real, 4, 8, Eigen::DontAlign> m_jointInfo;
    TargetVelocityMotorHingeJoint() : MotorJoint() {}
    [[nodiscard]] int &getTypeId() const override { return type_id; }

    bool initConstraint(SimulationModel &model,
                        unsigned int rbIndex1,
                        unsigned int rbIndex2,
                        const Vector3r &pos,
                        const Vector3r &axis);
    bool updateConstraint(SimulationModel &model) override;
    bool solvePositionConstraint(SimulationModel &model, unsigned int iter) override;
    bool solveVelocityConstraint(SimulationModel &model, unsigned int iter) override;
};

struct DamperJoint : public Constraint {
public:
    static int type_id;
    Real m_stiffness{};
    Eigen::Matrix<Real, 4, 6, Eigen::DontAlign> m_jointInfo;
    Real m_lambda{};

    DamperJoint() : Constraint(2) {}
    [[nodiscard]] int &getTypeId() const override { return type_id; }

    bool initConstraint(SimulationModel &model,
                        unsigned int rbIndex1,
                        unsigned int rbIndex2,
                        const Vector3r &axis,
                        Real stiffness);
    bool updateConstraint(SimulationModel &model) override;
    bool solvePositionConstraint(SimulationModel &model, unsigned int iter) override;
};

struct RigidBodyParticleBallJoint : public Constraint {
public:
    static int type_id;
    Eigen::Matrix<Real, 3, 2, Eigen::DontAlign> m_jointInfo;

    RigidBodyParticleBallJoint() : Constraint(2) {}
    [[nodiscard]] int &getTypeId() const override { return type_id; }

    bool initConstraint(SimulationModel &model, unsigned int rbIndex, unsigned int particleIndex);
    bool updateConstraint(SimulationModel &model) override;
    bool solvePositionConstraint(SimulationModel &model, unsigned int iter) override;
};

struct RigidBodySpring : public Constraint {
public:
    static int type_id;
    Eigen::Matrix<Real, 3, 4, Eigen::DontAlign> m_jointInfo;
    Real m_restLength{};
    Real m_stiffness{};
    Real m_lambda{};

    RigidBodySpring() : Constraint(2) {}
    [[nodiscard]] int &getTypeId() const override { return type_id; }

    bool initConstraint(SimulationModel &model,
                        unsigned int rbIndex1,
                        unsigned int rbIndex2,
                        const Vector3r &pos1,
                        const Vector3r &pos2,
                        Real stiffness);
    bool updateConstraint(SimulationModel &model) override;
    bool solvePositionConstraint(SimulationModel &model, unsigned int iter) override;
};

struct DistanceJoint : public Constraint {
public:
    static int type_id;
    Eigen::Matrix<Real, 3, 4, Eigen::DontAlign> m_jointInfo;
    Real m_restLength{};

    DistanceJoint() : Constraint(2) {}
    [[nodiscard]] int &getTypeId() const override { return type_id; }

    bool initConstraint(SimulationModel &model,
                        unsigned int rbIndex1,
                        unsigned int rbIndex2,
                        const Vector3r &pos1,
                        const Vector3r &pos2);
    bool updateConstraint(SimulationModel &model) override;
    bool solvePositionConstraint(SimulationModel &model, unsigned int iter) override;
};

struct DistanceConstraint : public Constraint {
public:
    static int type_id;
    Real m_restLength{};
    Real m_stiffness{};

    DistanceConstraint() : Constraint(2) {}
    [[nodiscard]] int &getTypeId() const override { return type_id; }

    virtual bool initConstraint(SimulationModel &model,
                                unsigned int particle1,
                                unsigned int particle2,
                                Real stiffness);
    bool solvePositionConstraint(SimulationModel &model, unsigned int iter) override;
};

struct DistanceConstraint_XPBD : public Constraint {
public:
    static int type_id;
    Real m_restLength{};
    Real m_lambda{};
    Real m_stiffness{};

    DistanceConstraint_XPBD() : Constraint(2) {}
    [[nodiscard]] int &getTypeId() const override { return type_id; }

    virtual bool initConstraint(SimulationModel &model,
                                unsigned int particle1,
                                unsigned int particle2,
                                Real stiffness);
    bool solvePositionConstraint(SimulationModel &model, unsigned int iter) override;
};

struct DihedralConstraint : public Constraint {
public:
    static int type_id;
    Real m_restAngle{};
    Real m_stiffness{};

    DihedralConstraint() : Constraint(4) {}
    [[nodiscard]] int &getTypeId() const override { return type_id; }

    virtual bool initConstraint(SimulationModel &model,
                                unsigned int particle1,
                                unsigned int particle2,
                                unsigned int particle3,
                                unsigned int particle4,
                                Real stiffness);
    bool solvePositionConstraint(SimulationModel &model, unsigned int iter) override;
};

struct IsometricBendingConstraint : public Constraint {
public:
    static int type_id;
    Real m_stiffness{};
    Matrix4r m_Q;

    IsometricBendingConstraint() : Constraint(4) {}
    [[nodiscard]] int &getTypeId() const override { return type_id; }

    virtual bool initConstraint(SimulationModel &model,
                                unsigned int particle1,
                                unsigned int particle2,
                                unsigned int particle3,
                                unsigned int particle4,
                                Real stiffness);
    bool solvePositionConstraint(SimulationModel &model, unsigned int iter) override;
};

struct IsometricBendingConstraint_XPBD : public Constraint {
public:
    static int type_id;
    Real m_stiffness{};
    Matrix4r m_Q;
    Real m_lambda{};

    IsometricBendingConstraint_XPBD() : Constraint(4) {}
    [[nodiscard]] int &getTypeId() const override { return type_id; }

    virtual bool initConstraint(SimulationModel &model,
                                unsigned int particle1,
                                unsigned int particle2,
                                unsigned int particle3,
                                unsigned int particle4,
                                Real stiffness);
    bool solvePositionConstraint(SimulationModel &model, unsigned int iter) override;
};

struct FEMTriangleConstraint : public Constraint {
public:
    static int type_id;
    Real m_area{};
    Matrix2r m_invRestMat;
    Real m_xxStiffness{};
    Real m_xyStiffness{};
    Real m_yyStiffness{};
    Real m_xyPoissonRatio{};
    Real m_yxPoissonRatio{};

    FEMTriangleConstraint() : Constraint(3) {}
    [[nodiscard]] int &getTypeId() const override { return type_id; }

    virtual bool initConstraint(SimulationModel &model,
                                unsigned int particle1,
                                unsigned int particle2,
                                unsigned int particle3,
                                Real xxStiffness,
                                Real yyStiffness,
                                Real xyStiffness,
                                Real xyPoissonRatio,
                                Real yxPoissonRatio);
    bool solvePositionConstraint(SimulationModel &model, unsigned int iter) override;
};

struct StrainTriangleConstraint : public Constraint {
public:
    static int type_id;
    Matrix2r m_invRestMat;
    Real m_xxStiffness{};
    Real m_xyStiffness{};
    Real m_yyStiffness{};
    bool m_normalizeStretch{};
    bool m_normalizeShear{};

    StrainTriangleConstraint() : Constraint(3) {}
    [[nodiscard]] int &getTypeId() const override { return type_id; }

    virtual bool initConstraint(SimulationModel &model,
                                unsigned int particle1,
                                unsigned int particle2,
                                unsigned int particle3,
                                Real xxStiffness,
                                Real yyStiffness,
                                Real xyStiffness,
                                bool normalizeStretch,
                                bool normalizeShear);
    bool solvePositionConstraint(SimulationModel &model, unsigned int iter) override;
};

struct VolumeConstraint : public Constraint {
public:
    static int type_id;
    Real m_stiffness{};
    Real m_restVolume{};

    VolumeConstraint() : Constraint(4) {}
    [[nodiscard]] int &getTypeId() const override { return type_id; }

    virtual bool initConstraint(SimulationModel &model,
                                unsigned int particle1,
                                unsigned int particle2,
                                unsigned int particle3,
                                unsigned int particle4,
                                Real stiffness);
    bool solvePositionConstraint(SimulationModel &model, unsigned int iter) override;
};

struct VolumeConstraint_XPBD : public Constraint {
public:
    static int type_id;
    Real m_stiffness{};
    Real m_restVolume{};
    Real m_lambda{};

    VolumeConstraint_XPBD() : Constraint(4) {}
    [[nodiscard]] int &getTypeId() const override { return type_id; }

    virtual bool initConstraint(SimulationModel &model,
                                unsigned int particle1,
                                unsigned int particle2,
                                unsigned int particle3,
                                unsigned int particle4,
                                Real stiffness);
    bool solvePositionConstraint(SimulationModel &model, unsigned int iter) override;
};

struct FEMTetConstraint : public Constraint {
public:
    static int type_id;
    Real m_stiffness{};
    Real m_poissonRatio{};
    Real m_volume{};
    Matrix3r m_invRestMat;

    FEMTetConstraint() : Constraint(4) {}
    [[nodiscard]] int &getTypeId() const override { return type_id; }

    virtual bool initConstraint(SimulationModel &model,
                                unsigned int particle1,
                                unsigned int particle2,
                                unsigned int particle3,
                                unsigned int particle4,
                                Real stiffness,
                                Real poissonRatio);
    bool solvePositionConstraint(SimulationModel &model, unsigned int iter) override;
};

struct StrainTetConstraint : public Constraint {
public:
    static int type_id;
    Real m_stretchStiffness{};
    Real m_shearStiffness{};
    Matrix3r m_invRestMat;
    bool m_normalizeStretch{};
    bool m_normalizeShear{};

    StrainTetConstraint() : Constraint(4) {}
    [[nodiscard]] int &getTypeId() const override { return type_id; }

    virtual bool initConstraint(SimulationModel &model,
                                unsigned int particle1,
                                unsigned int particle2,
                                unsigned int particle3,
                                unsigned int particle4,
                                Real stretchStiffness,
                                Real shearStiffness,
                                bool normalizeStretch,
                                bool normalizeShear);
    bool solvePositionConstraint(SimulationModel &model, unsigned int iter) override;
};

struct ShapeMatchingConstraint : public Constraint {
public:
    static int type_id;
    Real m_stiffness{};
    Vector3r m_restCm;
    Real *m_w;
    Vector3r *m_x0;
    Vector3r *m_x;
    Vector3r *m_corr;
    unsigned int *m_numClusters;

    explicit ShapeMatchingConstraint(const unsigned int numberOfParticles) : Constraint(numberOfParticles) {
        m_x = new Vector3r[numberOfParticles];
        m_x0 = new Vector3r[numberOfParticles];
        m_corr = new Vector3r[numberOfParticles];
        m_w = new Real[numberOfParticles];
        m_numClusters = new unsigned int[numberOfParticles];
    }
    ~ShapeMatchingConstraint() override {
        delete[] m_x;
        delete[] m_x0;
        delete[] m_corr;
        delete[] m_w;
        delete[] m_numClusters;
    }
    [[nodiscard]] int &getTypeId() const override { return type_id; }

    virtual bool initConstraint(SimulationModel &model,
                                const unsigned int particleIndices[],
                                const unsigned int numClusters[],
                                Real stiffness);
    bool solvePositionConstraint(SimulationModel &model, unsigned int iter) override;
};

struct RigidBodyContactConstraint {
public:
    static int type_id;
    /** indices of the linked bodies */
    std::array<unsigned int, 2> m_bodies{};
    Real m_stiffness{};
    Real m_frictionCoeff{};
    Real m_sum_impulses{};
    Eigen::Matrix<Real, 3, 5, Eigen::DontAlign> m_constraintInfo;

    RigidBodyContactConstraint() = default;
    ~RigidBodyContactConstraint() = default;
    [[nodiscard]] virtual int &getTypeId() const { return type_id; }

    bool initConstraint(SimulationModel &model,
                        unsigned int rbIndex1,
                        unsigned int rbIndex2,
                        const Vector3r &cp1,
                        const Vector3r &cp2,
                        const Vector3r &normal,
                        Real dist,
                        Real restitutionCoeff,
                        Real stiffness,
                        Real frictionCoeff);
    virtual bool solveVelocityConstraint(SimulationModel &model, unsigned int iter);
};

struct ParticleRigidBodyContactConstraint {
public:
    static int type_id;
    /** indices of the linked bodies */
    std::array<unsigned int, 2> m_bodies{};
    Real m_stiffness{};
    Real m_frictionCoeff{};
    Real m_sum_impulses{};
    Eigen::Matrix<Real, 3, 5, Eigen::DontAlign> m_constraintInfo;

    ParticleRigidBodyContactConstraint() = default;
    ~ParticleRigidBodyContactConstraint() = default;
    [[nodiscard]] virtual int &getTypeId() const { return type_id; }

    bool initConstraint(SimulationModel &model,
                        unsigned int particleIndex,
                        unsigned int rbIndex,
                        const Vector3r &cp1,
                        const Vector3r &cp2,
                        const Vector3r &normal,
                        Real dist,
                        Real restitutionCoeff,
                        Real stiffness,
                        Real frictionCoeff);
    virtual bool solveVelocityConstraint(SimulationModel &model, unsigned int iter);
};

struct ParticleTetContactConstraint {
public:
    static int type_id;
    /** indices of the linked bodies */
    std::array<unsigned int, 2> m_bodies{};
    unsigned int m_solidIndex{};
    unsigned int m_tetIndex{};
    Vector3r m_bary;
    Real m_lambda{};
    Real m_frictionCoeff{};
    Eigen::Matrix<Real, 3, 3, Eigen::DontAlign> m_constraintInfo;
    Real m_invMasses[4]{};
    std::array<Vector3r, 4> m_x;
    std::array<Vector3r, 4> m_v;

    ParticleTetContactConstraint() = default;
    ~ParticleTetContactConstraint() = default;
    [[nodiscard]] virtual int &getTypeId() const { return type_id; }

    bool initConstraint(SimulationModel &model,
                        unsigned int particleIndex,
                        unsigned int solidIndex,
                        unsigned int tetindex,
                        const Vector3r &bary,
                        const Vector3r &cp1,
                        const Vector3r &cp2,
                        const Vector3r &normal,
                        Real dist,
                        Real frictionCoeff);
    virtual bool solvePositionConstraint(SimulationModel &model, unsigned int iter);
    virtual bool solveVelocityConstraint(SimulationModel &model, unsigned int iter);
};

struct StretchShearConstraint : public Constraint {
public:
    static int type_id;
    Real m_restLength{};
    Real m_shearingStiffness1{};
    Real m_shearingStiffness2{};
    Real m_stretchingStiffness{};

    StretchShearConstraint() : Constraint(3) {}
    [[nodiscard]] int &getTypeId() const override { return type_id; }

    virtual bool initConstraint(SimulationModel &model,
                                unsigned int particle1,
                                unsigned int particle2,
                                unsigned int quaternion1,
                                Real stretchingStiffness,
                                Real shearingStiffness1,
                                Real shearingStiffness2);
    bool solvePositionConstraint(SimulationModel &model, unsigned int iter) override;
};

struct BendTwistConstraint : public Constraint {
public:
    static int type_id;
    Quaternionr m_restDarbouxVector;
    Real m_bendingStiffness1{};
    Real m_bendingStiffness2{};
    Real m_twistingStiffness{};

    BendTwistConstraint() : Constraint(2) {}
    [[nodiscard]] int &getTypeId() const override { return type_id; }

    virtual bool initConstraint(SimulationModel &model,
                                unsigned int quaternion1,
                                unsigned int quaternion2,
                                Real twistingStiffness,
                                Real bendingStiffness1,
                                Real bendingStiffness2);
    bool solvePositionConstraint(SimulationModel &model, unsigned int iter) override;
};

struct StretchBendingTwistingConstraint : public Constraint {
    using Matrix6r = Eigen::Matrix<Real, 6, 6, Eigen::DontAlign>;
    using Vector6r = Eigen::Matrix<Real, 6, 1, Eigen::DontAlign>;

public:
    static int type_id;
    Eigen::Matrix<Real, 3, 4, Eigen::DontAlign> m_constraintInfo;

    Real m_averageRadius{};
    Real m_averageSegmentLength{};
    Vector3r m_restDarbouxVector;
    Vector3r m_stiffnessCoefficientK;
    Vector3r m_stretchCompliance;
    Vector3r m_bendingAndTorsionCompliance;
    Vector6r m_lambdaSum;

    StretchBendingTwistingConstraint() : Constraint(2) {}

    [[nodiscard]] int &getTypeId() const override { return type_id; }

    bool initConstraint(SimulationModel &model,
                        unsigned int segmentIndex1,
                        unsigned int segmentIndex2,
                        const Vector3r &pos,
                        Real averageRadius,
                        Real averageSegmentLength,
                        Real youngsModulus,
                        Real torsionModulus);
    bool initConstraintBeforeProjection(SimulationModel &model) override;
    bool updateConstraint(SimulationModel &model) override;
    bool solvePositionConstraint(SimulationModel &model, unsigned int iter) override;
};

struct Node;
struct Interval;
struct SimulationModel;
using Vector6r = Eigen::Matrix<Real, 6, 1, Eigen::DontAlign>;

struct DirectPositionBasedSolverForStiffRodsConstraint : public Constraint {
    struct RodSegmentImpl : public RodSegment {
    public:
        RodSegmentImpl(SimulationModel &model, unsigned int idx) : m_model(model), m_segmentIdx(idx){};

        bool IsDynamic() override;
        Real Mass() override;
        const Vector3r &InertiaTensor() override;
        const Vector3r &Position() override;
        const Quaternionr &Rotation() override;

        SimulationModel &m_model;
        unsigned int m_segmentIdx;
    };

    struct RodConstraintImpl : public RodConstraint {
    public:
        std::vector<unsigned int> m_segments;
        Eigen::Matrix<Real, 3, 4, Eigen::DontAlign> m_constraintInfo;

        Real m_averageRadius;
        Real m_averageSegmentLength;
        Vector3r m_restDarbouxVector;
        Vector3r m_stiffnessCoefficientK;
        Vector3r m_stretchCompliance;
        Vector3r m_bendingAndTorsionCompliance;

        unsigned int SegmentIndex(unsigned int i) override {
            if (i < static_cast<unsigned int>(m_segments.size())) return m_segments[i];
            return 0u;
        };

        Eigen::Matrix<Real, 3, 4, Eigen::DontAlign> &GetConstraintInfo() override { return m_constraintInfo; }
        Real GetAverageSegmentLength() override { return m_averageSegmentLength; }
        Vector3r &GetRestDarbouxVector() override { return m_restDarbouxVector; }
        Vector3r &GetStiffnessCoefficientK() override { return m_stiffnessCoefficientK; };
        Vector3r &GetStretchCompliance() override { return m_stretchCompliance; }
        Vector3r &GetBendingAndTorsionCompliance() override { return m_bendingAndTorsionCompliance; }
    };

public:
    static int type_id;

    DirectPositionBasedSolverForStiffRodsConstraint()
        : Constraint(2), root(nullptr), numberOfIntervals(0), intervals(nullptr), forward(nullptr), backward(nullptr) {}
    ~DirectPositionBasedSolverForStiffRodsConstraint() override;

    [[nodiscard]] int &getTypeId() const override { return type_id; }

    bool initConstraint(SimulationModel &model,
                        const std::vector<std::pair<unsigned int, unsigned int>> &constraintSegmentIndices,
                        const std::vector<Vector3r> &constraintPositions,
                        const std::vector<Real> &averageRadii,
                        const std::vector<Real> &averageSegmentLengths,
                        const std::vector<Real> &youngsModuli,
                        const std::vector<Real> &torsionModuli);

    bool initConstraintBeforeProjection(SimulationModel &model) override;
    bool updateConstraint(SimulationModel &model) override;
    bool solvePositionConstraint(SimulationModel &model, unsigned int iter) override;

protected:
    /** root node */
    Node *root;
    /** intervals of constraints */
    Interval *intervals;
    /** number of intervals */
    int numberOfIntervals;
    /** list to process nodes with increasing row index in the system matrix H (from the leaves to the root) */
    std::list<Node *> *forward;
    /** list to process nodes starting with the highest row index to row index zero in the matrix H (from the root to
     * the leaves) */
    std::list<Node *> *backward;

    std::vector<RodConstraintImpl> m_Constraints;
    std::vector<RodConstraint *> m_rodConstraints;

    std::vector<RodSegmentImpl> m_Segments;
    std::vector<RodSegment *> m_rodSegments;

    std::vector<Vector6r> m_rightHandSide;
    std::vector<Vector6r> m_lambdaSums;
    std::vector<std::vector<Matrix3r>> m_bendingAndTorsionJacobians;
    std::vector<Vector3r> m_corr_x;
    std::vector<Quaternionr> m_corr_q;

    void deleteNodes();
};
}  // namespace vox::force