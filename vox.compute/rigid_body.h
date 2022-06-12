//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <vector>

#include "vox.compute/common.h"
#include "vox.compute/rigid_body_geometry.h"
#include "vox.compute/volume_integration.h"

namespace vox::compute {
/** This class encapsulates the state of a rigid body.
 */
class RigidBody {
private:
    /** mass */
    Real m_mass_{};
    /** inverse mass */
    Real m_inv_mass_{};
    /** center of mass */
    Vector3r m_x_;
    Vector3r m_last_x_;
    Vector3r m_old_x_;
    Vector3r m_x_0_;
    /** center of mass velocity */
    Vector3r m_v_;
    Vector3r m_v_0_;
    /** acceleration (by external forces) */
    Vector3r m_a_;

    /** Inertia tensor in the principal axis system: \n
     * After the main axis transformation the inertia tensor is a diagonal matrix.
     * So only three values are required to store the inertia tensor. These values
     * are constant over time.
     */
    Vector3r m_inertia_tensor_;
    /** 3x3 matrix, inertia tensor in world space */
    Matrix3r m_inertia_tensor_w_;
    /** Inverse inertia tensor in body space */
    Vector3r m_inertia_tensor_inverse_;
    /** 3x3 matrix, inverse of the inertia tensor in world space */
    Matrix3r m_inertia_tensor_inverse_w_;
    /** Quaternion that describes the rotation of the body in world space */
    Quaternionr m_q_;
    Quaternionr m_last_q_;
    Quaternionr m_old_q_;
    Quaternionr m_q_0_;
    /** Quaternion representing the rotation of the main axis transformation
    that is performed to get a diagonal inertia tensor */
    Quaternionr m_q_mat_;
    /** Quaternion representing the initial rotation of the geometry */
    Quaternionr m_q_initial_;
    /** difference of the initial translation and the translation of the main axis transformation */
    Vector3r m_x_0_mat_;
    /** rotationMatrix = 3x3 matrix.
     * Important for the transformation from world in body space and vice versa.
     * When using quaternions the rotation matrix is computed out of the quaternion.
     */
    Matrix3r m_rot_;
    /** Angular velocity, defines rotation axis and velocity (magnitude of the vector) */
    Vector3r m_omega_;
    Vector3r m_omega_0_;
    /** external torque */
    Vector3r m_torque_;

    Real m_restitution_coeff_{};
    Real m_friction_coeff_{};

    RigidBodyGeometry m_geometry_;

    // transformation required to transform a point to local space or vice vera
    Matrix3r m_transformation_r_;
    Vector3r m_transformation_v_1_;
    Vector3r m_transformation_v_2_;
    Vector3r m_transformation_r_x_v_1_;

public:
    RigidBody() = default;

    ~RigidBody() = default;

    void InitBody(const Real mass,
                  const Vector3r &x,
                  const Vector3r &inertia_tensor,
                  const Quaternionr &rotation,
                  const VertexData &vertices,
                  const IndexedFaceMesh &mesh,
                  const Vector3r &scale = Vector3r(1.0, 1.0, 1.0)) {
        SetMass(mass);
        m_x_ = x;
        m_x_0_ = x;
        m_last_x_ = x;
        m_old_x_ = x;
        m_v_.setZero();
        m_v_0_.setZero();
        m_a_.setZero();

        SetInertiaTensor(inertia_tensor);
        m_q_ = rotation;
        m_q_0_ = rotation;
        m_last_q_ = rotation;
        m_old_q_ = rotation;
        m_rot_ = m_q_.matrix();
        m_q_mat_ = Quaternionr(1.0, 0.0, 0.0, 0.0);
        m_q_initial_ = Quaternionr(1.0, 0.0, 0.0, 0.0);
        m_x_0_mat_.setZero();
        RotationUpdated();
        m_omega_.setZero();
        m_omega_0_.setZero();
        m_torque_.setZero();

        m_restitution_coeff_ = static_cast<Real>(0.6);
        m_friction_coeff_ = static_cast<Real>(0.2);

        GetGeometry().InitMesh(vertices.Size(), mesh.NumFaces(), &vertices.GetPosition(0), mesh.GetFaces().data(),
                               mesh.GetUvIndices(), mesh.GetUVs(), scale, mesh.GetFlatShading());
        GetGeometry().UpdateMeshTransformation(GetPosition(), GetRotationMatrix());
    }

    void InitBody(const Real density,
                  const Vector3r &x,
                  const Quaternionr &rotation,
                  const VertexData &vertices,
                  const IndexedFaceMesh &mesh,
                  const Vector3r &scale = Vector3r(1.0, 1.0, 1.0)) {
        m_mass_ = 1.0;
        m_inertia_tensor_ = Vector3r(1.0, 1.0, 1.0);
        m_x_ = x;
        m_x_0_ = x;
        m_last_x_ = x;
        m_old_x_ = x;
        m_v_.setZero();
        m_v_0_.setZero();
        m_a_.setZero();

        m_q_ = rotation;
        m_q_0_ = rotation;
        m_last_q_ = rotation;
        m_old_q_ = rotation;
        m_rot_ = m_q_.matrix();
        RotationUpdated();
        m_omega_.setZero();
        m_omega_0_.setZero();
        m_torque_.setZero();

        m_restitution_coeff_ = static_cast<Real>(0.6);
        m_friction_coeff_ = static_cast<Real>(0.2);

        GetGeometry().InitMesh(vertices.Size(), mesh.NumFaces(), &vertices.GetPosition(0), mesh.GetFaces().data(),
                               mesh.GetUvIndices(), mesh.GetUVs(), scale, mesh.GetFlatShading());
        DetermineMassProperties(density);
        GetGeometry().UpdateMeshTransformation(GetPosition(), GetRotationMatrix());
    }

    void Reset() {
        GetPosition() = GetPosition0();
        GetOldPosition() = GetPosition0();
        GetLastPosition() = GetPosition0();

        GetRotation() = GetRotation0();
        GetOldRotation() = GetRotation0();
        GetLastRotation() = GetRotation0();

        GetVelocity() = GetVelocity0();
        GetAngularVelocity() = GetAngularVelocity0();

        GetAcceleration().setZero();
        GetTorque().setZero();

        RotationUpdated();
    }

    void UpdateInverseTransformation() {
        // remove the rotation of the main axis transformation that is performed
        // to get a diagonal inertia tensor since the distance function is
        // evaluated in local coordinates
        //
        // transformation world to local:
        // p_local = R_initial^T ( R_MAT R^T (p_world - x) - x_initial + x_MAT)
        //
        // transformation local to world:
        // p_world = R R_MAT^T (R_initial p_local + x_initial - x_MAT) + x
        //
        m_transformation_r_ = (GetRotationInitial().inverse() * GetRotationMat() * GetRotation().inverse()).matrix();
        m_transformation_v_1_ = -GetRotationInitial().inverse().matrix() * GetPositionInitialMat();
        m_transformation_v_2_ =
                (GetRotation() * GetRotationMat().inverse()).matrix() * GetPositionInitialMat() + GetPosition();
        m_transformation_r_x_v_1_ = -m_transformation_r_ * GetPosition() + m_transformation_v_1_;
    }

    void RotationUpdated() {
        if (m_mass_ != 0.0) {
            m_rot_ = m_q_.matrix();
            UpdateInertiaW();
            UpdateInverseTransformation();
        }
    }

    void UpdateInertiaW() {
        if (m_mass_ != 0.0) {
            m_inertia_tensor_w_ = m_rot_ * m_inertia_tensor_.asDiagonal() * m_rot_.transpose();
            m_inertia_tensor_inverse_w_ = m_rot_ * m_inertia_tensor_inverse_.asDiagonal() * m_rot_.transpose();
        }
    }

    /** Determine mass and inertia tensor of the given geometry.
     */
    void DetermineMassProperties(const Real density) {
        // apply initial rotation
        VertexData &vd = m_geometry_.GetVertexDataLocal();

        VolumeIntegration vi(m_geometry_.GetVertexDataLocal().Size(), m_geometry_.GetMesh().NumFaces(),
                             &m_geometry_.GetVertexDataLocal().GetPosition(0), m_geometry_.GetMesh().GetFaces().data());
        vi.ComputeInertiaTensor(density);

        // Diagonalize Inertia Tensor
        Eigen::SelfAdjointEigenSolver<Matrix3r> es(vi.GetInertia());
        Vector3r inertia_tensor = es.eigenvalues();
        Matrix3r r = es.eigenvectors();

        SetMass(vi.GetMass());
        SetInertiaTensor(inertia_tensor);

        if (r.determinant() < 0.0) r = -r;

        for (unsigned int i = 0; i < vd.Size(); i++) vd.GetPosition(i) = m_rot_ * vd.GetPosition(i) + m_x_0_;

        Vector3r x_mat = vi.GetCenterOfMass();
        r = m_rot_ * r;
        x_mat = m_rot_ * x_mat + m_x_0_;

        // rotate vertices back
        for (unsigned int i = 0; i < vd.Size(); i++) vd.GetPosition(i) = r.transpose() * (vd.GetPosition(i) - x_mat);

        // set rotation
        auto q_r = Quaternionr(r);
        q_r.normalize();
        m_q_mat_ = q_r;
        m_q_initial_ = m_q_0_;
        m_x_0_mat_ = m_x_0_ - x_mat;

        m_q_0_ = q_r;
        m_q_ = m_q_0_;
        m_last_q_ = m_q_0_;
        m_old_q_ = m_q_0_;
        RotationUpdated();

        // set translation
        m_x_0_ = x_mat;
        m_x_ = m_x_0_;
        m_last_x_ = m_x_0_;
        m_old_x_ = m_x_0_;
        UpdateInverseTransformation();
    }

    const Matrix3r &GetTransformationR() { return m_transformation_r_; }
    const Vector3r &GetTransformationV1() { return m_transformation_v_1_; }
    const Vector3r &GetTransformationV2() { return m_transformation_v_2_; }
    const Vector3r &GetTransformationRxv1() { return m_transformation_r_x_v_1_; }

    FORCE_INLINE Real &GetMass() { return m_mass_; }

    [[nodiscard]] FORCE_INLINE const Real &GetMass() const { return m_mass_; }

    FORCE_INLINE void SetMass(const Real &value) {
        m_mass_ = value;
        if (m_mass_ != 0.0)
            m_inv_mass_ = static_cast<Real>(1.0) / m_mass_;
        else
            m_inv_mass_ = 0.0;
    }

    [[nodiscard]] FORCE_INLINE const Real &GetInvMass() const { return m_inv_mass_; }

    FORCE_INLINE Vector3r &GetPosition() { return m_x_; }

    [[nodiscard]] FORCE_INLINE const Vector3r &GetPosition() const { return m_x_; }

    FORCE_INLINE void SetPosition(const Vector3r &pos) { m_x_ = pos; }

    FORCE_INLINE Vector3r &GetLastPosition() { return m_last_x_; }

    [[nodiscard]] FORCE_INLINE const Vector3r &GetLastPosition() const { return m_last_x_; }

    FORCE_INLINE void SetLastPosition(const Vector3r &pos) { m_last_x_ = pos; }

    FORCE_INLINE Vector3r &GetOldPosition() { return m_old_x_; }

    [[nodiscard]] FORCE_INLINE const Vector3r &GetOldPosition() const { return m_old_x_; }

    FORCE_INLINE void SetOldPosition(const Vector3r &pos) { m_old_x_ = pos; }

    FORCE_INLINE Vector3r &GetPosition0() { return m_x_0_; }

    [[nodiscard]] FORCE_INLINE const Vector3r &GetPosition0() const { return m_x_0_; }

    FORCE_INLINE void SetPosition0(const Vector3r &pos) { m_x_0_ = pos; }

    FORCE_INLINE Vector3r &GetPositionInitialMat() { return m_x_0_mat_; }

    [[nodiscard]] FORCE_INLINE const Vector3r &GetPositionInitialMat() const { return m_x_0_mat_; }

    FORCE_INLINE void SetPositionInitialMat(const Vector3r &pos) { m_x_0_mat_ = pos; }

    FORCE_INLINE Vector3r &GetVelocity() { return m_v_; }

    [[nodiscard]] FORCE_INLINE const Vector3r &GetVelocity() const { return m_v_; }

    FORCE_INLINE void SetVelocity(const Vector3r &value) { m_v_ = value; }

    FORCE_INLINE Vector3r &GetVelocity0() { return m_v_0_; }

    [[nodiscard]] FORCE_INLINE const Vector3r &GetVelocity0() const { return m_v_0_; }

    FORCE_INLINE void SetVelocity0(const Vector3r &value) { m_v_0_ = value; }

    FORCE_INLINE Vector3r &GetAcceleration() { return m_a_; }

    [[nodiscard]] FORCE_INLINE const Vector3r &GetAcceleration() const { return m_a_; }

    FORCE_INLINE void SetAcceleration(const Vector3r &accel) { m_a_ = accel; }

    [[nodiscard]] FORCE_INLINE const Vector3r &GetInertiaTensor() const { return m_inertia_tensor_; }

    FORCE_INLINE void SetInertiaTensor(const Vector3r &value) {
        m_inertia_tensor_ = value;
        m_inertia_tensor_inverse_ = Vector3r(static_cast<Real>(1.0) / value[0], static_cast<Real>(1.0) / value[1],
                                             static_cast<Real>(1.0) / value[2]);
    }

    FORCE_INLINE Matrix3r &GetInertiaTensorW() { return m_inertia_tensor_w_; }

    [[nodiscard]] FORCE_INLINE const Matrix3r &GetInertiaTensorW() const { return m_inertia_tensor_w_; }

    [[nodiscard]] FORCE_INLINE const Vector3r &GetInertiaTensorInverse() const { return m_inertia_tensor_inverse_; }

    FORCE_INLINE Matrix3r &GetInertiaTensorInverseW() { return m_inertia_tensor_inverse_w_; }

    [[nodiscard]] FORCE_INLINE const Matrix3r &GetInertiaTensorInverseW() const { return m_inertia_tensor_inverse_w_; }

    FORCE_INLINE void SetInertiaTensorInverseW(const Matrix3r &value) { m_inertia_tensor_inverse_w_ = value; }

    FORCE_INLINE Quaternionr &GetRotation() { return m_q_; }

    [[nodiscard]] FORCE_INLINE const Quaternionr &GetRotation() const { return m_q_; }

    FORCE_INLINE void SetRotation(const Quaternionr &value) { m_q_ = value; }

    FORCE_INLINE Quaternionr &GetLastRotation() { return m_last_q_; }

    [[nodiscard]] FORCE_INLINE const Quaternionr &GetLastRotation() const { return m_last_q_; }

    FORCE_INLINE void SetLastRotation(const Quaternionr &value) { m_last_q_ = value; }

    FORCE_INLINE Quaternionr &GetOldRotation() { return m_old_q_; }

    [[nodiscard]] FORCE_INLINE const Quaternionr &GetOldRotation() const { return m_old_q_; }

    FORCE_INLINE void SetOldRotation(const Quaternionr &value) { m_old_q_ = value; }

    FORCE_INLINE Quaternionr &GetRotation0() { return m_q_0_; }

    [[nodiscard]] FORCE_INLINE const Quaternionr &GetRotation0() const { return m_q_0_; }

    FORCE_INLINE void SetRotation0(const Quaternionr &value) { m_q_0_ = value; }

    FORCE_INLINE Quaternionr &GetRotationMat() { return m_q_mat_; }

    [[nodiscard]] FORCE_INLINE const Quaternionr &GetRotationMat() const { return m_q_mat_; }

    FORCE_INLINE void SetRotationMat(const Quaternionr &value) { m_q_mat_ = value; }

    FORCE_INLINE Quaternionr &GetRotationInitial() { return m_q_initial_; }

    [[nodiscard]] FORCE_INLINE const Quaternionr &GetRotationInitial() const { return m_q_initial_; }

    FORCE_INLINE void SetRotationInitial(const Quaternionr &value) { m_q_initial_ = value; }

    FORCE_INLINE Matrix3r &GetRotationMatrix() { return m_rot_; }

    [[nodiscard]] FORCE_INLINE const Matrix3r &GetRotationMatrix() const { return m_rot_; }

    FORCE_INLINE void SetRotationMatrix(const Matrix3r &value) { m_rot_ = value; }

    FORCE_INLINE Vector3r &GetAngularVelocity() { return m_omega_; }

    [[nodiscard]] FORCE_INLINE const Vector3r &GetAngularVelocity() const { return m_omega_; }

    FORCE_INLINE void SetAngularVelocity(const Vector3r &value) { m_omega_ = value; }

    FORCE_INLINE Vector3r &GetAngularVelocity0() { return m_omega_0_; }

    [[nodiscard]] FORCE_INLINE const Vector3r &GetAngularVelocity0() const { return m_omega_0_; }

    FORCE_INLINE void SetAngularVelocity0(const Vector3r &value) { m_omega_0_ = value; }

    FORCE_INLINE Vector3r &GetTorque() { return m_torque_; }

    [[nodiscard]] FORCE_INLINE const Vector3r &GetTorque() const { return m_torque_; }

    FORCE_INLINE void SetTorque(const Vector3r &value) { m_torque_ = value; }

    [[nodiscard]] FORCE_INLINE Real GetRestitutionCoeff() const { return m_restitution_coeff_; }

    FORCE_INLINE void SetRestitutionCoeff(Real val) { m_restitution_coeff_ = val; }

    [[nodiscard]] FORCE_INLINE Real GetFrictionCoeff() const { return m_friction_coeff_; }

    FORCE_INLINE void SetFrictionCoeff(Real val) { m_friction_coeff_ = val; }

    RigidBodyGeometry &GetGeometry() { return m_geometry_; }
};
}  // namespace vox::compute
