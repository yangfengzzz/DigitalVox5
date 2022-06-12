//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.compute/position_based_rigid_body_dynamics.h"

#include <cmath>
#include <iostream>

#include "vox.compute/math_functions.h"

namespace vox::compute {

// ----------------------------------------------------------------------------------------------
void PositionBasedRigidBodyDynamics::ComputeMatrixK(
        const Vector3r &connector, Real inv_mass, const Vector3r &x, const Matrix3r &inertia_inverse_w, Matrix3r &k) {
    if (inv_mass != 0.0) {
        const Vector3r v = connector - x;
        const Real a = v[0];
        const Real b = v[1];
        const Real c = v[2];

        // J is symmetric
        const Real j11 = inertia_inverse_w(0, 0);
        const Real j12 = inertia_inverse_w(0, 1);
        const Real j13 = inertia_inverse_w(0, 2);
        const Real j22 = inertia_inverse_w(1, 1);
        const Real j23 = inertia_inverse_w(1, 2);
        const Real j33 = inertia_inverse_w(2, 2);

        k(0, 0) = c * c * j22 - b * c * (j23 + j23) + b * b * j33 + inv_mass;
        k(0, 1) = -(c * c * j12) + a * c * j23 + b * c * j13 - a * b * j33;
        k(0, 2) = b * c * j12 - a * c * j22 - b * b * j13 + a * b * j23;
        k(1, 0) = k(0, 1);
        k(1, 1) = c * c * j11 - a * c * (j13 + j13) + a * a * j33 + inv_mass;
        k(1, 2) = -(b * c * j11) + a * c * j12 + a * b * j13 - a * a * j23;
        k(2, 0) = k(0, 2);
        k(2, 1) = k(1, 2);
        k(2, 2) = b * b * j11 - a * b * (j12 + j12) + a * a * j22 + inv_mass;
    } else
        k.setZero();
}

// ----------------------------------------------------------------------------------------------
void PositionBasedRigidBodyDynamics::ComputeMatrixK(const Vector3r &connector0,
                                                    const Vector3r &connector1,
                                                    Real inv_mass,
                                                    const Vector3r &x,
                                                    const Matrix3r &inertia_inverse_w,
                                                    Matrix3r &K) {
    if (inv_mass != 0.0) {
        const Vector3r v0 = connector0 - x;
        const Real a = v0[0];
        const Real b = v0[1];
        const Real c = v0[2];

        const Vector3r v1 = connector1 - x;
        const Real d = v1[0];
        const Real e = v1[1];
        const Real f = v1[2];

        // J is symmetric
        const Real j11 = inertia_inverse_w(0, 0);
        const Real j12 = inertia_inverse_w(0, 1);
        const Real j13 = inertia_inverse_w(0, 2);
        const Real j22 = inertia_inverse_w(1, 1);
        const Real j23 = inertia_inverse_w(1, 2);
        const Real j33 = inertia_inverse_w(2, 2);

        K(0, 0) = c * f * j22 - c * e * j23 - b * f * j23 + b * e * j33 + inv_mass;
        K(0, 1) = -(c * f * j12) + c * d * j23 + b * f * j13 - b * d * j33;
        K(0, 2) = c * e * j12 - c * d * j22 - b * e * j13 + b * d * j23;
        K(1, 0) = -(c * f * j12) + c * e * j13 + a * f * j23 - a * e * j33;
        K(1, 1) = c * f * j11 - c * d * j13 - a * f * j13 + a * d * j33 + inv_mass;
        K(1, 2) = -(c * e * j11) + c * d * j12 + a * e * j13 - a * d * j23;
        K(2, 0) = b * f * j12 - b * e * j13 - a * f * j22 + a * e * j23;
        K(2, 1) = -(b * f * j11) + b * d * j13 + a * f * j12 - a * d * j23;
        K(2, 2) = b * e * j11 - b * d * j12 - a * e * j12 + a * d * j22 + inv_mass;
    } else
        K.setZero();
}

// ----------------------------------------------------------------------------------------------
void PositionBasedRigidBodyDynamics::ComputeMatrixG(const Quaternionr &q,
                                                    Eigen::Matrix<Real, 4, 3, Eigen::DontAlign> &G) {
    G(0, 0) = -static_cast<Real>(0.5) * q.x();
    G(0, 1) = -static_cast<Real>(0.5) * q.y();
    G(0, 2) = -static_cast<Real>(0.5) * q.z();

    G(1, 0) = static_cast<Real>(0.5) * q.w();
    G(1, 1) = static_cast<Real>(0.5) * q.z();
    G(1, 2) = static_cast<Real>(-0.5) * q.y();

    G(2, 0) = static_cast<Real>(-0.5) * q.z();
    G(2, 1) = static_cast<Real>(0.5) * q.w();
    G(2, 2) = static_cast<Real>(0.5) * q.x();

    G(3, 0) = static_cast<Real>(0.5) * q.y();
    G(3, 1) = static_cast<Real>(-0.5) * q.x();
    G(3, 2) = static_cast<Real>(0.5) * q.w();
}

// ----------------------------------------------------------------------------------------------
void PositionBasedRigidBodyDynamics::ComputeMatrixQ(const Quaternionr &q,
                                                    Eigen::Matrix<Real, 4, 4, Eigen::DontAlign> &Q) {
    Q(0, 0) = q.w();
    Q(0, 1) = -q.x();
    Q(0, 2) = -q.y();
    Q(0, 3) = -q.z();

    Q(1, 0) = q.x();
    Q(1, 1) = q.w();
    Q(1, 2) = -q.z();
    Q(1, 3) = q.y();

    Q(2, 0) = q.y();
    Q(2, 1) = q.z();
    Q(2, 2) = q.w();
    Q(2, 3) = -q.x();

    Q(3, 0) = q.z();
    Q(3, 1) = -q.y();
    Q(3, 2) = q.x();
    Q(3, 3) = q.w();
}

// ----------------------------------------------------------------------------------------------
void PositionBasedRigidBodyDynamics::ComputeMatrixQHat(const Quaternionr &q,
                                                       Eigen::Matrix<Real, 4, 4, Eigen::DontAlign> &Q) {
    Q(0, 0) = q.w();
    Q(0, 1) = -q.x();
    Q(0, 2) = -q.y();
    Q(0, 3) = -q.z();

    Q(1, 0) = q.x();
    Q(1, 1) = q.w();
    Q(1, 2) = q.z();
    Q(1, 3) = -q.y();

    Q(2, 0) = q.y();
    Q(2, 1) = -q.z();
    Q(2, 2) = q.w();
    Q(2, 3) = q.x();

    Q(3, 0) = q.z();
    Q(3, 1) = q.y();
    Q(3, 2) = -q.x();
    Q(3, 3) = q.w();
}

//MARK: -
// ----------------------------------------------------------------------------------------------
bool PositionBasedRigidBodyDynamics::InitBallJoint(const Vector3r &x0,
                                                   const Quaternionr &q0,
                                                   const Vector3r &x1,
                                                   const Quaternionr &q1,
                                                   const Vector3r &joint_position,
                                                   Eigen::Matrix<Real, 3, 4, Eigen::DontAlign> &joint_info) {
    // jointInfo contains
    // 0:	connector in body 0 (local)
    // 1:	connector in body 1 (local)
    // 2:	connector in body 0 (global)
    // 3:	connector in body 1 (global)

    // transform in local coordinates
    const Matrix3r rot0T = q0.matrix().transpose();
    const Matrix3r rot1T = q1.matrix().transpose();

    joint_info.col(0) = rot0T * (joint_position - x0);
    joint_info.col(1) = rot1T * (joint_position - x1);
    joint_info.col(2) = joint_position;
    joint_info.col(3) = joint_position;

    return true;
}

// ----------------------------------------------------------------------------------------------
bool PositionBasedRigidBodyDynamics::UpdateBallJoint(const Vector3r &x0,
                                                     const Quaternionr &q0,
                                                     const Vector3r &x1,
                                                     const Quaternionr &q1,
                                                     Eigen::Matrix<Real, 3, 4, Eigen::DontAlign> &ball_joint_info) {
    // jointInfo contains
    // 0:	connector in body 0 (local)
    // 1:	connector in body 1 (local)
    // 2:	connector in body 0 (global)
    // 3:	connector in body 1 (global)

    // compute world space positions of connectors
    const Matrix3r rot0 = q0.matrix();
    const Matrix3r rot1 = q1.matrix();
    ball_joint_info.col(2) = rot0 * ball_joint_info.col(0) + x0;
    ball_joint_info.col(3) = rot1 * ball_joint_info.col(1) + x1;

    return true;
}

// ----------------------------------------------------------------------------------------------
bool PositionBasedRigidBodyDynamics::SolveBallJoint(Real inv_mass_0,
                                                    const Vector3r &x0,
                                                    const Matrix3r &inertia_inverse_w_0,
                                                    const Quaternionr &q0,
                                                    Real invMass1,
                                                    const Vector3r &x1,
                                                    const Matrix3r &inertia_inverse_w_1,
                                                    const Quaternionr &q1,
                                                    const Eigen::Matrix<Real, 3, 4, Eigen::DontAlign> &ball_joint_info,
                                                    Vector3r &corr_x0,
                                                    Quaternionr &corr_q0,
                                                    Vector3r &corr_x1,
                                                    Quaternionr &corr_q1) {
    // jointInfo contains
    // 0:	connector in body 0 (local)
    // 1:	connector in body 1 (local)
    // 2:	connector in body 0 (global)
    // 3:	connector in body 1 (global)

    const Vector3r &connector0 = ball_joint_info.col(2);
    const Vector3r &connector1 = ball_joint_info.col(3);

    // Compute Kinv
    Matrix3r K1, K2;
    ComputeMatrixK(connector0, inv_mass_0, x0, inertia_inverse_w_0, K1);
    ComputeMatrixK(connector1, invMass1, x1, inertia_inverse_w_1, K2);

    const Vector3r pt = (K1 + K2).llt().solve(connector1 - connector0);

    if (inv_mass_0 != 0.0) {
        const Vector3r r0 = connector0 - x0;
        corr_x0 = inv_mass_0 * pt;

        const Vector3r ot = (inertia_inverse_w_0 * (r0.cross(pt)));
        const Quaternionr otQ(0.0, ot[0], ot[1], ot[2]);
        corr_q0.coeffs() = 0.5 * (otQ * q0).coeffs();
    }

    if (invMass1 != 0.0) {
        const Vector3r r1 = connector1 - x1;
        corr_x1 = -invMass1 * pt;

        const Vector3r ot = (inertia_inverse_w_1 * (r1.cross(-pt)));
        const Quaternionr otQ(0.0, ot[0], ot[1], ot[2]);
        corr_q1.coeffs() = 0.5 * (otQ * q1).coeffs();
    }

    return true;
}

//MARK: -
// ----------------------------------------------------------------------------------------------
bool PositionBasedRigidBodyDynamics::InitDistanceJoint(const Vector3r &x0,
                                                       const Quaternionr &q0,
                                                       const Vector3r &x1,
                                                       const Quaternionr &q1,
                                                       const Vector3r &pos0,
                                                       const Vector3r &pos1,
                                                       Eigen::Matrix<Real, 3, 4, Eigen::DontAlign> &joint_info) {
    // jointInfo contains
    // 0:	connector in body 0 (local)
    // 1:	connector in body 1 (local)
    // 2:	connector in body 0 (global)
    // 3:	connector in body 1 (global)

    // transform in local coordinates
    const Matrix3r rot0T = q0.matrix().transpose();
    const Matrix3r rot1T = q1.matrix().transpose();

    joint_info.col(0) = rot0T * (pos0 - x0);
    joint_info.col(1) = rot1T * (pos1 - x1);
    joint_info.col(2) = pos0;
    joint_info.col(3) = pos1;

    return true;
}

// ----------------------------------------------------------------------------------------------
bool PositionBasedRigidBodyDynamics::UpdateDistanceJoint(const Vector3r &x0,
                                                         const Quaternionr &q0,
                                                         const Vector3r &x1,
                                                         const Quaternionr &q1,
                                                         Eigen::Matrix<Real, 3, 4, Eigen::DontAlign> &joint_info) {
    // jointInfo contains
    // 0:	connector in body 0 (local)
    // 1:	connector in body 1 (local)
    // 2:	connector in body 0 (global)
    // 3:	connector in body 1 (global)

    // compute world space positions of connectors
    const Matrix3r rot0 = q0.matrix();
    const Matrix3r rot1 = q1.matrix();
    joint_info.col(2) = rot0 * joint_info.col(0) + x0;
    joint_info.col(3) = rot1 * joint_info.col(1) + x1;

    return true;
}

// ----------------------------------------------------------------------------------------------
bool PositionBasedRigidBodyDynamics::SolveDistanceJoint(Real inv_mass_0,
                                                        const Vector3r &x0,
                                                        const Matrix3r &inertia_inverse_w_0,
                                                        const Quaternionr &q0,
                                                        Real inv_mass_1,
                                                        const Vector3r &x1,
                                                        const Matrix3r &inertia_inverse_w_1,
                                                        const Quaternionr &q1,
                                                        Real stiffness,
                                                        Real rest_length,
                                                        Real dt,
                                                        const Eigen::Matrix<Real, 3, 4, Eigen::DontAlign> &joint_info,
                                                        Real &lambda,
                                                        Vector3r &corr_x0,
                                                        Quaternionr &corr_q0,
                                                        Vector3r &corr_x1,
                                                        Quaternionr &corr_q1) {
    // jointInfo contains
    // 0:	connector in body 0 (local)
    // 1:	connector in body 1 (local)
    // 2:	connector in body 0 (global)
    // 3:	connector in body 1 (global)

    // evaluate constraint function
    const Vector3r &c0 = joint_info.col(2);
    const Vector3r &c1 = joint_info.col(3);
    const Real length = (c0 - c1).norm();

    const Real C = (length - rest_length);

    // compute K = J M^-1 J^T
    Matrix3r K1, K2;
    ComputeMatrixK(c0, inv_mass_0, x0, inertia_inverse_w_0, K1);
    ComputeMatrixK(c1, inv_mass_1, x1, inertia_inverse_w_1, K2);
    Vector3r dir = c0 - c1;
    if (length > static_cast<Real>(1e-5))
        dir /= length;
    else {
        corr_x0.setZero();
        corr_x1.setZero();
        corr_q0.setIdentity();
        corr_q1.setIdentity();
        return true;
    }

    // J = (dir^T	dir^T * r^* )

    // J = (I   r^*)

    Real K = (dir.transpose() * (K1 + K2)).dot(dir);

    Real alpha = 0.0;
    if (stiffness != 0.0) {
        alpha = static_cast<Real>(1.0) / (stiffness * dt * dt);
        K += alpha;
    }

    Real Kinv = 0.0;
    if (fabs(K) > static_cast<Real>(1e-6))
        Kinv = static_cast<Real>(1.0) / K;
    else {
        corr_x0.setZero();
        corr_x1.setZero();
        corr_q0.setIdentity();
        corr_q1.setIdentity();
        return true;
    }

    const Real delta_lambda = -Kinv * (C + alpha * lambda);
    lambda += delta_lambda;
    const Vector3r pt = dir * delta_lambda;

    if (inv_mass_0 != 0.0) {
        const Vector3r r0 = c0 - x0;
        corr_x0 = inv_mass_0 * pt;

        const Vector3r ot = (inertia_inverse_w_0 * (r0.cross(pt)));
        const Quaternionr otQ(0.0, ot[0], ot[1], ot[2]);
        corr_q0.coeffs() = 0.5 * (otQ * q0).coeffs();
    }

    if (inv_mass_1 != 0.0) {
        const Vector3r r1 = c1 - x1;
        corr_x1 = -inv_mass_1 * pt;

        const Vector3r ot = (inertia_inverse_w_1 * (r1.cross(-pt)));
        const Quaternionr otQ(0.0, ot[0], ot[1], ot[2]);
        corr_q1.coeffs() = 0.5 * (otQ * q1).coeffs();
    }
    return true;
}

//MARK: -
// ----------------------------------------------------------------------------------------------
bool PositionBasedRigidBodyDynamics::InitBallOnLineJoint(const Vector3r &x0,
                                                         const Quaternionr &q0,
                                                         const Vector3r &x1,
                                                         const Quaternionr &q1,
                                                         const Vector3r &position,
                                                         const Vector3r &direction,
                                                         Eigen::Matrix<Real, 3, 10, Eigen::DontAlign> &joint_info) {
    // jointInfo contains
    // 0:	connector in body 0 (local)
    // 1:	connector in body 1 (local)
    // 2-4:	coordinate system of body 0 (local)
    // 5:	connector in body 0 (global)
    // 6:	connector in body 1 (global)
    // 7-9:	coordinate system of body 0 (global)

    // transform in local coordinates
    const Matrix3r rot0T = q0.matrix().transpose();
    const Matrix3r rot1T = q1.matrix().transpose();

    joint_info.col(0) = rot0T * (position - x0);
    joint_info.col(1) = rot1T * (position - x1);
    joint_info.col(5) = position;
    joint_info.col(6) = position;

    // determine constraint coordinate system
    // with direction as x-axis
    joint_info.col(7) = direction;
    joint_info.col(7).normalize();

    Vector3r v(1.0, 0.0, 0.0);
    // check if vectors are parallel
    if (fabs(v.dot(joint_info.col(7))) > 0.99) v = Vector3r(0.0, 1.0, 0.0);

    joint_info.col(8) = joint_info.col(7).cross(v);
    joint_info.col(9) = joint_info.col(7).cross(joint_info.col(8));
    joint_info.col(8).normalize();
    joint_info.col(9).normalize();

    joint_info.block<3, 3>(0, 2) = rot0T * joint_info.block<3, 3>(0, 7);

    return true;
}

// ----------------------------------------------------------------------------------------------
bool PositionBasedRigidBodyDynamics::UpdateBallOnLineJoint(const Vector3r &x0,
                                                           const Quaternionr &q0,
                                                           const Vector3r &x1,
                                                           const Quaternionr &q1,
                                                           Eigen::Matrix<Real, 3, 10, Eigen::DontAlign> &joint_info) {
    // jointInfo contains
    // 0:	connector in body 0 (local)
    // 1:	connector in body 1 (local)
    // 2-4:	coordinate system of body 0 (local)
    // 5:	connector in body 0 (global)
    // 6:	connector in body 1 (global)
    // 7-9:	coordinate system of body 0 (global)

    // compute world space positions of connectors
    const Matrix3r rot0 = q0.matrix();
    const Matrix3r rot1 = q1.matrix();
    joint_info.col(5) = rot0 * joint_info.col(0) + x0;
    joint_info.col(6) = rot1 * joint_info.col(1) + x1;

    // transform constraint coordinate system to world space
    joint_info.block<3, 3>(0, 7) = rot0 * joint_info.block<3, 3>(0, 2);

    const Vector3r dir = joint_info.col(7);
    const Vector3r p = joint_info.col(5);
    const Vector3r s = joint_info.col(6);
    // move the joint point of body 0 to the closest point on the line to joint point 1
    joint_info.col(5) = p + (dir * (((s - p).dot(dir)) / dir.squaredNorm()));

    return true;
}

// ----------------------------------------------------------------------------------------------
bool PositionBasedRigidBodyDynamics::SolveBallOnLineJoint(
        Real inv_mass_0,
        const Vector3r &x0,
        const Matrix3r &inertia_inverse_w_0,
        const Quaternionr &q0,
        Real inv_mass_1,
        const Vector3r &x1,
        const Matrix3r &inertia_inverse_w_1,
        const Quaternionr &q1,
        const Eigen::Matrix<Real, 3, 10, Eigen::DontAlign> &joint_info,
        Vector3r &corr_x0,
        Quaternionr &corr_q0,
        Vector3r &corr_x1,
        Quaternionr &corr_q1) {
    // jointInfo contains
    // 0:	connector in body 0 (local)
    // 1:	connector in body 1 (local)
    // 2-4:	coordinate system of body 0 (local)
    // 5:	connector in body 0 (global)
    // 6:	connector in body 1 (global)
    // 7-9:	coordinate system of body 0 (global)

    const Vector3r &connector0 = joint_info.col(5);
    const Vector3r &connector1 = joint_info.col(6);

    // Compute Kinv
    Matrix3r K1, K2;
    ComputeMatrixK(connector0, inv_mass_0, x0, inertia_inverse_w_0, K1);
    ComputeMatrixK(connector1, inv_mass_1, x1, inertia_inverse_w_1, K2);

    // projection
    const Eigen::Matrix<Real, 3, 2> PT = joint_info.block<3, 2>(0, 8);
    const Eigen::Matrix<Real, 2, 3> P = PT.transpose();

    const Matrix2r K = P * (K1 + K2) * PT;
    const Vector2r pt2D = K.llt().solve(P * (connector1 - connector0));
    const Vector3r pt = PT * pt2D;

    if (inv_mass_0 != 0.0) {
        const Vector3r r0 = connector0 - x0;
        corr_x0 = inv_mass_0 * pt;

        const Vector3r ot = (inertia_inverse_w_0 * (r0.cross(pt)));
        const Quaternionr otQ(0.0, ot[0], ot[1], ot[2]);
        corr_q0.coeffs() = 0.5 * (otQ * q0).coeffs();
    }

    if (inv_mass_1 != 0.0) {
        const Vector3r r1 = connector1 - x1;
        corr_x1 = -inv_mass_1 * pt;

        const Vector3r ot = (inertia_inverse_w_1 * (r1.cross(-pt)));
        const Quaternionr otQ(0.0, ot[0], ot[1], ot[2]);
        corr_q1.coeffs() = 0.5 * (otQ * q1).coeffs();
    }

    return true;
}

//MARK: -
// ----------------------------------------------------------------------------------------------
bool PositionBasedRigidBodyDynamics::InitHingeJoint(const Vector3r &x0,
                                                    const Quaternionr &q0,
                                                    const Vector3r &x1,
                                                    const Quaternionr &q1,
                                                    const Vector3r &hinge_joint_position,
                                                    const Vector3r &hinge_joint_axis,
                                                    Eigen::Matrix<Real, 4, 7, Eigen::DontAlign> &hinge_joint_info) {
    // jointInfo contains
    // 0-1:	projection matrix Pr for the rotational part
    // 2:	connector in body 0 (local)
    // 3:	connector in body 1 (local)
    // 4:	connector in body 0 (global)
    // 5:	connector in body 1 (global)
    // 6:	hinge axis in body 0 (local) used for rendering

    // transform in local coordinates
    const Matrix3r rot0T = q0.matrix().transpose();
    const Matrix3r rot1T = q1.matrix().transpose();

    // connector in body 0 (local)
    hinge_joint_info.block<3, 1>(0, 2) = rot0T * (hinge_joint_position - x0);
    // connector in body 1 (local)
    hinge_joint_info.block<3, 1>(0, 3) = rot1T * (hinge_joint_position - x1);
    // connector in body 0 (global)
    hinge_joint_info.block<3, 1>(0, 4) = hinge_joint_position;
    // connector in body 1 (global)
    hinge_joint_info.block<3, 1>(0, 5) = hinge_joint_position;
    hinge_joint_info.block<3, 1>(0, 6) = rot0T * hinge_joint_axis;

    // determine constraint coordinate system
    // with direction as x-axis
    Matrix3r R0;
    R0.col(0) = hinge_joint_axis;
    R0.col(0).normalize();

    Vector3r v(1.0, 0.0, 0.0);
    // check if vectors are parallel
    if (fabs(v.dot(R0.col(0))) > 0.99) v = Vector3r(0.0, 1.0, 0.0);

    R0.col(1) = R0.col(0).cross(v);
    R0.col(2) = R0.col(0).cross(R0.col(1));
    R0.col(1).normalize();
    R0.col(2).normalize();

    Quaternionr qR0(R0);
    const Quaternionr q00 = (q0.conjugate() * qR0).conjugate();
    const Quaternionr q10 = (q1.conjugate() * qR0).conjugate();

    Eigen::Matrix<Real, 4, 4, Eigen::DontAlign> Qq00, QHatq10;
    ComputeMatrixQ(q00, Qq00);
    ComputeMatrixQHat(q10, QHatq10);
    Eigen::Matrix<Real, 2, 4, Eigen::DontAlign> Pr = (QHatq10.transpose() * Qq00).block<2, 4>(2, 0);
    hinge_joint_info.block<4, 2>(0, 0) = Pr.transpose();

    return true;
}

// ----------------------------------------------------------------------------------------------
bool PositionBasedRigidBodyDynamics::UpdateHingeJoint(const Vector3r &x0,
                                                      const Quaternionr &q0,
                                                      const Vector3r &x1,
                                                      const Quaternionr &q1,
                                                      Eigen::Matrix<Real, 4, 7, Eigen::DontAlign> &hinge_joint_info) {
    // jointInfo contains
    // 0-1:	projection matrix Pr for the rotational part
    // 2:	connector in body 0 (local)
    // 3:	connector in body 1 (local)
    // 4:	connector in body 0 (global)
    // 5:	connector in body 1 (global)
    // 6:	hinge axis in body 0 (local) used for rendering

    // compute world space positions of connectors
    const Matrix3r rot0 = q0.matrix();
    const Matrix3r rot1 = q1.matrix();
    hinge_joint_info.block<3, 1>(0, 4) = rot0 * hinge_joint_info.block<3, 1>(0, 2) + x0;
    hinge_joint_info.block<3, 1>(0, 5) = rot1 * hinge_joint_info.block<3, 1>(0, 3) + x1;

    return true;
}

// ----------------------------------------------------------------------------------------------
bool PositionBasedRigidBodyDynamics::SolveHingeJoint(
        Real inv_mass_0,
        const Vector3r &x0,
        const Matrix3r &inertia_inverse_w_0,
        const Quaternionr &q0,
        Real inv_mass_1,
        const Vector3r &x1,
        const Matrix3r &inertia_inverse_w_1,
        const Quaternionr &q1,
        const Eigen::Matrix<Real, 4, 7, Eigen::DontAlign> &hinge_joint_info,
        Vector3r &corr_x0,
        Quaternionr &corr_q0,
        Vector3r &corr_x1,
        Quaternionr &corr_q1) {
    // jointInfo contains
    // 0-1:	projection matrix Pr for the rotational part
    // 2:	connector in body 0 (local)
    // 3:	connector in body 1 (local)
    // 4:	connector in body 0 (global)
    // 5:	connector in body 1 (global)
    // 6:	hinge axis in body 0 (local) used for rendering

    // compute constraint value
    const Vector3r &c0 = hinge_joint_info.block<3, 1>(0, 4);
    const Vector3r &c1 = hinge_joint_info.block<3, 1>(0, 5);
    const Eigen::Matrix<Real, 2, 4> &Pr = hinge_joint_info.block<4, 2>(0, 0).transpose();

    Eigen::Matrix<Real, 5, 1> C;
    C.block<3, 1>(0, 0) = c0 - c1;

    const Quaternionr tmp = (q0.conjugate() * q1);
    const Vector4r qVec(tmp.w(), tmp.x(), tmp.y(), tmp.z());
    C.block<2, 1>(3, 0) = Pr * qVec;

    // compute matrix J M^-1 J^T = K
    const Vector3r r0 = c0 - x0;
    const Vector3r r1 = c1 - x1;
    Matrix3r r0_star, r1_star;
    MathFunctions::CrossProductMatrix(r0, r0_star);
    MathFunctions::CrossProductMatrix(r1, r1_star);

    Eigen::Matrix<Real, 4, 3, Eigen::DontAlign> Gq1;
    ComputeMatrixG(q1, Gq1);
    Eigen::Matrix<Real, 4, 4, Eigen::DontAlign> Qq0;
    ComputeMatrixQ(q0, Qq0);

    const Eigen::Matrix<Real, 2, 3> t = -Pr * (Qq0.transpose() * Gq1);

    Eigen::Matrix<Real, 5, 5> K;
    K.setZero();
    if (inv_mass_0 != 0.0) {
        // Jacobian for body 0 is
        //
        // (I_3   -r0*)
        // (0     t)
        //
        // where I_3 is the identity matrix, r0* is the cross product matrix of r0 and
        // t = -Pr * (Qq0^T * Gq1)
        //
        // J M^-1 J^T =
        // ( 1/m I_3-r0 * J0^-1 * r0*    -r0 * J0^-1 * t^T   )
        // ( (-r0 * J0^-1 * t^T)^T       t * J0^-1 * t^T     )

        Matrix3r K00;
        ComputeMatrixK(c0, inv_mass_0, x0, inertia_inverse_w_0, K00);

        K.block<3, 3>(0, 0) = K00;
        K.block<3, 2>(0, 3) = -r0_star * inertia_inverse_w_0 * t.transpose();
        K.block<2, 3>(3, 0) = K.block<3, 2>(0, 3).transpose();
        K.block<2, 2>(3, 3) = t * inertia_inverse_w_0 * t.transpose();
    }
    if (inv_mass_1 != 0.0) {
        // Jacobian for body 1 is
        //
        // (-I_3   r1*)
        // (0     -t)
        //
        // where I_3 is the identity matrix, r1* is the cross product matrix of r1 and
        // t = -Pr * (Qq0^T * Gq1)
        //
        // J M^-1 J^T =
        // ( 1/m I_3-r1 * J1^-1 * r1*    r1 * J1^-1 * t^T   )
        // ( (r1 * J1^-1 * t^T)^T       t * J1^-1 * t^T     )

        Matrix3r K11;
        ComputeMatrixK(c1, inv_mass_1, x1, inertia_inverse_w_1, K11);

        K.block<3, 3>(0, 0) += K11;
        Eigen::Matrix<Real, 3, 2> K03 = -r1_star * inertia_inverse_w_1 * t.transpose();
        K.block<3, 2>(0, 3) += K03;
        K.block<2, 3>(3, 0) += K03.transpose();
        K.block<2, 2>(3, 3) += t * inertia_inverse_w_1 * t.transpose();
    }

    const Eigen::Matrix<Real, 5, 1> lambda = K.llt().solve(-C);

    const Vector3r pt = lambda.block<3, 1>(0, 0);
    const Vector3r amt = t.transpose() * lambda.block<2, 1>(3, 0);

    if (inv_mass_0 != 0.0) {
        corr_x0 = inv_mass_0 * pt;
        const Vector3r ot = (inertia_inverse_w_0 * (r0.cross(pt) + amt));
        const Quaternionr otQ(0.0, ot[0], ot[1], ot[2]);
        corr_q0.coeffs() = 0.5 * (otQ * q0).coeffs();
    }

    if (inv_mass_1 != 0.0) {
        corr_x1 = -inv_mass_1 * pt;
        const Vector3r ot = (inertia_inverse_w_1 * (r1.cross(-pt) - amt));
        const Quaternionr otQ(0.0, ot[0], ot[1], ot[2]);
        corr_q1.coeffs() = 0.5 * (otQ * q1).coeffs();
    }

    return true;
}

//MARK: -
// ----------------------------------------------------------------------------------------------
bool PositionBasedRigidBodyDynamics::InitUniversalJoint(const Vector3r &x0,
                                                        const Quaternionr &q0,
                                                        const Vector3r &x1,
                                                        const Quaternionr &q1,
                                                        const Vector3r &joint_position,
                                                        const Vector3r &joint_axis_0,
                                                        const Vector3r &joint_axis_1,
                                                        Eigen::Matrix<Real, 3, 8, Eigen::DontAlign> &joint_info) {
    // jointInfo contains
    // 0:	connector in body 0 (local)
    // 1:	connector in body 1 (local)
    // 2:	constraint axis 0 in body 0 (local)
    // 3:	constraint axis 1 in body 1 (local)
    // 4:	connector in body 0 (global)
    // 5:	connector in body 1 (global)
    // 6:	constraint axis 0 in body 0 (global)
    // 7:	constraint axis 1 in body 1 (global)

    // transform in local coordinates
    const Matrix3r rot0T = q0.matrix().transpose();
    const Matrix3r rot1T = q1.matrix().transpose();

    // connector in body 0 (local)
    joint_info.col(0) = rot0T * (joint_position - x0);
    // connector in body 1 (local)
    joint_info.col(1) = rot1T * (joint_position - x1);
    // connector in body 0 (global)
    joint_info.col(4) = joint_position;
    // connector in body 1 (global)
    joint_info.col(5) = joint_position;

    // determine constraint coordinate system
    Vector3r constraintAxis = joint_axis_0.cross(joint_axis_1);
    if (constraintAxis.norm() < 1.0e-3) return false;

    // joint axis in body 0 (global)
    joint_info.col(6) = joint_axis_0;
    joint_info.col(6).normalize();

    // joint axis in body 1 (global)
    joint_info.col(7) = joint_axis_1;
    joint_info.col(7).normalize();

    // correction axis in body 0 (local)
    joint_info.col(2) = rot0T * joint_info.col(6);

    // correction axis in body 1 (local)
    joint_info.col(3) = rot1T * joint_info.col(7);

    return true;
}

// ----------------------------------------------------------------------------------------------
bool PositionBasedRigidBodyDynamics::UpdateUniversalJoint(const Vector3r &x0,
                                                          const Quaternionr &q0,
                                                          const Vector3r &x1,
                                                          const Quaternionr &q1,
                                                          Eigen::Matrix<Real, 3, 8, Eigen::DontAlign> &joint_info) {
    // jointInfo contains
    // 0:	connector in body 0 (local)
    // 1:	connector in body 1 (local)
    // 2:	constraint axis 0 in body 0 (local)
    // 3:	constraint axis 1 in body 1 (local)
    // 4:	connector in body 0 (global)
    // 5:	connector in body 1 (global)
    // 6:	constraint axis 0 in body 0 (global)
    // 7:	constraint axis 1 in body 1 (global)

    // compute world space positions of connectors
    const Matrix3r rot0 = q0.matrix();
    const Matrix3r rot1 = q1.matrix();
    joint_info.col(4) = rot0 * joint_info.col(0) + x0;
    joint_info.col(5) = rot1 * joint_info.col(1) + x1;

    // transform joint axis of body 0 to world space
    joint_info.col(6) = rot0 * joint_info.col(2);
    // transform joint axis of body 1 to world space
    joint_info.col(7) = rot1 * joint_info.col(3);

    return true;
}

// ----------------------------------------------------------------------------------------------
bool PositionBasedRigidBodyDynamics::SolveUniversalJoint(Real inv_mass_0,
                                                         const Vector3r &x0,
                                                         const Matrix3r &inertia_inverse_w_0,
                                                         const Quaternionr &q0,
                                                         Real inv_mass_1,
                                                         const Vector3r &x1,
                                                         const Matrix3r &inertia_inverse_w_1,
                                                         const Quaternionr &q1,
                                                         const Eigen::Matrix<Real, 3, 8, Eigen::DontAlign> &joint_info,
                                                         Vector3r &corr_x0,
                                                         Quaternionr &corr_q0,
                                                         Vector3r &corr_x1,
                                                         Quaternionr &corr_q1) {
    // jointInfo contains
    // 0:	connector in body 0 (local)
    // 1:	connector in body 1 (local)
    // 2:	constraint axis 0 in body 0 (local)
    // 3:	constraint axis 1 in body 1 (local)
    // 4:	connector in body 0 (global)
    // 5:	connector in body 1 (global)
    // 6:	constraint axis 0 in body 0 (global)
    // 7:	constraint axis 1 in body 1 (global)

    const Vector3r &c0 = joint_info.col(4);
    const Vector3r &c1 = joint_info.col(5);
    const Vector3r &axis0 = joint_info.col(6);
    const Vector3r &axis1 = joint_info.col(7);
    const Vector3r u = axis0.cross(axis1);
    const Vector3r r0 = c0 - x0;
    const Vector3r r1 = c1 - x1;
    Matrix3r r0_star, r1_star;
    MathFunctions::CrossProductMatrix(r0, r0_star);
    MathFunctions::CrossProductMatrix(r1, r1_star);

    Eigen::Matrix<Real, 4, 1> b;
    b.block<3, 1>(0, 0) = c1 - c0;
    b(3, 0) = -axis0.dot(axis1);

    Eigen::Matrix<Real, 4, 4> K;
    K.setZero();
    Eigen::Matrix<Real, 4, 6> J0, J1;
    if (inv_mass_0 != 0.0) {
        // Jacobian for body 0 is
        //
        // (I_3   -r0*)
        // (0     u^T)
        //
        // where I_3 is the identity matrix and r0* is the cross product matrix of r0
        //
        // J M^-1 J^T =
        // ( 1/m I_3-r0 * J0^-1 * r0*    -r0 * J0^-1 * u )
        // ( (-r0 * J0^-1 * u)^T         u^T * J0^-1 * u )

        Matrix3r K00;
        ComputeMatrixK(c0, inv_mass_0, x0, inertia_inverse_w_0, K00);

        K.block<3, 3>(0, 0) = K00;
        K.block<3, 1>(0, 3) = -r0_star * inertia_inverse_w_0 * u;
        K.block<1, 3>(3, 0) = K.block<3, 1>(0, 3).transpose();
        K(3, 3) = u.transpose() * inertia_inverse_w_0 * u;
    }
    if (inv_mass_1 != 0.0) {
        // Jacobian for body 1 is
        //
        // ( -I_3   r1*  )
        // ( 0     -u^T )
        //
        // where I_3 is the identity matrix and r1* is the cross product matrix of r1
        //
        // J M^-1 J^T =
        // ( 1/m I_3-r1 * J1^-1 * r1*    -r1 * J1^-1 * u )
        // ( (-r1 * J1^-1 * u)^T         u^T * J1^-1 * u )

        Matrix3r K11;
        ComputeMatrixK(c1, inv_mass_1, x1, inertia_inverse_w_1, K11);

        K.block<3, 3>(0, 0) += K11;
        const Vector3r K_03 = -r1_star * inertia_inverse_w_1 * u;
        K.block<3, 1>(0, 3) += K_03;
        K.block<1, 3>(3, 0) += K_03.transpose();
        K(3, 3) += u.transpose() * inertia_inverse_w_1 * u;
    }

    const Eigen::Matrix<Real, 4, 1> lambda = K.llt().solve(b);

    const Vector3r pt = lambda.block<3, 1>(0, 0);

    if (inv_mass_0 != 0.0) {
        corr_x0 = inv_mass_0 * pt;
        const Vector3r ot = (inertia_inverse_w_0 * (r0.cross(pt) + u * lambda(3, 0)));
        const Quaternionr otQ(0.0, ot[0], ot[1], ot[2]);
        corr_q0.coeffs() = 0.5 * (otQ * q0).coeffs();
    }

    if (inv_mass_1 != 0.0) {
        corr_x1 = -inv_mass_1 * pt;
        const Vector3r ot = (inertia_inverse_w_1 * (r1.cross(-pt) - u * lambda(3, 0)));
        const Quaternionr otQ(0.0, ot[0], ot[1], ot[2]);
        corr_q1.coeffs() = 0.5 * (otQ * q1).coeffs();
    }

    return true;
}

//MARK: -
// ----------------------------------------------------------------------------------------------
bool PositionBasedRigidBodyDynamics::InitSliderJoint(const Vector3r &x0,
                                                     const Quaternionr &q0,
                                                     const Vector3r &x1,
                                                     const Quaternionr &q1,
                                                     const Vector3r &slider_joint_axis,
                                                     Eigen::Matrix<Real, 4, 6, Eigen::DontAlign> &joint_info) {
    // jointInfo contains
    // 0:	coordinate system in body 0, where the x-axis is the slider axis (local)
    // 1:	coordinate system in body 0, where the x-axis is the slider axis (global)
    // 2:   2D vector d = P * (x0 - x1), where P projects the vector onto a plane perpendicular to the slider axis
    // 3-5:	projection matrix Pr for the rotational part

    // determine constraint coordinate system
    // with direction as x-axis
    Matrix3r R0;
    R0.col(0) = slider_joint_axis;
    R0.col(0).normalize();

    Vector3r v(1.0, 0.0, 0.0);
    // check if vectors are parallel
    if (fabs(v.dot(R0.col(0))) > 0.99) v = Vector3r(0.0, 1.0, 0.0);

    R0.col(1) = R0.col(0).cross(v);
    R0.col(2) = R0.col(0).cross(R0.col(1));
    R0.col(1).normalize();
    R0.col(2).normalize();

    Quaternionr qR0(R0);
    joint_info.col(1) = qR0.coeffs();

    // coordinate system of body 0 (local)
    joint_info.col(0) = (q0.conjugate() * qR0).coeffs();

    const Eigen::Matrix<Real, 2, 3> P = R0.block<3, 2>(0, 1).transpose();
    joint_info.block<2, 1>(0, 2) = P * (x0 - x1);

    const Quaternionr q00 = (q0.conjugate() * qR0).conjugate();
    const Quaternionr q10 = (q1.conjugate() * qR0).conjugate();

    Eigen::Matrix<Real, 4, 4, Eigen::DontAlign> Qq00, QHatq10;
    ComputeMatrixQ(q00, Qq00);
    ComputeMatrixQHat(q10, QHatq10);
    Eigen::Matrix<Real, 3, 4> Pr = (QHatq10.transpose() * Qq00).block<3, 4>(1, 0);
    joint_info.block<4, 3>(0, 3) = Pr.transpose();

    return true;
}

// ----------------------------------------------------------------------------------------------
bool PositionBasedRigidBodyDynamics::UpdateSliderJoint(const Vector3r &x0,
                                                       const Quaternionr &q0,
                                                       const Vector3r &x1,
                                                       const Quaternionr &q1,
                                                       Eigen::Matrix<Real, 4, 6, Eigen::DontAlign> &joint_info) {
    // jointInfo contains
    // 0:	coordinate system in body 0, where the x-axis is the slider axis (local)
    // 1:	coordinate system in body 0, where the x-axis is the slider axis (global)
    // 2:   2D vector d = P * (x0 - x1), where P projects the vector onto a plane perpendicular to the slider axis
    // 3-5:	projection matrix Pr for the rotational part

    // transform constraint coordinate system of body 0 to world space
    Quaternionr qR0;
    qR0.coeffs() = joint_info.col(0);
    joint_info.col(1) = (q0 * qR0).coeffs();
    return true;
}

// ----------------------------------------------------------------------------------------------
bool PositionBasedRigidBodyDynamics::SolveSliderJoint(Real inv_mass_0,
                                                      const Vector3r &x0,
                                                      const Matrix3r &inertia_inverse_w_0,
                                                      const Quaternionr &q0,
                                                      Real inv_mass_1,
                                                      const Vector3r &x1,
                                                      const Matrix3r &inertia_inverse_w_1,
                                                      const Quaternionr &q1,
                                                      const Eigen::Matrix<Real, 4, 6, Eigen::DontAlign> &joint_info,
                                                      Vector3r &corr_x0,
                                                      Quaternionr &corr_q0,
                                                      Vector3r &corr_x1,
                                                      Quaternionr &corr_q1) {
    // jointInfo contains
    // 0:	coordinate system in body 0, where the x-axis is the slider axis (local)
    // 1:	coordinate system in body 0, where the x-axis is the slider axis (global)
    // 2:   2D vector d = P * (x0 - x1), where P projects the vector onto a plane perpendicular to the slider axis
    // 3-5:	projection matrix Pr for the rotational part

    Eigen::Matrix<Real, 2, 3> P;
    Quaternionr qCoord;
    qCoord.coeffs() = joint_info.col(1);
    const Matrix3r &R0 = qCoord.matrix();
    P.row(0) = R0.col(1).transpose();
    P.row(1) = R0.col(2).transpose();

    const Eigen::Matrix<Real, 3, 4> &Pr = joint_info.block<4, 3>(0, 3).transpose();

    const Vector2r &d = joint_info.block<2, 1>(0, 2);

    // evaluate constraint function
    Eigen::Matrix<Real, 5, 1> C;
    C.block<2, 1>(0, 0) = P * (x0 - x1) - d;
    const Quaternionr tmp = (q0.conjugate() * q1);
    const Vector4r qVec(tmp.w(), tmp.x(), tmp.y(), tmp.z());
    C.block<3, 1>(2, 0) = Pr * qVec;

    Eigen::Matrix<Real, 4, 3, Eigen::DontAlign> Gq1;
    ComputeMatrixG(q1, Gq1);
    Eigen::Matrix<Real, 4, 4, Eigen::DontAlign> Qq0;
    ComputeMatrixQ(q0, Qq0);

    const Matrix3r t = -Pr * (Qq0.transpose() * Gq1);

    // compute K = J M^-1 J^T
    Eigen::Matrix<Real, 5, 5> K;
    K.setZero();
    if (inv_mass_0 != 0.0) {
        // Jacobian for body 0 is
        //
        // (P     0)
        // (0     t)
        //
        // where I_3 is the identity matrix and t = -Pr * (Qq0^T * Gq1)
        //
        // J M^-1 J^T =
        // ( P (1/m I_3) P^T     0               )
        // ( 0                   t * J0^-1 t^T   )

        K.block<2, 2>(0, 0) = inv_mass_0 * P * P.transpose();
        K.block<3, 3>(2, 2) = t * inertia_inverse_w_0 * t.transpose();
    }
    if (inv_mass_1 != 0.0) {
        // Jacobian for body 1 is
        //
        // ( -P    0    )
        // ( 0     -t   )
        //
        // where I_3 is the identity matrix and t = -Pr * (Qq0^T * Gq1)
        //
        // J M^-1 J^T =
        // ( P (1/m I_3) P^T     0               )
        // ( 0                   t * J1^-1 t^T   )

        K.block<2, 2>(0, 0) += inv_mass_1 * P * P.transpose();
        K.block<3, 3>(2, 2) += t * inertia_inverse_w_1 * t.transpose();
    }

    const Eigen::Matrix<Real, 5, 1> lambda = K.llt().solve(-C);

    const Vector3r pt = P.transpose() * lambda.block<2, 1>(0, 0);
    const Vector3r amt = t.transpose() * lambda.block<3, 1>(2, 0);

    if (inv_mass_0 != 0.0) {
        corr_x0 = inv_mass_0 * pt;
        const Vector3r ot = inertia_inverse_w_0 * amt;
        const Quaternionr otQ(0.0, ot[0], ot[1], ot[2]);
        corr_q0.coeffs() = 0.5 * (otQ * q0).coeffs();
    }

    if (inv_mass_1 != 0.0) {
        corr_x1 = -inv_mass_1 * pt;
        const Vector3r ot = -inertia_inverse_w_1 * amt;
        const Quaternionr otQ(0.0, ot[0], ot[1], ot[2]);
        corr_q1.coeffs() = 0.5 * (otQ * q1).coeffs();
    }

    return true;
}

//MARK: -
// ----------------------------------------------------------------------------------------------
bool PositionBasedRigidBodyDynamics::InitTargetPositionMotorSliderJoint(
        const Vector3r &x0,
        const Quaternionr &q0,
        const Vector3r &x1,
        const Quaternionr &q1,
        const Vector3r &slider_joint_axis,
        Eigen::Matrix<Real, 4, 6, Eigen::DontAlign> &joint_info) {
    // jointInfo contains
    // 0:	slider axis in body 0 (local)
    // 1:	slider axis in body 0 (global)
    // 2:   distance vector d = (x0 - x1)
    // 3-5:	projection matrix Pr for the rotational part

    // determine constraint coordinate system
    // with direction as x-axis
    Matrix3r R0;
    R0.col(0) = slider_joint_axis;
    R0.col(0).normalize();

    Vector3r v(1.0, 0.0, 0.0);
    // check if vectors are parallel
    if (fabs(v.dot(R0.col(0))) > 0.99) v = Vector3r(0.0, 1.0, 0.0);

    R0.col(1) = R0.col(0).cross(v);
    R0.col(2) = R0.col(0).cross(R0.col(1));
    R0.col(1).normalize();
    R0.col(2).normalize();

    joint_info.block<3, 1>(0, 1) = slider_joint_axis;

    // slider axis in body 0 (local)
    joint_info.block<3, 1>(0, 0) = q0.matrix().transpose() * slider_joint_axis;

    const Eigen::Matrix<Real, 2, 3> P = R0.block<3, 2>(0, 1).transpose();
    joint_info.block<3, 1>(0, 2) = (x0 - x1);

    Quaternionr qR0(R0);
    const Quaternionr q00 = (q0.conjugate() * qR0).conjugate();
    const Quaternionr q10 = (q1.conjugate() * qR0).conjugate();

    Eigen::Matrix<Real, 4, 4, Eigen::DontAlign> Qq00, QHatq10;
    ComputeMatrixQ(q00, Qq00);
    ComputeMatrixQHat(q10, QHatq10);
    Eigen::Matrix<Real, 3, 4> Pr = (QHatq10.transpose() * Qq00).block<3, 4>(1, 0);
    joint_info.block<4, 3>(0, 3) = Pr.transpose();

    return true;
}

// ----------------------------------------------------------------------------------------------
bool PositionBasedRigidBodyDynamics::UpdateTargetPositionMotorSliderJoint(
        const Vector3r &x0,
        const Quaternionr &q0,
        const Vector3r &x1,
        const Quaternionr &q1,
        Eigen::Matrix<Real, 4, 6, Eigen::DontAlign> &joint_info) {
    // jointInfo contains
    // 0:	slider axis in body 0 (local)
    // 1:	slider axis in body 0 (global)
    // 2:   distance vector d = (x0 - x1)
    // 3-5:	projection matrix Pr for the rotational part

    // transform slider axis in body 0 to world space
    joint_info.block<3, 1>(0, 1) = q0.matrix() * joint_info.block<3, 1>(0, 0);
    return true;
}

// ----------------------------------------------------------------------------------------------
bool PositionBasedRigidBodyDynamics::SolveTargetPositionMotorSliderJoint(
        Real inv_mass_0,
        const Vector3r &x0,
        const Matrix3r &inertia_inverse_w_0,
        const Quaternionr &q0,
        Real inv_mass_1,
        const Vector3r &x1,
        const Matrix3r &inertia_inverse_w_1,
        const Quaternionr &q1,
        Real target_position,
        const Eigen::Matrix<Real, 4, 6, Eigen::DontAlign> &joint_info,
        Vector3r &corr_x0,
        Quaternionr &corr_q0,
        Vector3r &corr_x1,
        Quaternionr &corr_q1) {
    // jointInfo contains
    // 0:	slider axis in body 0 (local)
    // 1:	slider axis in body 0 (global)
    // 2:   distance vector d = (x0 - x1)
    // 3-5:	projection matrix Pr for the rotational part

    const Vector3r &axis = joint_info.block<3, 1>(0, 1);

    const Eigen::Matrix<Real, 3, 4> &Pr = joint_info.block<4, 3>(0, 3).transpose();

    const Vector3r &d = joint_info.block<3, 1>(0, 2);

    // evaluate constraint function
    Eigen::Matrix<Real, 6, 1> C;
    C.block<3, 1>(0, 0) = (x0 - x1) - d;
    C.block<3, 1>(0, 0) += target_position * axis;
    const Quaternionr tmp = (q0.conjugate() * q1);
    const Vector4r qVec(tmp.w(), tmp.x(), tmp.y(), tmp.z());
    C.block<3, 1>(3, 0) = Pr * qVec;

    Eigen::Matrix<Real, 4, 3, Eigen::DontAlign> Gq1;
    ComputeMatrixG(q1, Gq1);
    Eigen::Matrix<Real, 4, 4, Eigen::DontAlign> Qq0;
    ComputeMatrixQ(q0, Qq0);

    const Matrix3r t = -Pr * (Qq0.transpose() * Gq1);

    Eigen::Matrix<Real, 6, 6> K;
    K.setZero();
    if (inv_mass_0 != 0.0) {
        // Jacobian for body 0 is
        //
        // (I     0)
        // (0     t)
        //
        // where I_3 is the identity matrix and t = -Pr * (Qq0^T * Gq1)
        //
        // J M^-1 J^T =
        // ( (1/m I_3) I	     0               )
        // ( 0                   t * J0^-1 t^T   )

        K.block<3, 3>(0, 0) = inv_mass_0 * Matrix3r::Identity();
        K.block<3, 3>(3, 3) = t * inertia_inverse_w_0 * t.transpose();
    }
    if (inv_mass_1 != 0.0) {
        // Jacobian for body 1 is
        //
        // ( -I    0    )
        // ( 0     -t   )
        //
        // where I_3 is the identity matrix and t = -Pr * (Qq0^T * Gq1)
        //
        // J M^-1 J^T =
        // ( (1/m I_3) I	     0               )
        // ( 0                   t * J1^-1 t^T   )

        K.block<3, 3>(0, 0) += inv_mass_1 * Matrix3r::Identity();
        K.block<3, 3>(3, 3) += t * inertia_inverse_w_1 * t.transpose();
    }

    const Eigen::Matrix<Real, 6, 1> lambda = K.llt().solve(-C);

    const Vector3r pt = lambda.block<3, 1>(0, 0);
    const Vector3r amt = t.transpose() * lambda.block<3, 1>(3, 0);

    if (inv_mass_0 != 0.0) {
        corr_x0 = inv_mass_0 * pt;
        const Vector3r ot = inertia_inverse_w_0 * amt;
        const Quaternionr otQ(0.0, ot[0], ot[1], ot[2]);
        corr_q0.coeffs() = 0.5 * (otQ * q0).coeffs();
    }

    if (inv_mass_1 != 0.0) {
        corr_x1 = -inv_mass_1 * pt;
        const Vector3r ot = -inertia_inverse_w_1 * amt;
        const Quaternionr otQ(0.0, ot[0], ot[1], ot[2]);
        corr_q1.coeffs() = 0.5 * (otQ * q1).coeffs();
    }

    return true;
}

//MARK: -
// ----------------------------------------------------------------------------------------------
bool PositionBasedRigidBodyDynamics::InitTargetVelocityMotorSliderJoint(
        const Vector3r &x0,
        const Quaternionr &q0,
        const Vector3r &x1,
        const Quaternionr &q1,
        const Vector3r &slider_joint_axis,
        Eigen::Matrix<Real, 4, 6, Eigen::DontAlign> &joint_info) {
    // jointInfo contains
    // 0:	coordinate system in body 0, where the x-axis is the slider axis (local)
    // 1:	coordinate system in body 0, where the x-axis is the slider axis (global)
    // 2:   2D vector d = P * (x0 - x1), where P projects the vector onto a plane perpendicular to the slider axis
    // 3-5:	projection matrix Pr for the rotational part

    return InitSliderJoint(x0, q0, x1, q1, slider_joint_axis, joint_info);
}

// ----------------------------------------------------------------------------------------------
bool PositionBasedRigidBodyDynamics::UpdateTargetVelocityMotorSliderJoint(
        const Vector3r &x0,
        const Quaternionr &q0,
        const Vector3r &x1,
        const Quaternionr &q1,
        Eigen::Matrix<Real, 4, 6, Eigen::DontAlign> &joint_info) {
    // jointInfo contains
    // 0:	coordinate system in body 0, where the x-axis is the slider axis (local)
    // 1:	coordinate system in body 0, where the x-axis is the slider axis (global)
    // 2:   2D vector d = P * (x0 - x1), where P projects the vector onto a plane perpendicular to the slider axis
    // 3-5:	projection matrix Pr for the rotational part

    return UpdateSliderJoint(x0, q0, x1, q1, joint_info);
}

// ----------------------------------------------------------------------------------------------
bool PositionBasedRigidBodyDynamics::SolveTargetVelocityMotorSliderJoint(
        Real inv_mass_0,
        const Vector3r &x0,
        const Matrix3r &inertia_inverse_w_0,
        const Quaternionr &q0,
        Real inv_mass_1,
        const Vector3r &x1,
        const Matrix3r &inertia_inverse_w_1,
        const Quaternionr &q1,
        const Eigen::Matrix<Real, 4, 6, Eigen::DontAlign> &joint_info,
        Vector3r &corr_x0,
        Quaternionr &corr_q0,
        Vector3r &corr_x1,
        Quaternionr &corr_q1) {
    return SolveSliderJoint(inv_mass_0, x0, inertia_inverse_w_0, q0, inv_mass_1, x1, inertia_inverse_w_1, q1,
                            joint_info, corr_x0, corr_q0, corr_x1, corr_q1);
}

// ----------------------------------------------------------------------------------------------
bool PositionBasedRigidBodyDynamics::VelocitySolveTargetVelocityMotorSliderJoint(
        Real inv_mass_0,
        const Vector3r &x0,
        const Vector3r &v0,
        const Matrix3r &inertia_inverse_w_0,
        const Quaternionr &q0,
        const Vector3r &omega_0,
        Real inv_mass_1,
        const Vector3r &x1,
        const Vector3r &v1,
        const Matrix3r &inertia_inverse_w_1,
        const Quaternionr &q1,
        const Vector3r &omega_1,
        Real target_velocity,
        const Eigen::Matrix<Real, 4, 6, Eigen::DontAlign> &joint_info,
        Vector3r &corr_v0,
        Vector3r &corr_omega0,
        Vector3r &corr_v1,
        Vector3r &corr_omega1) {
    // jointInfo contains
    // 0:	coordinate system in body 0, where the x-axis is the slider axis (local)
    // 1:	coordinate system in body 0, where the x-axis is the slider axis (global)
    // 2:   2D vector d = P * (x0 - x1), where P projects the vector onto a plane perpendicular to the slider axis
    // 3-5:	projection matrix Pr for the rotational part

    Eigen::Matrix<Real, 6, 1> C;
    Quaternionr qCoord;
    qCoord.coeffs() = joint_info.col(1);
    const Matrix3r &R0 = qCoord.matrix();

    const Vector3r &axis0 = R0.col(0);

    // evaluate constraint function
    Vector3r deltaOmega = omega_0 - omega_1;
    C.block<3, 1>(0, 0) = (v0 - v1) + target_velocity * axis0;
    C.block<3, 1>(3, 0) = deltaOmega;

    // compute K= J M^1 J^T
    const Eigen::Matrix<Real, 3, 4> &Pr = joint_info.block<4, 3>(0, 3).transpose();

    Eigen::Matrix<Real, 4, 3, Eigen::DontAlign> Gq1;
    ComputeMatrixG(q1, Gq1);
    Eigen::Matrix<Real, 4, 4, Eigen::DontAlign> Qq0;
    ComputeMatrixQ(q0, Qq0);

    const Matrix3r t = -Pr * (Qq0.transpose() * Gq1);

    Eigen::Matrix<Real, 6, 6> K;
    K.setZero();
    if (inv_mass_0 != 0.0) {
        // Jacobian for body 0 is
        //
        // (I     0)
        // (0     t)
        //
        // where I_3 is the identity matrix and t = -Pr * (Qq0^T * Gq1)
        //
        // J M^-1 J^T =
        // ( (1/m I_3) I	     0               )
        // ( 0                   t * J0^-1 t^T   )

        K.block<3, 3>(0, 0) = inv_mass_0 * Matrix3r::Identity();
        K.block<3, 3>(3, 3) = t * inertia_inverse_w_0 * t.transpose();
    }
    if (inv_mass_1 != 0.0) {
        // Jacobian for body 1 is
        //
        // ( -I     0    )
        // ( 0     -t   )
        //
        // where I_3 is the identity matrix and t = -Pr * (Qq0^T * Gq1)
        //
        // J M^-1 J^T =
        // ( (1/m I_3) I	     0               )
        // ( 0                   t * J1^-1 t^T   )

        K.block<3, 3>(0, 0) += inv_mass_1 * Matrix3r::Identity();
        K.block<3, 3>(3, 3) += t * inertia_inverse_w_1 * t.transpose();
    }

    const Eigen::Matrix<Real, 6, 1> lambda = K.llt().solve(-C);

    const Vector3r p = lambda.block<3, 1>(0, 0);
    const Vector3r angMomentum = lambda.block<3, 1>(3, 0);

    if (inv_mass_0 != 0.0) {
        corr_v0 = inv_mass_0 * p;
        corr_omega0 = inertia_inverse_w_0 * angMomentum;
    }

    if (inv_mass_1 != 0.0) {
        corr_v1 = -inv_mass_1 * p;
        corr_omega1 = -inertia_inverse_w_1 * angMomentum;
    }

    return true;
}

//MARK: -
// ----------------------------------------------------------------------------------------------
bool PositionBasedRigidBodyDynamics::InitTargetAngleMotorHingeJoint(
        const Vector3r &x0,
        const Quaternionr &q0,
        const Vector3r &x1,
        const Quaternionr &q1,
        const Vector3r &hinge_joint_position,
        const Vector3r &hinge_joint_axis,
        Eigen::Matrix<Real, 4, 8, Eigen::DontAlign> &joint_info) {
    // jointInfo contains
    // 0-2:	projection matrix Pr for the rotational part
    // 3:	connector in body 0 (local)
    // 4:	connector in body 1 (local)
    // 5:	connector in body 0 (global)
    // 6:	connector in body 1 (global)
    // 7:	hinge axis in body 0 (local) used for rendering

    // transform in local coordinates
    const Matrix3r rot0T = q0.matrix().transpose();
    const Matrix3r rot1T = q1.matrix().transpose();

    // connector in body 0 (local)
    joint_info.block<3, 1>(0, 3) = rot0T * (hinge_joint_position - x0);
    // connector in body 1 (local)
    joint_info.block<3, 1>(0, 4) = rot1T * (hinge_joint_position - x1);
    // connector in body 0 (global)
    joint_info.block<3, 1>(0, 5) = hinge_joint_position;
    // connector in body 1 (global)
    joint_info.block<3, 1>(0, 6) = hinge_joint_position;
    joint_info.block<3, 1>(0, 7) = rot0T * hinge_joint_axis;

    // determine constraint coordinate system
    // with direction as x-axis
    Matrix3r R0;
    R0.col(0) = hinge_joint_axis;
    R0.col(0).normalize();

    Vector3r v(1.0, 0.0, 0.0);
    // check if vectors are parallel
    if (fabs(v.dot(R0.col(0))) > 0.99) v = Vector3r(0.0, 1.0, 0.0);

    R0.col(1) = R0.col(0).cross(v);
    R0.col(2) = R0.col(0).cross(R0.col(1));
    R0.col(1).normalize();
    R0.col(2).normalize();

    Quaternionr qR0(R0);
    const Quaternionr q00 = (q0.conjugate() * qR0).conjugate();
    const Quaternionr q10 = (q1.conjugate() * qR0).conjugate();

    Eigen::Matrix<Real, 4, 4, Eigen::DontAlign> Qq00, QHatq10;
    ComputeMatrixQ(q00, Qq00);
    ComputeMatrixQHat(q10, QHatq10);
    Eigen::Matrix<Real, 3, 4> Pr = (QHatq10.transpose() * Qq00).block<3, 4>(1, 0);
    joint_info.block<4, 3>(0, 0) = Pr.transpose();

    return true;
}

// ----------------------------------------------------------------------------------------------
bool PositionBasedRigidBodyDynamics::UpdateTargetAngleMotorHingeJoint(
        const Vector3r &x0,
        const Quaternionr &q0,
        const Vector3r &x1,
        const Quaternionr &q1,
        Eigen::Matrix<Real, 4, 8, Eigen::DontAlign> &joint_info) {
    // jointInfo contains
    // 0-2:	projection matrix Pr for the rotational part
    // 3:	connector in body 0 (local)
    // 4:	connector in body 1 (local)
    // 5:	connector in body 0 (global)
    // 6:	connector in body 1 (global)
    // 7:	hinge axis in body 0 (local) used for rendering

    // compute world space positions of connectors
    const Matrix3r rot0 = q0.matrix();
    const Matrix3r rot1 = q1.matrix();
    joint_info.block<3, 1>(0, 5) = rot0 * joint_info.block<3, 1>(0, 3) + x0;
    joint_info.block<3, 1>(0, 6) = rot1 * joint_info.block<3, 1>(0, 4) + x1;

    return true;
}

// ----------------------------------------------------------------------------------------------
bool PositionBasedRigidBodyDynamics::SolveTargetAngleMotorHingeJoint(
        Real inv_mass_0,
        const Vector3r &x0,
        const Matrix3r &inertia_inverse_w_0,
        const Quaternionr &q0,
        Real inv_mass_1,
        const Vector3r &x1,
        const Matrix3r &inertia_inverse_w_1,
        const Quaternionr &q1,
        Real target_angle,
        const Eigen::Matrix<Real, 4, 8, Eigen::DontAlign> &joint_info,
        Vector3r &corr_x0,
        Quaternionr &corr_q0,
        Vector3r &corr_x1,
        Quaternionr &corr_q1) {
    // jointInfo contains
    // 0-2:	projection matrix Pr for the rotational part
    // 3:	connector in body 0 (local)
    // 4:	connector in body 1 (local)
    // 5:	connector in body 0 (global)
    // 6:	connector in body 1 (global)
    // 7:	hinge axis in body 0 (local) used for rendering
    const Vector3r &c0 = joint_info.block<3, 1>(0, 5);
    const Vector3r &c1 = joint_info.block<3, 1>(0, 6);

    const Eigen::Matrix<Real, 3, 4> &Pr = joint_info.block<4, 3>(0, 0).transpose();

    // evaluate constraint function
    Eigen::Matrix<Real, 6, 1> C;
    C.block<3, 1>(0, 0) = (c0 - c1);

    const Quaternionr tmp = (q0.conjugate() * q1);
    const Vector4r qVec(tmp.w(), tmp.x(), tmp.y(), tmp.z());
    C.block<3, 1>(3, 0) = Pr * qVec;
    C(3, 0) -= sin(static_cast<Real>(0.5) * target_angle);

    const Vector3r r0 = c0 - x0;
    const Vector3r r1 = c1 - x1;
    Matrix3r r0_star, r1_star;
    MathFunctions::CrossProductMatrix(r0, r0_star);
    MathFunctions::CrossProductMatrix(r1, r1_star);

    Eigen::Matrix<Real, 4, 3, Eigen::DontAlign> Gq1;
    ComputeMatrixG(q1, Gq1);
    Eigen::Matrix<Real, 4, 4, Eigen::DontAlign> Qq0;
    ComputeMatrixQ(q0, Qq0);

    const Matrix3r t = -Pr * (Qq0.transpose() * Gq1);

    // compute K = J M^-1 J^T
    Eigen::Matrix<Real, 6, 6> K;
    K.setZero();
    if (inv_mass_0 != 0.0) {
        // Jacobian for body 0 is
        //
        // (I_3   -r0*)
        // (0     t)
        //
        // where I_3 is the identity matrix, r0* is the cross product matrix of r0 and
        // t = -Pr * (Qq0^T * Gq1)
        //
        // J M^-1 J^T =
        // ( 1/m I_3-r0 * J0^-1 * r0*    -r0 * J0^-1 * t^T   )
        // ( (-r0 * J0^-1 * t^T)^T       t * J0^-1 * t^T     )

        Matrix3r K00;
        ComputeMatrixK(c0, inv_mass_0, x0, inertia_inverse_w_0, K00);

        K.block<3, 3>(0, 0) = K00;
        K.block<3, 3>(0, 3) = -r0_star * inertia_inverse_w_0 * t.transpose();
        K.block<3, 3>(3, 0) = K.block<3, 3>(0, 3).transpose();
        K.block<3, 3>(3, 3) = t * inertia_inverse_w_0 * t.transpose();
    }
    if (inv_mass_1 != 0.0) {
        // Jacobian for body 1 is
        //
        // (-I_3   r1*)
        // (0     -t)
        //
        // where I_3 is the identity matrix, r1* is the cross product matrix of r1 and
        // t = -Pr * (Qq0^T * Gq1)
        //
        // J M^-1 J^T =
        // ( 1/m I_3-r1 * J1^-1 * r1*    r1 * J1^-1 * t^T   )
        // ( (r1 * J1^-1 * t^T)^T       t * J1^-1 * t^T     )

        Matrix3r K11;
        ComputeMatrixK(c1, inv_mass_1, x1, inertia_inverse_w_1, K11);

        K.block<3, 3>(0, 0) += K11;
        Eigen::Matrix<Real, 3, 3> K03 = -r1_star * inertia_inverse_w_1 * t.transpose();
        K.block<3, 3>(0, 3) += K03;
        K.block<3, 3>(3, 0) += K03.transpose();
        K.block<3, 3>(3, 3) += t * inertia_inverse_w_1 * t.transpose();
    }

    auto &llt = K.llt();
    const Eigen::Matrix<Real, 6, 1> lambda = llt.solve(-C);
    if (llt.info() != Eigen::Success) {
        std::cerr << "Cholesky decomposition failed.";
        std::cerr << llt.info();
        return false;
    }

    const Vector3r pt = lambda.block<3, 1>(0, 0);
    const Vector3r amt = t.transpose() * lambda.block<3, 1>(3, 0);

    if (inv_mass_0 != 0.0) {
        corr_x0 = inv_mass_0 * pt;
        const Vector3r ot = (inertia_inverse_w_0 * (r0.cross(pt) + amt));
        const Quaternionr otQ(0.0, ot[0], ot[1], ot[2]);
        corr_q0.coeffs() = 0.5 * (otQ * q0).coeffs();
    }

    if (inv_mass_1 != 0.0) {
        corr_x1 = -inv_mass_1 * pt;
        const Vector3r ot = (inertia_inverse_w_1 * (r1.cross(-pt) - amt));
        const Quaternionr otQ(0.0, ot[0], ot[1], ot[2]);
        corr_q1.coeffs() = 0.5 * (otQ * q1).coeffs();

        // std::cout << pt.transpose() << ",      " << ot.transpose() << "\n";
    }

    return true;
}

//MARK: -
// ----------------------------------------------------------------------------------------------
bool PositionBasedRigidBodyDynamics::InitTargetVelocityMotorHingeJoint(
        const Vector3r &x0,
        const Quaternionr &q0,
        const Vector3r &x1,
        const Quaternionr &q1,
        const Vector3r &hinge_joint_position,
        const Vector3r &hinge_joint_axis,
        Eigen::Matrix<Real, 4, 8, Eigen::DontAlign> &joint_info) {
    // jointInfo contains
    // 0-1:	projection matrix Pr for the rotational part
    // 2:	connector in body 0 (local)
    // 3:	connector in body 1 (local)
    // 4:	connector in body 0 (global)
    // 5:	connector in body 1 (global)
    // 6:	hinge axis in body 0 (local)
    // 7:   hinge axis in body 0 (global)

    // transform in local coordinates
    const Matrix3r rot0T = q0.matrix().transpose();
    const Matrix3r rot1T = q1.matrix().transpose();

    // connector in body 0 (local)
    joint_info.block<3, 1>(0, 2) = rot0T * (hinge_joint_position - x0);
    // connector in body 1 (local)
    joint_info.block<3, 1>(0, 3) = rot1T * (hinge_joint_position - x1);
    // connector in body 0 (global)
    joint_info.block<3, 1>(0, 4) = hinge_joint_position;
    // connector in body 1 (global)
    joint_info.block<3, 1>(0, 5) = hinge_joint_position;

    // determine constraint coordinate system
    // with direction as x-axis
    Matrix3r R0;
    R0.col(0) = hinge_joint_axis;
    R0.col(0).normalize();

    Vector3r v(1.0, 0.0, 0.0);
    // check if vectors are parallel
    if (fabs(v.dot(R0.col(0))) > 0.99) v = Vector3r(0.0, 1.0, 0.0);

    R0.col(1) = R0.col(0).cross(v);
    R0.col(2) = R0.col(0).cross(R0.col(1));
    R0.col(1).normalize();
    R0.col(2).normalize();

    joint_info.block<3, 1>(0, 6) = rot0T * hinge_joint_axis;
    joint_info.block<3, 1>(0, 7) = hinge_joint_axis;

    Quaternionr qR0(R0);
    const Quaternionr q00 = (q0.conjugate() * qR0).conjugate();
    const Quaternionr q10 = (q1.conjugate() * qR0).conjugate();

    Eigen::Matrix<Real, 4, 4, Eigen::DontAlign> Qq00, QHatq10;
    ComputeMatrixQ(q00, Qq00);
    ComputeMatrixQHat(q10, QHatq10);
    Eigen::Matrix<Real, 2, 4> Pr = (QHatq10.transpose() * Qq00).block<2, 4>(2, 0);
    joint_info.block<4, 2>(0, 0) = Pr.transpose();

    return true;
}

// ----------------------------------------------------------------------------------------------
bool PositionBasedRigidBodyDynamics::UpdateTargetVelocityMotorHingeJoint(
        const Vector3r &x0,
        const Quaternionr &q0,
        const Vector3r &x1,
        const Quaternionr &q1,
        Eigen::Matrix<Real, 4, 8, Eigen::DontAlign> &joint_info) {
    // jointInfo contains
    // 0-1:	projection matrix Pr for the rotational part
    // 2:	connector in body 0 (local)
    // 3:	connector in body 1 (local)
    // 4:	connector in body 0 (global)
    // 5:	connector in body 1 (global)
    // 6:	hinge axis in body 0 (local)
    // 7:   hinge axis in body 0 (global)

    // compute world space positions of connectors
    const Matrix3r rot0 = q0.matrix();
    const Matrix3r rot1 = q1.matrix();
    joint_info.block<3, 1>(0, 4) = rot0 * joint_info.block<3, 1>(0, 2) + x0;
    joint_info.block<3, 1>(0, 5) = rot1 * joint_info.block<3, 1>(0, 3) + x1;

    joint_info.block<3, 1>(0, 7) = rot0 * joint_info.block<3, 1>(0, 6);

    return true;
}

// ----------------------------------------------------------------------------------------------
bool PositionBasedRigidBodyDynamics::SolveTargetVelocityMotorHingeJoint(
        Real inv_mass_0,
        const Vector3r &x0,
        const Matrix3r &inertia_inverse_w_0,
        const Quaternionr &q0,
        Real inv_mass_1,
        const Vector3r &x1,
        const Matrix3r &inertia_inverse_w_1,
        const Quaternionr &q1,
        const Eigen::Matrix<Real, 4, 8, Eigen::DontAlign> &joint_info,
        Vector3r &corr_x0,
        Quaternionr &corr_q0,
        Vector3r &corr_x1,
        Quaternionr &corr_q1) {
    Eigen::Matrix<Real, 4, 7> hingeJointInfo = joint_info.block<4, 7>(0, 0);
    const bool res = SolveHingeJoint(inv_mass_0, x0, inertia_inverse_w_0, q0, inv_mass_1, x1, inertia_inverse_w_1, q1,
                                     hingeJointInfo, corr_x0, corr_q0, corr_x1, corr_q1);
    return res;
}

// ----------------------------------------------------------------------------------------------
bool PositionBasedRigidBodyDynamics::VelocitySolveTargetVelocityMotorHingeJoint(
        Real inv_mass_0,
        const Vector3r &x0,
        const Vector3r &v0,
        const Matrix3r &inertia_inverse_w_0,
        const Vector3r &omega0,
        Real inv_mass_1,
        const Vector3r &x1,
        const Vector3r &v1,
        const Matrix3r &inertia_inverse_w_1,
        const Vector3r &omega1,
        Real target_angular_velocity,
        const Eigen::Matrix<Real, 4, 8, Eigen::DontAlign> &joint_info,
        Vector3r &corr_v0,
        Vector3r &corr_omega0,
        Vector3r &corr_v1,
        Vector3r &corr_omega1) {
    // jointInfo contains
    // 0-1:	projection matrix Pr for the rotational part
    // 2:	connector in body 0 (local)
    // 3:	connector in body 1 (local)
    // 4:	connector in body 0 (global)
    // 5:	connector in body 1 (global)
    // 6:	hinge axis in body 0 (local)
    // 7:   hinge axis in body 0 (global)

    const Vector3r &axis0 = joint_info.block<3, 1>(0, 7);

    const Vector3r &c0 = joint_info.block<3, 1>(0, 4);
    const Vector3r &c1 = joint_info.block<3, 1>(0, 5);

    Vector3r deltaOmega = omega0 - omega1;
    Eigen::Matrix<Real, 6, 1> C;
    C.block<3, 1>(0, 0) = v0 - v1;
    C.block<3, 1>(3, 0) = deltaOmega + target_angular_velocity * axis0;

    // compute matrix J M^-1 J^T = K
    const Vector3r r0 = c0 - x0;
    const Vector3r r1 = c1 - x1;
    Matrix3r r0_star, r1_star;
    MathFunctions::CrossProductMatrix(r0, r0_star);
    MathFunctions::CrossProductMatrix(r1, r1_star);

    Eigen::Matrix<Real, 6, 6> K;
    K.setZero();
    if (inv_mass_0 != 0.0) {
        // Jacobian for body 0 is
        //
        // (I_3   -r0*)
        // (0     I_3)
        //
        // where I_3 is the identity matrix and r0* is the cross product matrix of r0
        //
        // J M^-1 J^T =
        // ( 1/m I_3-r0 * J0^-1 * r0*    -r0 * J0^-1)
        // ( (-r0 * J0^-1)^T			 J0^-1      )

        Matrix3r K00;
        ComputeMatrixK(c0, inv_mass_0, x0, inertia_inverse_w_0, K00);

        K.block<3, 3>(0, 0) = K00;
        K.block<3, 3>(0, 3) = -r0_star * inertia_inverse_w_0;
        K.block<3, 3>(3, 0) = K.block<3, 3>(0, 3).transpose();
        K.block<3, 3>(3, 3) = inertia_inverse_w_0;
    }
    if (inv_mass_1 != 0.0) {
        // Jacobian for body 1 is
        //
        // ( -I_3   r1*  )
        // ( 0     -I_3  )
        //
        // where I_3 is the identity matrix and r1* is the cross product matrix of r1
        //
        // J M^-1 J^T =
        // ( 1/m I_3-r1 * J1^-1 * r1*    -r1 * J1^-1)
        // ( (-r1 * J1^-1)^T			 J1^-1      )

        Matrix3r K11;
        ComputeMatrixK(c1, inv_mass_1, x1, inertia_inverse_w_1, K11);

        K.block<3, 3>(0, 0) += K11;
        Matrix3r K03 = -r1_star * inertia_inverse_w_1;
        K.block<3, 3>(0, 3) += K03;
        K.block<3, 3>(3, 0) += K03.transpose();
        K.block<3, 3>(3, 3) += inertia_inverse_w_1;
    }

    const Eigen::Matrix<Real, 6, 1> lambda = K.llt().solve(-C);

    const Vector3r p = lambda.block<3, 1>(0, 0);

    Vector3r angMomentum = lambda.block<3, 1>(3, 0);

    if (inv_mass_0 != 0.0) {
        corr_v0 = inv_mass_0 * p;
        corr_omega0 = (inertia_inverse_w_0 * (r0.cross(p) + angMomentum));
    }

    if (inv_mass_1 != 0.0) {
        corr_v1 = -inv_mass_1 * p;
        corr_omega1 = (inertia_inverse_w_1 * (r1.cross(-p) - angMomentum));
    }

    return true;
}

//MARK: -
// ----------------------------------------------------------------------------------------------
bool PositionBasedRigidBodyDynamics::InitDamperJoint(const Vector3r &x0,
                                                     const Quaternionr &q0,
                                                     const Vector3r &x1,
                                                     const Quaternionr &q1,
                                                     const Vector3r &direction,
                                                     Eigen::Matrix<Real, 4, 6, Eigen::DontAlign> &joint_info) {
    // jointInfo contains
    // 0:	coordinate system in body 0, where the x-axis is the slider axis (local)
    // 1:	coordinate system in body 0, where the x-axis is the slider axis (global)
    // 2:   3D vector d = R^T * (x0 - x1), where R is a rotation matrix with the slider axis as first column
    // 3-5:	projection matrix Pr for the rotational part

    // determine constraint coordinate system
    // with direction as x-axis
    Matrix3r R0;
    R0.col(0) = direction;
    R0.col(0).normalize();

    Vector3r v(1.0, 0.0, 0.0);
    // check if vectors are parallel
    if (fabs(v.dot(R0.col(0))) > 0.99) v = Vector3r(0.0, 1.0, 0.0);

    R0.col(1) = R0.col(0).cross(v);
    R0.col(2) = R0.col(0).cross(R0.col(1));
    R0.col(1).normalize();
    R0.col(2).normalize();

    Quaternionr qR0(R0);
    joint_info.col(1) = qR0.coeffs();

    // coordinate system of body 0 (local)
    joint_info.col(0) = (q0.conjugate() * qR0).coeffs();

    joint_info.block<3, 1>(0, 2) = R0.transpose() * (x0 - x1);

    const Quaternionr q00 = (q0.conjugate() * qR0).conjugate();
    const Quaternionr q10 = (q1.conjugate() * qR0).conjugate();

    Eigen::Matrix<Real, 4, 4, Eigen::DontAlign> Qq00, QHatq10;
    ComputeMatrixQ(q00, Qq00);
    ComputeMatrixQHat(q10, QHatq10);
    Eigen::Matrix<Real, 3, 4> Pr = (QHatq10.transpose() * Qq00).block<3, 4>(1, 0);
    joint_info.block<4, 3>(0, 3) = Pr.transpose();

    return true;
}

// ----------------------------------------------------------------------------------------------
bool PositionBasedRigidBodyDynamics::UpdateDamperJoint(const Vector3r &x0,
                                                       const Quaternionr &q0,
                                                       const Vector3r &x1,
                                                       const Quaternionr &q1,
                                                       Eigen::Matrix<Real, 4, 6, Eigen::DontAlign> &joint_info) {
    // jointInfo contains
    // 0:	coordinate system in body 0, where the x-axis is the slider axis (local)
    // 1:	coordinate system in body 0, where the x-axis is the slider axis (global)
    // 2:   3D vector d = R^T * (x0 - x1), where R is a rotation matrix with the slider axis as first column
    // 3-5:	projection matrix Pr for the rotational part

    // transform constraint coordinate system of body 0 to world space
    Quaternionr qR0;
    qR0.coeffs() = joint_info.col(0);
    joint_info.col(1) = (q0 * qR0).coeffs();
    return true;
}

// ----------------------------------------------------------------------------------------------
bool PositionBasedRigidBodyDynamics::SolveDamperJoint(Real inv_mass_0,
                                                      const Vector3r &x0,
                                                      const Matrix3r &inertia_inverse_w_0,
                                                      const Quaternionr &q0,
                                                      Real inv_mass_1,
                                                      const Vector3r &x1,
                                                      const Matrix3r &inertia_inverse_w_1,
                                                      const Quaternionr &q1,
                                                      Real stiffness,
                                                      Real dt,
                                                      const Eigen::Matrix<Real, 4, 6, Eigen::DontAlign> &joint_info,
                                                      Real &lambda,
                                                      Vector3r &corr_x0,
                                                      Quaternionr &corr_q0,
                                                      Vector3r &corr_x1,
                                                      Quaternionr &corr_q1) {
    // jointInfo contains
    // 0:	coordinate system in body 0, where the x-axis is the slider axis (local)
    // 1:	coordinate system in body 0, where the x-axis is the slider axis (global)
    // 2:   3D vector d = R^T * (x0 - x1), where R is a rotation matrix with the slider axis as first column
    // 3-5:	projection matrix Pr for the rotational part

    Quaternionr qCoord;
    qCoord.coeffs() = joint_info.col(1);
    const Matrix3r &R0 = qCoord.matrix();

    const Eigen::Matrix<Real, 3, 4> &Pr = joint_info.block<4, 3>(0, 3).transpose();

    const Vector3r &d = joint_info.block<3, 1>(0, 2);

    // evaluate constraint function
    Eigen::Matrix<Real, 6, 1> C;
    C.block<3, 1>(0, 0) = R0.transpose() * (x0 - x1) - d;
    const Quaternionr tmp = (q0.conjugate() * q1);
    const Vector4r qVec(tmp.w(), tmp.x(), tmp.y(), tmp.z());
    C.block<3, 1>(3, 0) = Pr * qVec;

    Eigen::Matrix<Real, 4, 3, Eigen::DontAlign> Gq1;
    ComputeMatrixG(q1, Gq1);
    Eigen::Matrix<Real, 4, 4, Eigen::DontAlign> Qq0;
    ComputeMatrixQ(q0, Qq0);

    const Matrix3r t = -Pr * (Qq0.transpose() * Gq1);

    Eigen::Matrix<Real, 6, 6> K;
    K.setZero();
    if (inv_mass_0 != 0.0) {
        // Jacobian for body 0 is
        //
        // (I     0)
        // (0     t)
        //
        // where I_3 is the identity matrix and t = -Pr * (Qq0^T * Gq1)
        //
        // J M^-1 J^T =
        // ( (1/m I_3)		     0               )
        // ( 0                   t * J0^-1 t^T   )

        K.block<3, 3>(0, 0) = inv_mass_0 * Matrix3r::Identity();
        K.block<3, 3>(3, 3) = t * inertia_inverse_w_0 * t.transpose();
    }
    if (inv_mass_1 != 0.0) {
        // Jacobian for body 1 is
        //
        // ( -I    0    )
        // ( 0     -t   )
        //
        // where I_3 is the identity matrix and t = -Pr * (Qq0^T * Gq1)
        //
        // J M^-1 J^T =
        // ( (1/m I_3)  	     0               )
        // ( 0                   t * J1^-1 t^T   )

        K.block<3, 3>(0, 0) += inv_mass_1 * Matrix3r::Identity();
        K.block<3, 3>(3, 3) += t * inertia_inverse_w_1 * t.transpose();
    }

    Real alpha = 0.0;
    if (stiffness != 0.0) {
        alpha = static_cast<Real>(1.0) / (stiffness * dt * dt);
        K(0, 0) += alpha;
    }

    C(0, 0) += alpha * lambda;
    const Eigen::Matrix<Real, 6, 1> delta_lambda = K.llt().solve(-C);

    lambda += delta_lambda(0, 0);

    const Vector3r pt = R0 * delta_lambda.block<3, 1>(0, 0);
    const Vector3r amt = t.transpose() * delta_lambda.block<3, 1>(3, 0);

    if (inv_mass_0 != 0.0) {
        corr_x0 = inv_mass_0 * pt;
        const Vector3r ot = inertia_inverse_w_0 * amt;
        const Quaternionr otQ(0.0, ot[0], ot[1], ot[2]);
        corr_q0.coeffs() = 0.5 * (otQ * q0).coeffs();
    }

    if (inv_mass_1 != 0.0) {
        corr_x1 = -inv_mass_1 * pt;
        const Vector3r ot = -inertia_inverse_w_1 * amt;
        const Quaternionr otQ(0.0, ot[0], ot[1], ot[2]);
        corr_q1.coeffs() = 0.5 * (otQ * q1).coeffs();
    }

    return true;
}

//MARK: -
// ----------------------------------------------------------------------------------------------
bool PositionBasedRigidBodyDynamics::InitRigidBodyParticleBallJoint(
        const Vector3r &x0,
        const Quaternionr &q0,
        const Vector3r &x1,
        Eigen::Matrix<Real, 3, 2, Eigen::DontAlign> &joint_info) {
    // jointInfo contains
    // 0:	connector in rigid body (local)
    // 1:	connector in rigid body (global)

    // transform in local coordinates
    const Matrix3r rot0T = q0.matrix().transpose();

    joint_info.col(0) = rot0T * (x1 - x0);
    joint_info.col(1) = x1;

    return true;
}

// ----------------------------------------------------------------------------------------------
bool PositionBasedRigidBodyDynamics::UpdateRigidBodyParticleBallJoint(
        const Vector3r &x0,
        const Quaternionr &q0,
        const Vector3r &x1,
        Eigen::Matrix<Real, 3, 2, Eigen::DontAlign> &joint_info) {
    // jointInfo contains
    // 0:	connector in rigid body (local)
    // 1:	connector in rigid body (global)

    // compute world space position of connector
    const Matrix3r rot0 = q0.matrix();
    joint_info.col(1) = rot0 * joint_info.col(0) + x0;

    return true;
}

// ----------------------------------------------------------------------------------------------
bool PositionBasedRigidBodyDynamics::SolveRigidBodyParticleBallJoint(
        Real inv_mass_0,
        const Vector3r &x0,
        const Matrix3r &inertia_inverse_w_0,
        const Quaternionr &q0,
        Real inv_mass_1,
        const Vector3r &x1,
        const Eigen::Matrix<Real, 3, 2, Eigen::DontAlign> &joint_info,
        Vector3r &corr_x0,
        Quaternionr &corr_q0,
        Vector3r &corr_x1) {
    // jointInfo contains
    // 0:	connector in rigid body (local)
    // 1:	connector in rigid body (global)

    const Vector3r &connector0 = joint_info.col(1);

    // Compute Kinv
    Matrix3r K1, K2;
    ComputeMatrixK(connector0, inv_mass_0, x0, inertia_inverse_w_0, K1);

    K2.setZero();
    if (inv_mass_1 != 0.0) {
        K2(0, 0) = inv_mass_1;
        K2(1, 1) = inv_mass_1;
        K2(2, 2) = inv_mass_1;
    }

    const Vector3r pt = (K1 + K2).llt().solve(x1 - connector0);

    if (inv_mass_0 != 0.0) {
        const Vector3r r0 = connector0 - x0;
        corr_x0 = inv_mass_0 * pt;

        const Vector3r ot = (inertia_inverse_w_0 * (r0.cross(pt)));
        const Quaternionr otQ(0.0, ot[0], ot[1], ot[2]);
        corr_q0.coeffs() = 0.5 * (otQ * q0).coeffs();
    }

    if (inv_mass_1 != 0.0) {
        corr_x1 = -inv_mass_1 * pt;
    }

    return true;
}

//MARK: -
// ----------------------------------------------------------------------------------------------
bool PositionBasedRigidBodyDynamics::InitRigidBodyContactConstraint(
        Real inv_mass_0,                      // inverse mass is zero if body is static
        const Vector3r &x0,                   // center of mass of body 0
        const Vector3r &v0,                   // velocity of body 0
        const Matrix3r &inertia_inverse_w_0,  // inverse inertia tensor (world space) of body 0
        const Quaternionr &q0,                // rotation of body 0
        const Vector3r &omega0,               // angular velocity of body 0
        Real inv_mass_1,                      // inverse mass is zero if body is static
        const Vector3r &x1,                   // center of mass of body 1
        const Vector3r &v1,                   // velocity of body 1
        const Matrix3r &inertia_inverse_w_1,  // inverse inertia tensor (world space) of body 1
        const Quaternionr &q1,                // rotation of body 1
        const Vector3r &omega1,               // angular velocity of body 1
        const Vector3r &cp0,                  // contact point of body 0
        const Vector3r &cp1,                  // contact point of body 1
        const Vector3r &normal,               // contact normal in body 1
        Real restitution_coeff,               // coefficient of restitution
        Eigen::Matrix<Real, 3, 5, Eigen::DontAlign> &constraint_info) {
    // constraintInfo contains
    // 0:	contact point in body 0 (global)
    // 1:	contact point in body 1 (global)
    // 2:	contact normal in body 1 (global)
    // 3:	contact tangent (global)
    // 0,4:  1.0 / normal^T * K * normal
    // 1,4: maximal impulse in tangent direction
    // 2,4: goal velocity in normal direction after collision

    // compute goal velocity in normal direction after collision
    const Vector3r r0 = cp0 - x0;
    const Vector3r r1 = cp1 - x1;

    const Vector3r u0 = v0 + omega0.cross(r0);
    const Vector3r u1 = v1 + omega1.cross(r1);
    const Vector3r u_rel = u0 - u1;
    const Real u_rel_n = normal.dot(u_rel);

    constraint_info.col(0) = cp0;
    constraint_info.col(1) = cp1;
    constraint_info.col(2) = normal;

    // tangent direction
    Vector3r t = u_rel - u_rel_n * normal;
    Real tl2 = t.squaredNorm();
    if (tl2 > 1.0e-6) t *= static_cast<Real>(1.0) / sqrt(tl2);

    constraint_info.col(3) = t;

    // determine K matrix
    Matrix3r K1, K2;
    ComputeMatrixK(cp0, inv_mass_0, x0, inertia_inverse_w_0, K1);
    ComputeMatrixK(cp1, inv_mass_1, x1, inertia_inverse_w_1, K2);
    Matrix3r K = K1 + K2;

    constraint_info(0, 4) = static_cast<Real>(1.0) / (normal.dot(K * normal));

    // maximal impulse in tangent direction
    constraint_info(1, 4) = static_cast<Real>(1.0) / (t.dot(K * t)) * u_rel.dot(t);

    // goal velocity in normal direction after collision
    constraint_info(2, 4) = 0.0;
    if (u_rel_n < 0.0) constraint_info(2, 4) = -restitution_coeff * u_rel_n;

    return true;
}

//--------------------------------------------------------------------------------------------
bool PositionBasedRigidBodyDynamics::VelocitySolveRigidBodyContactConstraint(
        Real inv_mass_0,                                               // inverse mass is zero if body is static
        const Vector3r &x0,                                            // center of mass of body 0
        const Vector3r &v0,                                            // velocity of body 0
        const Matrix3r &inertia_inverse_w_0,                           // inverse inertia tensor (world space) of body 0
        const Vector3r &omega0,                                        // angular velocity of body 0
        Real inv_mass_1,                                               // inverse mass is zero if body is static
        const Vector3r &x1,                                            // center of mass of body 1
        const Vector3r &v1,                                            // velocity of body 1
        const Matrix3r &inertia_inverse_w_1,                           // inverse inertia tensor (world space) of body 1
        const Vector3r &omega1,                                        // angular velocity of body 1
        Real stiffness,                                                // stiffness parameter of penalty impulse
        Real friction_coeff,                                           // friction coefficient
        Real &sum_impulses,                                            // sum of all impulses
        Eigen::Matrix<Real, 3, 5, Eigen::DontAlign> &constraint_info,  // precomputed contact info
        Vector3r &corr_v0,
        Vector3r &corr_omega0,
        Vector3r &corr_v1,
        Vector3r &corr_omega1) {
    // constraintInfo contains
    // 0:	contact point in body 0 (global)
    // 1:	contact point in body 1 (global)
    // 2:	contact normal in body 1 (global)
    // 3:	contact tangent (global)
    // 0,4:  1.0 / normal^T * K * normal
    // 1,4: maximal impulse in tangent direction
    // 2,4: goal velocity in normal direction after collision

    if ((inv_mass_0 == 0.0) && (inv_mass_1 == 0.0)) return false;

    const Vector3r &connector0 = constraint_info.col(0);
    const Vector3r &connector1 = constraint_info.col(1);
    const Vector3r &normal = constraint_info.col(2);
    const Vector3r &tangent = constraint_info.col(3);

    // 1.0 / normal^T * K * normal
    const Real nKn_inv = constraint_info(0, 4);

    // penetration depth
    const Real d = normal.dot(connector0 - connector1);

    // maximal impulse in tangent direction
    const Real pMax = constraint_info(1, 4);

    // goal velocity in normal direction after collision
    const Real goal_u_rel_n = constraint_info(2, 4);

    const Vector3r r0 = connector0 - x0;
    const Vector3r r1 = connector1 - x1;

    const Vector3r u0 = v0 + omega0.cross(r0);
    const Vector3r u1 = v1 + omega1.cross(r1);

    const Vector3r u_rel = u0 - u1;
    const Real u_rel_n = u_rel.dot(normal);
    const Real delta_u_reln = goal_u_rel_n - u_rel_n;

    Real correctionMagnitude = nKn_inv * delta_u_reln;

    if (correctionMagnitude < -sum_impulses) correctionMagnitude = -sum_impulses;

    // add penalty impulse to counteract penetration
    if (d < 0.0) correctionMagnitude -= stiffness * nKn_inv * d;

    Vector3r p(correctionMagnitude * normal);
    sum_impulses += correctionMagnitude;

    // dynamic friction
    const Real pn = p.dot(normal);
    if (friction_coeff * pn > pMax)
        p -= pMax * tangent;
    else if (friction_coeff * pn < -pMax)
        p += pMax * tangent;
    else
        p -= friction_coeff * pn * tangent;

    if (inv_mass_0 != 0.0) {
        corr_v0 = inv_mass_0 * p;
        corr_omega0 = inertia_inverse_w_0 * (r0.cross(p));
    }

    if (inv_mass_1 != 0.0) {
        corr_v1 = -inv_mass_1 * p;
        corr_omega1 = inertia_inverse_w_1 * (r1.cross(-p));
    }

    return true;
}

//MARK: -
// ----------------------------------------------------------------------------------------------
bool PositionBasedRigidBodyDynamics::InitParticleRigidBodyContactConstraint(
        Real invMass0,                        // inverse mass is zero if body is static
        const Vector3r &x0,                   // center of mass of body 0
        const Vector3r &v0,                   // velocity of body 0
        Real invMass1,                        // inverse mass is zero if body is static
        const Vector3r &x1,                   // center of mass of body 1
        const Vector3r &v1,                   // velocity of body 1
        const Matrix3r &inertia_inverse_w_1,  // inverse inertia tensor (world space) of body 1
        const Quaternionr &q1,                // rotation of body 1
        const Vector3r &omega1,               // angular velocity of body 1
        const Vector3r &cp0,                  // contact point of body 0
        const Vector3r &cp1,                  // contact point of body 1
        const Vector3r &normal,               // contact normal in body 1
        Real restitution_coeff,               // coefficient of restitution
        Eigen::Matrix<Real, 3, 5, Eigen::DontAlign> &constraint_info) {
    // constraintInfo contains
    // 0:	contact point in body 0 (global)
    // 1:	contact point in body 1 (global)
    // 2:	contact normal in body 1 (global)
    // 3:	contact tangent (global)
    // 0,4:  1.0 / normal^T * K * normal
    // 1,4: maximal impulse in tangent direction
    // 2,4: goal velocity in normal direction after collision

    // compute goal velocity in normal direction after collision
    const Vector3r r1 = cp1 - x1;

    const Vector3r u1 = v1 + omega1.cross(r1);
    const Vector3r u_rel = v0 - u1;
    const Real u_rel_n = normal.dot(u_rel);

    constraint_info.col(0) = cp0;
    constraint_info.col(1) = cp1;
    constraint_info.col(2) = normal;

    // tangent direction
    Vector3r t = u_rel - u_rel_n * normal;
    Real tl2 = t.squaredNorm();
    if (tl2 > 1.0e-6) t *= static_cast<Real>(1.0) / sqrt(tl2);

    constraint_info.col(3) = t;

    // determine K matrix
    Matrix3r K;
    ComputeMatrixK(cp1, invMass1, x1, inertia_inverse_w_1, K);
    if (invMass0 != 0.0) {
        K(0, 0) += invMass0;
        K(1, 1) += invMass0;
        K(2, 2) += invMass0;
    }

    constraint_info(0, 4) = static_cast<Real>(1.0) / (normal.dot(K * normal));

    // maximal impulse in tangent direction
    constraint_info(1, 4) = static_cast<Real>(1.0) / (t.dot(K * t)) * u_rel.dot(t);

    // goal velocity in normal direction after collision
    constraint_info(2, 4) = 0.0;
    if (u_rel_n < 0.0) constraint_info(2, 4) = -restitution_coeff * u_rel_n;

    return true;
}

//--------------------------------------------------------------------------------------------
bool PositionBasedRigidBodyDynamics::VelocitySolveParticleRigidBodyContactConstraint(
        Real inv_mass_0,                                               // inverse mass is zero if body is static
        const Vector3r &x0,                                            // center of mass of body 0
        const Vector3r &v0,                                            // velocity of body 0
        Real inv_mass_1,                                               // inverse mass is zero if body is static
        const Vector3r &x1,                                            // center of mass of body 1
        const Vector3r &v1,                                            // velocity of body 1
        const Matrix3r &inertia_inverse_w_1,                           // inverse inertia tensor (world space) of body 1
        const Vector3r &omega1,                                        // angular velocity of body 1
        Real stiffness,                                                // stiffness parameter of penalty impulse
        Real friction_coeff,                                           // friction coefficient
        Real &sum_impulses,                                            // sum of all impulses
        Eigen::Matrix<Real, 3, 5, Eigen::DontAlign> &constraint_info,  // precomputed contact info
        Vector3r &corr_v0,
        Vector3r &corr_v1,
        Vector3r &corr_omega1) {
    // constraintInfo contains
    // 0:	contact point in body 0 (global)
    // 1:	contact point in body 1 (global)
    // 2:	contact normal in body 1 (global)
    // 3:	contact tangent (global)
    // 0,4:  1.0 / normal^T * K * normal
    // 1,4: maximal impulse in tangent direction
    // 2,4: goal velocity in normal direction after collision

    if ((inv_mass_0 == 0.0) && (inv_mass_1 == 0.0)) return false;

    const Vector3r &connector0 = constraint_info.col(0);
    const Vector3r &connector1 = constraint_info.col(1);
    const Vector3r &normal = constraint_info.col(2);
    const Vector3r &tangent = constraint_info.col(3);

    // 1.0 / normal^T * K * normal
    const Real nKn_inv = constraint_info(0, 4);

    // penetration depth
    const Real d = normal.dot(connector0 - connector1);

    // maximal impulse in tangent direction
    const Real pMax = constraint_info(1, 4);

    // goal velocity in normal direction after collision
    const Real goal_u_rel_n = constraint_info(2, 4);

    const Vector3r r1 = connector1 - x1;
    const Vector3r u1 = v1 + omega1.cross(r1);

    const Vector3r u_rel = v0 - u1;
    const Real u_rel_n = u_rel.dot(normal);
    const Real delta_u_reln = goal_u_rel_n - u_rel_n;

    Real correctionMagnitude = nKn_inv * delta_u_reln;

    if (correctionMagnitude < -sum_impulses) correctionMagnitude = -sum_impulses;

    // add penalty impulse to counteract penetration
    if (d < 0.0) correctionMagnitude -= stiffness * nKn_inv * d;

    Vector3r p(correctionMagnitude * normal);
    sum_impulses += correctionMagnitude;

    const Real pn = p.dot(normal);
    if (friction_coeff * pn > pMax)
        p -= pMax * tangent;
    else if (friction_coeff * pn < -pMax)
        p += pMax * tangent;
    else
        p -= friction_coeff * pn * tangent;

    if (inv_mass_0 != 0.0) {
        corr_v0 = inv_mass_0 * p;
    }

    if (inv_mass_1 != 0.0) {
        corr_v1 = -inv_mass_1 * p;
        corr_omega1 = inertia_inverse_w_1 * (r1.cross(-p));
    }

    return true;
}
}  // namespace vox::compute
