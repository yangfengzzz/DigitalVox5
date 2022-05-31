//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.force/xpbd.h"

#include "vox.force/math_functions.h"

namespace vox::force {
//////////////////////////////////////////////////////////////////////////
// XPBD
//////////////////////////////////////////////////////////////////////////

bool XPBD::SolveDistanceConstraint(const Vector3r& p0,
                                   Real inv_mass_0,
                                   const Vector3r& p1,
                                   Real inv_mass_1,
                                   Real rest_length,
                                   Real stiffness,
                                   Real dt,
                                   Real& lambda,
                                   Vector3r& corr0,
                                   Vector3r& corr1) {
    Real K = inv_mass_0 + inv_mass_1;
    Vector3r n = p0 - p1;
    Real d = n.norm();
    Real C = d - rest_length;
    if (d > static_cast<Real>(1e-6))
        n /= d;
    else {
        corr0.setZero();
        corr1.setZero();
        return true;
    }

    Real alpha = 0.0;
    if (stiffness != 0.0) {
        alpha = static_cast<Real>(1.0) / (stiffness * dt * dt);
        K += alpha;
    }

    Real Kinv = 0.0;
    if (fabs(K) > static_cast<Real>(1e-6))
        Kinv = static_cast<Real>(1.0) / K;
    else {
        corr0.setZero();
        corr1.setZero();
        return true;
    }

    const Real kDeltaLambda = -Kinv * (C + alpha * lambda);
    lambda += kDeltaLambda;
    const Vector3r pt = n * kDeltaLambda;

    corr0 = inv_mass_0 * pt;
    corr1 = -inv_mass_1 * pt;
    return true;
}

// ----------------------------------------------------------------------------------------------
bool XPBD::SolveVolumeConstraint(const Vector3r& p0,
                                 Real inv_mass_0,
                                 const Vector3r& p1,
                                 Real inv_mass_1,
                                 const Vector3r& p2,
                                 Real inv_mass_2,
                                 const Vector3r& p3,
                                 Real inv_mass_3,
                                 Real rest_volume,
                                 Real stiffness,
                                 Real dt,
                                 Real& lambda,
                                 Vector3r& corr0,
                                 Vector3r& corr1,
                                 Vector3r& corr2,
                                 Vector3r& corr3) {
    Real volume = static_cast<Real>(1.0 / 6.0) * (p1 - p0).cross(p2 - p0).dot(p3 - p0);

    corr0.setZero();
    corr1.setZero();
    corr2.setZero();
    corr3.setZero();

    Vector3r grad0 = (p1 - p2).cross(p3 - p2);
    Vector3r grad1 = (p2 - p0).cross(p3 - p0);
    Vector3r grad2 = (p0 - p1).cross(p3 - p1);
    Vector3r grad3 = (p1 - p0).cross(p2 - p0);

    Real K = inv_mass_0 * grad0.squaredNorm() + inv_mass_1 * grad1.squaredNorm() + inv_mass_2 * grad2.squaredNorm() +
             inv_mass_3 * grad3.squaredNorm();

    Real alpha = 0.0;
    if (stiffness != 0.0) {
        alpha = static_cast<Real>(1.0) / (stiffness * dt * dt);
        K += alpha;
    }

    if (fabs(K) < std::numeric_limits<Real>::epsilon()) return false;

    const Real C = volume - rest_volume;
    const Real kDeltaLambda = -(C + alpha * lambda) / K;
    lambda += kDeltaLambda;

    corr0 = kDeltaLambda * inv_mass_0 * grad0;
    corr1 = kDeltaLambda * inv_mass_1 * grad1;
    corr2 = kDeltaLambda * inv_mass_2 * grad2;
    corr3 = kDeltaLambda * inv_mass_3 * grad3;

    return true;
}

// ----------------------------------------------------------------------------------------------
bool XPBD::InitIsometricBendingConstraint(
        const Vector3r& p0, const Vector3r& p1, const Vector3r& p2, const Vector3r& p3, Matrix4r& Q) {
    // Compute matrix Q for quadratic bending
    const Vector3r* x[4] = {&p2, &p3, &p0, &p1};

    const Vector3r e0 = *x[1] - *x[0];
    const Vector3r e1 = *x[2] - *x[0];
    const Vector3r e2 = *x[3] - *x[0];
    const Vector3r e3 = *x[2] - *x[1];
    const Vector3r e4 = *x[3] - *x[1];

    const Real c01 = MathFunctions::CotTheta(e0, e1);
    const Real c02 = MathFunctions::CotTheta(e0, e2);
    const Real c03 = MathFunctions::CotTheta(-e0, e3);
    const Real c04 = MathFunctions::CotTheta(-e0, e4);

    const Real A0 = static_cast<Real>(0.5) * (e0.cross(e1)).norm();
    const Real A1 = static_cast<Real>(0.5) * (e0.cross(e2)).norm();

    const Real coef = -3.f / (2.f * (A0 + A1));
    const Real K[4] = {c03 + c04, c01 + c02, -c01 - c03, -c02 - c04};
    const Real K2[4] = {coef * K[0], coef * K[1], coef * K[2], coef * K[3]};

    for (unsigned char j = 0; j < 4; j++) {
        for (unsigned char k = 0; k < j; k++) {
            Q(j, k) = Q(k, j) = K[j] * K2[k];
        }
        Q(j, j) = K[j] * K2[j];
    }

    return true;
}

// ----------------------------------------------------------------------------------------------
bool XPBD::SolveIsometricBendingConstraint(const Vector3r& p0,
                                           Real inv_mass_0,
                                           const Vector3r& p1,
                                           Real inv_mass_1,
                                           const Vector3r& p2,
                                           Real inv_mass_2,
                                           const Vector3r& p3,
                                           Real inv_mass_3,
                                           const Matrix4r& Q,
                                           Real stiffness,
                                           Real dt,
                                           Real& lambda,
                                           Vector3r& corr0,
                                           Vector3r& corr1,
                                           Vector3r& corr2,
                                           Vector3r& corr3) {
    const Vector3r* x[4] = {&p2, &p3, &p0, &p1};
    Real inv_mass[4] = {inv_mass_2, inv_mass_3, inv_mass_0, inv_mass_1};

    Real energy = 0.0;
    for (unsigned char k = 0; k < 4; k++)
        for (unsigned char j = 0; j < 4; j++) energy += Q(j, k) * (x[k]->dot(*x[j]));
    energy *= 0.5;

    Vector3r grad_c[4];
    grad_c[0].setZero();
    grad_c[1].setZero();
    grad_c[2].setZero();
    grad_c[3].setZero();
    for (unsigned char k = 0; k < 4; k++)
        for (unsigned char j = 0; j < 4; j++) grad_c[j] += Q(j, k) * *x[k];

    Real sum_norm_grad_c = 0.0;
    for (unsigned int j = 0; j < 4; j++) {
        // compute sum of squared gradient norms
        if (inv_mass[j] != 0.0) sum_norm_grad_c += inv_mass[j] * grad_c[j].squaredNorm();
    }

    Real alpha = 0.0;
    if (stiffness != 0.0) {
        alpha = static_cast<Real>(1.0) / (stiffness * dt * dt);
        sum_norm_grad_c += alpha;
    }

    // exit early if required
    if (fabs(sum_norm_grad_c) > std::numeric_limits<Real>::epsilon()) {
        // compute impulse-based scaling factor
        const Real kDeltaLambda = -(energy + alpha * lambda) / sum_norm_grad_c;
        lambda += kDeltaLambda;

        corr0 = (kDeltaLambda * inv_mass[2]) * grad_c[2];
        corr1 = (kDeltaLambda * inv_mass[3]) * grad_c[3];
        corr2 = (kDeltaLambda * inv_mass[0]) * grad_c[0];
        corr3 = (kDeltaLambda * inv_mass[1]) * grad_c[1];

        return true;
    }
    return false;
}

}  // namespace vox::force