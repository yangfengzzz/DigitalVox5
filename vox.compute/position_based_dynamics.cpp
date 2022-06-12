//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.compute/position_based_dynamics.h"

#include "vox.compute/math_functions.h"

namespace vox::compute {
//////////////////////////////////////////////////////////////////////////
// PositionBasedDynamics
//////////////////////////////////////////////////////////////////////////

bool PositionBasedDynamics::SolveDistanceConstraint(const Vector3r &p0,
                                                    Real inv_mass_0,
                                                    const Vector3r &p1,
                                                    Real inv_mass_1,
                                                    Real rest_length,
                                                    Real stiffness,
                                                    Vector3r &corr0,
                                                    Vector3r &corr1) {
    Real w_sum = inv_mass_0 + inv_mass_1;
    if (w_sum == 0.0) return false;

    Vector3r n = p1 - p0;
    Real d = n.norm();
    n.normalize();

    Vector3r corr;
    corr = stiffness * n * (d - rest_length) / w_sum;

    corr0 = inv_mass_0 * corr;
    corr1 = -inv_mass_1 * corr;
    return true;
}

bool PositionBasedDynamics::SolveDihedralConstraint(const Vector3r &p0,
                                                    Real inv_mass_0,
                                                    const Vector3r &p1,
                                                    Real inv_mass_1,
                                                    const Vector3r &p2,
                                                    Real inv_mass_2,
                                                    const Vector3r &p3,
                                                    Real inv_mass_3,
                                                    Real rest_angle,
                                                    Real stiffness,
                                                    Vector3r &corr0,
                                                    Vector3r &corr1,
                                                    Vector3r &corr2,
                                                    Vector3r &corr3) {
    // derivatives from Bridson, Simulation of Clothing with Folds and Wrinkles
    // his modes correspond to the derivatives of the bending angle arccos(n1 dot n2) with correct scaling

    if (inv_mass_0 == 0.0 && inv_mass_1 == 0.0) return false;

    Vector3r e = p3 - p2;
    Real elen = e.norm();
    if (elen < std::numeric_limits<Real>::epsilon()) return false;

    Real inv_elen = static_cast<Real>(1.0) / elen;

    Vector3r n1 = (p2 - p0).cross(p3 - p0);
    n1 /= n1.squaredNorm();
    Vector3r n2 = (p3 - p1).cross(p2 - p1);
    n2 /= n2.squaredNorm();

    Vector3r d0 = elen * n1;
    Vector3r d1 = elen * n2;
    Vector3r d2 = (p0 - p3).dot(e) * inv_elen * n1 + (p1 - p3).dot(e) * inv_elen * n2;
    Vector3r d3 = (p2 - p0).dot(e) * inv_elen * n1 + (p2 - p1).dot(e) * inv_elen * n2;

    n1.normalize();
    n2.normalize();
    Real dot = n1.dot(n2);

    if (dot < -1.0) dot = -1.0;
    if (dot > 1.0) dot = 1.0;
    Real phi = acos(dot);

    // Real phi = (-0.6981317 * dot * dot - 0.8726646) * dot + 1.570796;	// fast approximation

    Real lambda = inv_mass_0 * d0.squaredNorm() + inv_mass_1 * d1.squaredNorm() + inv_mass_2 * d2.squaredNorm() +
                  inv_mass_3 * d3.squaredNorm();

    if (lambda == 0.0) return false;

    // stability
    // 1.5 is the largest magic number I found to be stable in all cases :-)
    // if (stiffness > 0.5 && fabs(phi - b.restAngle) > 1.5)
    //	stiffness = 0.5;

    lambda = (phi - rest_angle) / lambda * stiffness;

    if (n1.cross(n2).dot(e) > 0.0) lambda = -lambda;

    corr0 = -inv_mass_0 * lambda * d0;
    corr1 = -inv_mass_1 * lambda * d1;
    corr2 = -inv_mass_2 * lambda * d2;
    corr3 = -inv_mass_3 * lambda * d3;

    return true;
}

bool PositionBasedDynamics::SolveVolumeConstraint(const Vector3r &p0,
                                                  Real inv_mass_0,
                                                  const Vector3r &p1,
                                                  Real inv_mass_1,
                                                  const Vector3r &p2,
                                                  Real inv_mass_2,
                                                  const Vector3r &p3,
                                                  Real inv_mass_3,
                                                  Real rest_volume,
                                                  Real stiffness,
                                                  Vector3r &corr0,
                                                  Vector3r &corr1,
                                                  Vector3r &corr2,
                                                  Vector3r &corr3) {
    Real volume = static_cast<Real>(1.0 / 6.0) * (p1 - p0).cross(p2 - p0).dot(p3 - p0);

    corr0.setZero();
    corr1.setZero();
    corr2.setZero();
    corr3.setZero();

    if (stiffness == 0.0) return false;

    Vector3r grad0 = (p1 - p2).cross(p3 - p2);
    Vector3r grad1 = (p2 - p0).cross(p3 - p0);
    Vector3r grad2 = (p0 - p1).cross(p3 - p1);
    Vector3r grad3 = (p1 - p0).cross(p2 - p0);

    Real lambda = inv_mass_0 * grad0.squaredNorm() + inv_mass_1 * grad1.squaredNorm() +
                  inv_mass_2 * grad2.squaredNorm() + inv_mass_3 * grad3.squaredNorm();

    if (fabs(lambda) < std::numeric_limits<Real>::epsilon()) return false;

    lambda = stiffness * (volume - rest_volume) / lambda;

    corr0 = -lambda * inv_mass_0 * grad0;
    corr1 = -lambda * inv_mass_1 * grad1;
    corr2 = -lambda * inv_mass_2 * grad2;
    corr3 = -lambda * inv_mass_3 * grad3;

    return true;
}

// ----------------------------------------------------------------------------------------------
bool PositionBasedDynamics::InitIsometricBendingConstraint(
        const Vector3r &p0, const Vector3r &p1, const Vector3r &p2, const Vector3r &p3, Matrix4r &Q) {
    // Compute matrix Q for quadratic bending
    const Vector3r *x[4] = {&p2, &p3, &p0, &p1};

    const Vector3r kE0 = *x[1] - *x[0];
    const Vector3r kE1 = *x[2] - *x[0];
    const Vector3r kE2 = *x[3] - *x[0];
    const Vector3r kE3 = *x[2] - *x[1];
    const Vector3r kE4 = *x[3] - *x[1];

    const Real kC01 = MathFunctions::CotTheta(kE0, kE1);
    const Real kC02 = MathFunctions::CotTheta(kE0, kE2);
    const Real kC03 = MathFunctions::CotTheta(-kE0, kE3);
    const Real kC04 = MathFunctions::CotTheta(-kE0, kE4);

    const Real kA0 = static_cast<Real>(0.5) * (kE0.cross(kE1)).norm();
    const Real kA1 = static_cast<Real>(0.5) * (kE0.cross(kE2)).norm();

    const Real kCoef = -3.f / (2.f * (kA0 + kA1));
    const Real K[4] = {kC03 + kC04, kC01 + kC02, -kC01 - kC03, -kC02 - kC04};
    const Real K2[4] = {kCoef * K[0], kCoef * K[1], kCoef * K[2], kCoef * K[3]};

    for (unsigned char j = 0; j < 4; j++) {
        for (unsigned char k = 0; k < j; k++) {
            Q(j, k) = Q(k, j) = K[j] * K2[k];
        }
        Q(j, j) = K[j] * K2[j];
    }

    return true;
}

// ----------------------------------------------------------------------------------------------
bool PositionBasedDynamics::SolveIsometricBendingConstraint(const Vector3r &p0,
                                                            Real inv_mass_0,
                                                            const Vector3r &p1,
                                                            Real inv_mass_1,
                                                            const Vector3r &p2,
                                                            Real inv_mass_2,
                                                            const Vector3r &p3,
                                                            Real inv_mass_3,
                                                            const Matrix4r &Q,
                                                            Real stiffness,
                                                            Vector3r &corr0,
                                                            Vector3r &corr1,
                                                            Vector3r &corr2,
                                                            Vector3r &corr3) {
    const Vector3r *x[4] = {&p2, &p3, &p0, &p1};
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

    // exit early if required
    if (fabs(sum_norm_grad_c) > std::numeric_limits<Real>::epsilon()) {
        // compute impulse-based scaling factor
        const Real kS = energy / sum_norm_grad_c;

        corr0 = -stiffness * (kS * inv_mass[2]) * grad_c[2];
        corr1 = -stiffness * (kS * inv_mass[3]) * grad_c[3];
        corr2 = -stiffness * (kS * inv_mass[0]) * grad_c[0];
        corr3 = -stiffness * (kS * inv_mass[1]) * grad_c[1];

        return true;
    }
    return false;
}

// ----------------------------------------------------------------------------------------------
bool PositionBasedDynamics::SolveEdgePointDistanceConstraint(const Vector3r &p,
                                                             Real inv_mass,
                                                             const Vector3r &p0,
                                                             Real inv_mass_0,
                                                             const Vector3r &p1,
                                                             Real inv_mass_1,
                                                             Real rest_dist,
                                                             Real compression_stiffness,
                                                             Real stretch_stiffness,
                                                             Vector3r &corr,
                                                             Vector3r &corr0,
                                                             Vector3r &corr1) {
    Vector3r d = p1 - p0;
    Real t;
    if ((p0 - p1).squaredNorm() < std::numeric_limits<Real>::epsilon() * std::numeric_limits<Real>::epsilon())
        t = 0.5;
    else {
        Real d2 = d.dot(d);
        t = d.dot(p - p1) / d2;
        if (t < 0.0)
            t = 0.0;
        else if (t > 1.0)
            t = 1.0;
    }
    Vector3r q = p0 + d * t;  // closest point on edge
    Vector3r n = p - q;
    Real dist = n.norm();
    n.normalize();
    Real C = dist - rest_dist;
    Real b0 = static_cast<Real>(1.0) - t;
    Real b1 = t;
    Vector3r grad = n;
    Vector3r grad0 = -n * b0;
    Vector3r grad1 = -n * b1;

    Real s = inv_mass + inv_mass_0 * b0 * b0 + inv_mass_1 * b1 * b1;
    if (s == 0.0) return false;

    s = C / s;
    if (C < 0.0)
        s *= compression_stiffness;
    else
        s *= stretch_stiffness;

    if (s == 0.0) return false;

    corr = -s * inv_mass * grad;
    corr0 = -s * inv_mass_0 * grad0;
    corr1 = -s * inv_mass_1 * grad1;
    return true;
}

// ----------------------------------------------------------------------------------------------
bool PositionBasedDynamics::SolveTrianglePointDistanceConstraint(const Vector3r &p,
                                                                 Real inv_mass,
                                                                 const Vector3r &p0,
                                                                 Real inv_mass_0,
                                                                 const Vector3r &p1,
                                                                 Real inv_mass_1,
                                                                 const Vector3r &p2,
                                                                 Real inv_mass_2,
                                                                 Real rest_dist,
                                                                 Real compression_stiffness,
                                                                 Real stretch_stiffness,
                                                                 Vector3r &corr,
                                                                 Vector3r &corr0,
                                                                 Vector3r &corr1,
                                                                 Vector3r &corr2) {
    // find barycentric coordinates of the closest point on triangle

    Real b0 = static_cast<Real>(1.0 / 3.0);  // for singular case
    Real b1 = b0;
    Real b2 = b0;

    Vector3r d1 = p1 - p0;
    Vector3r d2 = p2 - p0;
    Vector3r pp0 = p - p0;
    Real a = d1.dot(d1);
    Real b = d2.dot(d1);
    Real c = pp0.dot(d1);
    Real d = b;
    Real e = d2.dot(d2);
    Real f = pp0.dot(d2);
    Real det = a * e - b * d;

    if (det != 0.0) {
        Real s = (c * e - b * f) / det;
        Real t = (a * f - c * d) / det;
        b0 = static_cast<Real>(1.0) - s - t;  // inside triangle
        b1 = s;
        b2 = t;
        if (b0 < 0.0) {  // on edge 1-2
            Vector3r d = p2 - p1;
            Real d2 = d.dot(d);
            Real t = (d2 == static_cast<Real>(0.0)) ? static_cast<Real>(0.5) : d.dot(p - p1) / d2;
            if (t < 0.0) t = 0.0;  // on point 1
            if (t > 1.0) t = 1.0;  // on point 2
            b0 = 0.0;
            b1 = (static_cast<Real>(1.0) - t);
            b2 = t;
        } else if (b1 < 0.0) {  // on edge 2-0
            Vector3r d = p0 - p2;
            Real d2 = d.dot(d);
            Real t = (d2 == static_cast<Real>(0.0)) ? static_cast<Real>(0.5) : d.dot(p - p2) / d2;
            if (t < 0.0) t = 0.0;  // on point 2
            if (t > 1.0) t = 1.0;  // on point 0
            b1 = 0.0;
            b2 = (static_cast<Real>(1.0) - t);
            b0 = t;
        } else if (b2 < 0.0) {  // on edge 0-1
            Vector3r d = p1 - p0;
            Real d2 = d.dot(d);
            Real t = (d2 == static_cast<Real>(0.0)) ? static_cast<Real>(0.5) : d.dot(p - p0) / d2;
            if (t < 0.0) t = 0.0;  // on point 0
            if (t > 1.0) t = 1.0;  // on point 1
            b2 = 0.0;
            b0 = (static_cast<Real>(1.0) - t);
            b1 = t;
        }
    }
    Vector3r q = p0 * b0 + p1 * b1 + p2 * b2;
    Vector3r n = p - q;
    Real dist = n.norm();
    n.normalize();
    Real C = dist - rest_dist;
    Vector3r grad = n;
    Vector3r grad0 = -n * b0;
    Vector3r grad1 = -n * b1;
    Vector3r grad2 = -n * b2;

    Real s = inv_mass + inv_mass_0 * b0 * b0 + inv_mass_1 * b1 * b1 + inv_mass_2 * b2 * b2;
    if (s == 0.0) return false;

    s = C / s;
    if (C < 0.0)
        s *= compression_stiffness;
    else
        s *= stretch_stiffness;

    if (s == 0.0) return false;

    corr = -s * inv_mass * grad;
    corr0 = -s * inv_mass_0 * grad0;
    corr1 = -s * inv_mass_1 * grad1;
    corr2 = -s * inv_mass_2 * grad2;
    return true;
}

// ----------------------------------------------------------------------------------------------
bool PositionBasedDynamics::SolveEdgeEdgeDistanceConstraint(const Vector3r &p0,
                                                            Real inv_mass_0,
                                                            const Vector3r &p1,
                                                            Real inv_mass_1,
                                                            const Vector3r &p2,
                                                            Real inv_mass_2,
                                                            const Vector3r &p3,
                                                            Real inv_mass_3,
                                                            Real rest_dist,
                                                            Real compression_stiffness,
                                                            Real stretch_stiffness,
                                                            Vector3r &corr0,
                                                            Vector3r &corr1,
                                                            Vector3r &corr2,
                                                            Vector3r &corr3) {
    Vector3r d0 = p1 - p0;
    Vector3r d1 = p3 - p2;

    Real a = d0.squaredNorm();
    Real b = -d0.dot(d1);
    Real c = d0.dot(d1);
    Real d = -d1.squaredNorm();
    Real e = (p2 - p0).dot(d0);
    Real f = (p2 - p0).dot(d1);
    Real det = a * d - b * c;
    Real s, t;
    if (det != 0.0) {
        det = static_cast<Real>(1.0) / det;
        s = (e * d - b * f) * det;
        t = (a * f - e * c) * det;
    } else {  // d0 and d1 parallel
        Real s0 = p0.dot(d0);
        Real s1 = p1.dot(d0);
        Real t0 = p2.dot(d0);
        Real t1 = p3.dot(d0);
        bool flip0 = false;
        bool flip1 = false;

        if (s0 > s1) {
            Real f = s0;
            s0 = s1;
            s1 = f;
            flip0 = true;
        }
        if (t0 > t1) {
            Real f = t0;
            t0 = t1;
            t1 = f;
            flip1 = true;
        }

        if (s0 >= t1) {
            s = !flip0 ? static_cast<Real>(0.0) : static_cast<Real>(1.0);
            t = !flip1 ? static_cast<Real>(1.0) : static_cast<Real>(0.0);
        } else if (t0 >= s1) {
            s = !flip0 ? static_cast<Real>(1.0) : static_cast<Real>(0.0);
            t = !flip1 ? static_cast<Real>(0.0) : static_cast<Real>(1.0);
        } else {  // overlap
            Real mid = (s0 > t0) ? (s0 + t1) * static_cast<Real>(0.5) : (t0 + s1) * static_cast<Real>(0.5);
            s = (s0 == s1) ? static_cast<Real>(0.5) : (mid - s0) / (s1 - s0);
            t = (t0 == t1) ? static_cast<Real>(0.5) : (mid - t0) / (t1 - t0);
        }
    }
    if (s < 0.0) s = 0.0;
    if (s > 1.0) s = 1.0;
    if (t < 0.0) t = 0.0;
    if (t > 1.0) t = 1.0;

    Real b0 = static_cast<Real>(1.0) - s;
    Real b1 = s;
    Real b2 = static_cast<Real>(1.0) - t;
    Real b3 = t;

    Vector3r q0 = p0 * b0 + p1 * b1;
    Vector3r q1 = p2 * b2 + p3 * b3;
    Vector3r n = q0 - q1;
    Real dist = n.norm();
    n.normalize();
    Real C = dist - rest_dist;
    Vector3r grad0 = n * b0;
    Vector3r grad1 = n * b1;
    Vector3r grad2 = -n * b2;
    Vector3r grad3 = -n * b3;

    s = inv_mass_0 * b0 * b0 + inv_mass_1 * b1 * b1 + inv_mass_2 * b2 * b2 + inv_mass_3 * b3 * b3;
    if (s == 0.0) return false;

    s = C / s;
    if (C < 0.0)
        s *= compression_stiffness;
    else
        s *= stretch_stiffness;

    if (s == 0.0) return false;

    corr0 = -s * inv_mass_0 * grad0;
    corr1 = -s * inv_mass_1 * grad1;
    corr2 = -s * inv_mass_2 * grad2;
    corr3 = -s * inv_mass_3 * grad3;
    return true;
}

// ----------------------------------------------------------------------------------------------
bool PositionBasedDynamics::InitShapeMatchingConstraint(const Vector3r x0[],
                                                        const Real inv_masses[],
                                                        int num_points,
                                                        Vector3r &rest_cm) {
    // center of mass
    rest_cm.setZero();
    Real w_sum = 0.0;
    for (int i = 0; i < num_points; i++) {
        Real wi = static_cast<Real>(1.0) / (inv_masses[i] + std::numeric_limits<Real>::epsilon());
        rest_cm += x0[i] * wi;
        w_sum += wi;
    }
    if (w_sum == 0.0) return false;
    rest_cm /= w_sum;

    return true;
}

// ----------------------------------------------------------------------------------------------
bool PositionBasedDynamics::SolveShapeMatchingConstraint(const Vector3r x0[],
                                                         const Vector3r x[],
                                                         const Real inv_masses[],
                                                         int num_points,
                                                         const Vector3r &rest_cm,
                                                         Real stiffness,
                                                         bool allow_stretch,
                                                         Vector3r corr[],
                                                         Matrix3r *rot) {
    for (int i = 0; i < num_points; i++) corr[i].setZero();

    // center of mass
    Vector3r cm(0.0, 0.0, 0.0);
    Real w_sum = 0.0;
    for (int i = 0; i < num_points; i++) {
        Real wi = static_cast<Real>(1.0) / (inv_masses[i] + std::numeric_limits<Real>::epsilon());
        cm += x[i] * wi;
        w_sum += wi;
    }
    if (w_sum == 0.0) return false;
    cm /= w_sum;

    // A
    Matrix3r mat;
    mat.setZero();
    for (int i = 0; i < num_points; i++) {
        Vector3r q = x0[i] - rest_cm;
        Vector3r p = x[i] - cm;

        Real w = static_cast<Real>(1.0) / (inv_masses[i] + std::numeric_limits<Real>::epsilon());
        p *= w;

        mat(0, 0) += p[0] * q[0];
        mat(0, 1) += p[0] * q[1];
        mat(0, 2) += p[0] * q[2];
        mat(1, 0) += p[1] * q[0];
        mat(1, 1) += p[1] * q[1];
        mat(1, 2) += p[1] * q[2];
        mat(2, 0) += p[2] * q[0];
        mat(2, 1) += p[2] * q[1];
        mat(2, 2) += p[2] * q[2];
    }

    // mat = mat * invRestMat;

    Matrix3r R, U, D;
    R = mat;
    if (allow_stretch)
        R = mat;
    else
        // MathFunctions::polarDecomposition(mat, R, U, D);
        MathFunctions::PolarDecompositionStable(mat, std::numeric_limits<Real>::epsilon(), R);

    for (int i = 0; i < num_points; i++) {
        Vector3r goal = cm + R * (x0[i] - rest_cm);
        corr[i] = (goal - x[i]) * stiffness;
    }

    if (rot) *rot = R;

    return true;
}

// ----------------------------------------------------------------------------------------------
bool PositionBasedDynamics::InitStrainTriangleConstraint(const Vector3r &p0,
                                                         const Vector3r &p1,
                                                         const Vector3r &p2,
                                                         Matrix2r &inv_rest_mat) {
    Real a = p1[0] - p0[0];
    Real b = p2[0] - p0[0];
    Real c = p1[1] - p0[1];
    Real d = p2[1] - p0[1];

    // inverse
    Real det = a * d - b * c;
    if (fabs(det) < std::numeric_limits<Real>::epsilon()) return false;

    Real s = static_cast<Real>(1.0) / det;
    inv_rest_mat(0, 0) = d * s;
    inv_rest_mat(0, 1) = -b * s;
    inv_rest_mat(1, 0) = -c * s;
    inv_rest_mat(1, 1) = a * s;

    return true;
}

// ----------------------------------------------------------------------------------------------
bool PositionBasedDynamics::SolveStrainTriangleConstraint(const Vector3r &p0,
                                                          Real inv_mass_0,
                                                          const Vector3r &p1,
                                                          Real inv_mass_1,
                                                          const Vector3r &p2,
                                                          Real inv_mass_2,
                                                          const Matrix2r &inv_rest_mat,
                                                          Real xx_stiffness,
                                                          Real yy_stiffness,
                                                          Real xy_stiffness,
                                                          bool normalize_stretch,
                                                          bool normalize_shear,
                                                          Vector3r &corr0,
                                                          Vector3r &corr1,
                                                          Vector3r &corr2) {
    Vector3r c[2];
    c[0] = Vector3r(inv_rest_mat(0, 0), inv_rest_mat(1, 0), 0.0);
    c[1] = Vector3r(inv_rest_mat(0, 1), inv_rest_mat(1, 1), 0.0);

    Vector3r r[3];

    corr0.setZero();
    corr1.setZero();
    corr2.setZero();

    for (int i = 0; i < 2; i++) {
        for (int j = 0; j <= i; j++) {
            // 			r[0] = Vector3r(p1[0] - p0[0], p2[0] - p0[0], 0.0);  // Jacobi
            // 			r[1] = Vector3r(p1[1] - p0[1], p2[1] - p0[1], 0.0);
            // 			r[2] = Vector3r(p1[2] - p0[2], p2[2] - p0[2], 0.0);

            r[0] = Vector3r((p1[0] + corr1[0]) - (p0[0] + corr0[0]), (p2[0] + corr2[0]) - (p0[0] + corr0[0]),
                            0.0);  // Gauss - Seidel
            r[1] = Vector3r((p1[1] + corr1[1]) - (p0[1] + corr0[1]), (p2[1] + corr2[1]) - (p0[1] + corr0[1]), 0.0);
            r[2] = Vector3r((p1[2] + corr1[2]) - (p0[2] + corr0[2]), (p2[2] + corr2[2]) - (p0[2] + corr0[2]), 0.0);

            Real sij = 0.0;
            for (auto &k : r) sij += k.dot(c[i]) * k.dot(c[j]);

            Vector3r d[3];
            d[0] = Vector3r(0.0, 0.0, 0.0);

            for (int k = 0; k < 2; k++) {
                d[k + 1] = Vector3r(r[0].dot(c[j]), r[1].dot(c[j]), r[2].dot(c[j])) * inv_rest_mat(k, i);
                d[k + 1] += Vector3r(r[0].dot(c[i]), r[1].dot(c[i]), r[2].dot(c[i])) * inv_rest_mat(k, j);
                d[0] -= d[k + 1];
            }

            if (i != j && normalize_shear) {
                Real fi2 = 0.0;
                Real fj2 = 0.0;
                for (auto &k : r) {
                    fi2 += k.dot(c[i]) * k.dot(c[i]);
                    fj2 += k.dot(c[j]) * k.dot(c[j]);
                }
                Real fi = sqrt(fi2);
                Real fj = sqrt(fj2);

                d[0] = Vector3r(0.0, 0.0, 0.0);
                Real s = sij / (fi2 * fi * fj2 * fj);
                for (int k = 0; k < 2; k++) {
                    d[k + 1] /= fi * fj;
                    d[k + 1] -=
                            fj * fj * Vector3r(r[0].dot(c[i]), r[1].dot(c[i]), r[2].dot(c[i])) * inv_rest_mat(k, i) * s;
                    d[k + 1] -=
                            fi * fi * Vector3r(r[0].dot(c[j]), r[1].dot(c[j]), r[2].dot(c[j])) * inv_rest_mat(k, j) * s;
                    d[0] -= d[k + 1];
                }
                sij = sij / (fi * fj);
            }

            Real lambda =
                    inv_mass_0 * d[0].squaredNorm() + inv_mass_1 * d[1].squaredNorm() + inv_mass_2 * d[2].squaredNorm();

            if (lambda == 0.0) continue;

            if (i == 0 && j == 0) {
                if (normalize_stretch) {
                    Real s = sqrt(sij);
                    lambda = static_cast<Real>(2.0) * s * (s - static_cast<Real>(1.0)) / lambda * xx_stiffness;
                } else {
                    lambda = (sij - static_cast<Real>(1.0)) / lambda * xx_stiffness;
                }
            } else if (i == 1 && j == 1) {
                if (normalize_stretch) {
                    Real s = sqrt(sij);
                    lambda = static_cast<Real>(2.0) * s * (s - static_cast<Real>(1.0)) / lambda * yy_stiffness;
                } else {
                    lambda = (sij - static_cast<Real>(1.0)) / lambda * yy_stiffness;
                }
            } else {
                lambda = sij / lambda * xy_stiffness;
            }

            corr0 -= lambda * inv_mass_0 * d[0];
            corr1 -= lambda * inv_mass_1 * d[1];
            corr2 -= lambda * inv_mass_2 * d[2];
        }
    }
    return true;
}

// ----------------------------------------------------------------------------------------------
bool PositionBasedDynamics::InitStrainTetraConstraint(
        const Vector3r &p0, const Vector3r &p1, const Vector3r &p2, const Vector3r &p3, Matrix3r &inv_rest_mat) {
    Matrix3r m;
    m.col(0) = p1 - p0;
    m.col(1) = p2 - p0;
    m.col(2) = p3 - p0;

    Real det = m.determinant();
    if (fabs(det) > std::numeric_limits<Real>::epsilon()) {
        inv_rest_mat = m.inverse();
        return true;
    }
    return false;
}

// ----------------------------------------------------------------------------------------------
bool PositionBasedDynamics::SolveStrainTetraConstraint(const Vector3r &p0,
                                                       Real inv_mass_0,
                                                       const Vector3r &p1,
                                                       Real inv_mass_1,
                                                       const Vector3r &p2,
                                                       Real inv_mass_2,
                                                       const Vector3r &p3,
                                                       Real inv_mass_3,
                                                       const Matrix3r &inv_rest_mat,
                                                       const Vector3r &stretch_stiffness,
                                                       const Vector3r &shear_stiffness,
                                                       bool normalize_stretch,
                                                       bool normalize_shear,
                                                       Vector3r &corr0,
                                                       Vector3r &corr1,
                                                       Vector3r &corr2,
                                                       Vector3r &corr3) {
    corr0.setZero();
    corr1.setZero();
    corr2.setZero();
    corr3.setZero();

    Vector3r c[3];
    c[0] = inv_rest_mat.col(0);
    c[1] = inv_rest_mat.col(1);
    c[2] = inv_rest_mat.col(2);

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j <= i; j++) {
            Matrix3r P;
            // 			P.col(0) = p1 - p0;		// Jacobi
            // 			P.col(1) = p2 - p0;
            // 			P.col(2) = p3 - p0;

            P.col(0) = (p1 + corr1) - (p0 + corr0);  // Gauss - Seidel
            P.col(1) = (p2 + corr2) - (p0 + corr0);
            P.col(2) = (p3 + corr3) - (p0 + corr0);

            Vector3r fi = P * c[i];
            Vector3r fj = P * c[j];

            Real Sij = fi.dot(fj);

            Real wi, wj, s1, s3;
            if (normalize_shear && i != j) {
                wi = fi.norm();
                wj = fj.norm();
                s1 = static_cast<Real>(1.0) / (wi * wj);
                s3 = s1 * s1 * s1;
            }

            Vector3r d[4];
            d[0] = Vector3r(0.0, 0.0, 0.0);

            for (int k = 0; k < 3; k++) {
                d[k + 1] = fj * inv_rest_mat(k, i) + fi * inv_rest_mat(k, j);

                if (normalize_shear && i != j) {
                    d[k + 1] = s1 * d[k + 1] -
                               Sij * s3 * (wj * wj * fi * inv_rest_mat(k, i) + wi * wi * fj * inv_rest_mat(k, j));
                }

                d[0] -= d[k + 1];
            }

            if (normalize_shear && i != j) Sij *= s1;

            Real lambda = inv_mass_0 * d[0].squaredNorm() + inv_mass_1 * d[1].squaredNorm() +
                          inv_mass_2 * d[2].squaredNorm() + inv_mass_3 * d[3].squaredNorm();

            if (fabs(lambda) < std::numeric_limits<Real>::epsilon())  // foo: threshold should be scale dependent
                continue;

            if (i == j) {  // diagonal, stretch
                if (normalize_stretch) {
                    Real s = sqrt(Sij);
                    lambda = static_cast<Real>(2.0) * s * (s - static_cast<Real>(1.0)) / lambda * stretch_stiffness[i];
                } else {
                    lambda = (Sij - static_cast<Real>(1.0)) / lambda * stretch_stiffness[i];
                }
            } else {  // off diagonal, shear
                lambda = Sij / lambda * shear_stiffness[i + j - 1];
            }

            corr0 -= lambda * inv_mass_0 * d[0];
            corr1 -= lambda * inv_mass_1 * d[1];
            corr2 -= lambda * inv_mass_2 * d[2];
            corr3 -= lambda * inv_mass_3 * d[3];
        }
    }
    return true;
}

// ----------------------------------------------------------------------------------------------
bool PositionBasedDynamics::InitFemTriangleConstraint(
        const Vector3r &p0, const Vector3r &p1, const Vector3r &p2, Real &area, Matrix2r &inv_rest_mat) {
    Vector3r normal0 = (p1 - p0).cross(p2 - p0);
    area = normal0.norm() * static_cast<Real>(0.5);

    Vector3r axis0_1 = p1 - p0;
    axis0_1.normalize();
    Vector3r axis0_2 = normal0.cross(axis0_1);
    axis0_2.normalize();

    Vector2r p[3];
    p[0] = Vector2r(p0.dot(axis0_2), p0.dot(axis0_1));
    p[1] = Vector2r(p1.dot(axis0_2), p1.dot(axis0_1));
    p[2] = Vector2r(p2.dot(axis0_2), p2.dot(axis0_1));

    Matrix2r P;
    P(0, 0) = p[0][0] - p[2][0];
    P(1, 0) = p[0][1] - p[2][1];
    P(0, 1) = p[1][0] - p[2][0];
    P(1, 1) = p[1][1] - p[2][1];

    const Real kDet = P.determinant();
    if (fabs(kDet) > std::numeric_limits<Real>::epsilon()) {
        inv_rest_mat = P.inverse();
        return true;
    }
    return false;
}

// ----------------------------------------------------------------------------------------------
bool PositionBasedDynamics::SolveFemTriangleConstraint(const Vector3r &p0,
                                                       Real inv_mass_0,
                                                       const Vector3r &p1,
                                                       Real inv_mass_1,
                                                       const Vector3r &p2,
                                                       Real inv_mass_2,
                                                       const Real &area,
                                                       const Matrix2r &inv_rest_mat,
                                                       Real youngs_modulus_x,
                                                       Real youngs_modulus_y,
                                                       Real youngs_modulus_shear,
                                                       Real poisson_ratio_xy,
                                                       Real poisson_ratio_yx,
                                                       Vector3r &corr0,
                                                       Vector3r &corr1,
                                                       Vector3r &corr2) {
    // Orthotropic elasticity tensor
    Matrix3r C;
    C.setZero();
    C(0, 0) = youngs_modulus_x / (static_cast<Real>(1.0) - poisson_ratio_xy * poisson_ratio_yx);
    C(0, 1) = youngs_modulus_x * poisson_ratio_yx / (static_cast<Real>(1.0) - poisson_ratio_xy * poisson_ratio_yx);
    C(1, 1) = youngs_modulus_y / (static_cast<Real>(1.0) - poisson_ratio_xy * poisson_ratio_yx);
    C(1, 0) = youngs_modulus_y * poisson_ratio_xy / (static_cast<Real>(1.0) - poisson_ratio_xy * poisson_ratio_yx);
    C(2, 2) = youngs_modulus_shear;

    // Determine \partial x/\partial m_i
    Eigen::Matrix<Real, 3, 2> F;
    const Vector3r kP13 = p0 - p2;
    const Vector3r kP23 = p1 - p2;
    F(0, 0) = kP13[0] * inv_rest_mat(0, 0) + kP23[0] * inv_rest_mat(1, 0);
    F(0, 1) = kP13[0] * inv_rest_mat(0, 1) + kP23[0] * inv_rest_mat(1, 1);
    F(1, 0) = kP13[1] * inv_rest_mat(0, 0) + kP23[1] * inv_rest_mat(1, 0);
    F(1, 1) = kP13[1] * inv_rest_mat(0, 1) + kP23[1] * inv_rest_mat(1, 1);
    F(2, 0) = kP13[2] * inv_rest_mat(0, 0) + kP23[2] * inv_rest_mat(1, 0);
    F(2, 1) = kP13[2] * inv_rest_mat(0, 1) + kP23[2] * inv_rest_mat(1, 1);

    // epsilon = 0.5(F^T * F - I)
    Matrix2r epsilon;
    epsilon(0, 0) = static_cast<Real>(0.5) *
                    (F(0, 0) * F(0, 0) + F(1, 0) * F(1, 0) + F(2, 0) * F(2, 0) - static_cast<Real>(1.0));  // xx
    epsilon(1, 1) = static_cast<Real>(0.5) *
                    (F(0, 1) * F(0, 1) + F(1, 1) * F(1, 1) + F(2, 1) * F(2, 1) - static_cast<Real>(1.0));  // yy
    epsilon(0, 1) = static_cast<Real>(0.5) * (F(0, 0) * F(0, 1) + F(1, 0) * F(1, 1) + F(2, 0) * F(2, 1));  // xy
    epsilon(1, 0) = epsilon(0, 1);

    // P(F) = det(F) * C*E * F^-T => E = green strain
    Matrix2r stress;
    stress(0, 0) = C(0, 0) * epsilon(0, 0) + C(0, 1) * epsilon(1, 1) + C(0, 2) * epsilon(0, 1);
    stress(1, 1) = C(1, 0) * epsilon(0, 0) + C(1, 1) * epsilon(1, 1) + C(1, 2) * epsilon(0, 1);
    stress(0, 1) = C(2, 0) * epsilon(0, 0) + C(2, 1) * epsilon(1, 1) + C(2, 2) * epsilon(0, 1);
    stress(1, 0) = stress(0, 1);

    const Eigen::Matrix<Real, 3, 2> kPiolaKirchhoffStres = F * stress;

    Real psi = 0.0;
    for (unsigned char j = 0; j < 2; j++)
        for (unsigned char k = 0; k < 2; k++) psi += epsilon(j, k) * stress(j, k);
    psi = static_cast<Real>(0.5) * psi;
    Real energy = area * psi;

    // compute gradient
    Eigen::Matrix<Real, 3, 2> H = area * kPiolaKirchhoffStres * inv_rest_mat.transpose();
    Vector3r grad_c[3];
    for (unsigned char j = 0; j < 3; ++j) {
        grad_c[0][j] = H(j, 0);
        grad_c[1][j] = H(j, 1);
    }
    grad_c[2] = -grad_c[0] - grad_c[1];

    Real sum_norm_grad_c = inv_mass_0 * grad_c[0].squaredNorm();
    sum_norm_grad_c += inv_mass_1 * grad_c[1].squaredNorm();
    sum_norm_grad_c += inv_mass_2 * grad_c[2].squaredNorm();

    // exit early if required
    if (fabs(sum_norm_grad_c) > std::numeric_limits<Real>::epsilon()) {
        // compute scaling factor
        const Real kS = energy / sum_norm_grad_c;

        // update positions
        corr0 = -(kS * inv_mass_0) * grad_c[0];
        corr1 = -(kS * inv_mass_1) * grad_c[1];
        corr2 = -(kS * inv_mass_2) * grad_c[2];

        return true;
    }

    return false;
}

// ----------------------------------------------------------------------------------------------
bool PositionBasedDynamics::InitFemTetraConstraint(  // compute only when rest shape changes
        const Vector3r &p0,
        const Vector3r &p1,
        const Vector3r &p2,
        const Vector3r &p3,
        Real &volume,
        Matrix3r &inv_rest_mat) {
    volume = fabs(static_cast<Real>(1.0 / 6.0) * (p3 - p0).dot((p2 - p0).cross(p1 - p0)));

    Matrix3r m;
    m.col(0) = p0 - p3;
    m.col(1) = p1 - p3;
    m.col(2) = p2 - p3;

    Real det = m.determinant();
    if (fabs(det) > std::numeric_limits<Real>::epsilon()) {
        inv_rest_mat = m.inverse();
        return true;
    }
    return false;
}

// ----------------------------------------------------------------------------------------------
void PositionBasedDynamics::ComputeGreenStrainAndPiolaStress(const Vector3r &x1,
                                                             const Vector3r &x2,
                                                             const Vector3r &x3,
                                                             const Vector3r &x4,
                                                             const Matrix3r &inv_rest_mat,
                                                             Real rest_volume,
                                                             Real mu,
                                                             Real lambda,
                                                             Matrix3r &epsilon,
                                                             Matrix3r &sigma,
                                                             Real &energy) {
    // Determine \partial x/\partial m_i
    Matrix3r F;
    const Vector3r kP14 = x1 - x4;
    const Vector3r kP24 = x2 - x4;
    const Vector3r kP34 = x3 - x4;
    F(0, 0) = kP14[0] * inv_rest_mat(0, 0) + kP24[0] * inv_rest_mat(1, 0) + kP34[0] * inv_rest_mat(2, 0);
    F(0, 1) = kP14[0] * inv_rest_mat(0, 1) + kP24[0] * inv_rest_mat(1, 1) + kP34[0] * inv_rest_mat(2, 1);
    F(0, 2) = kP14[0] * inv_rest_mat(0, 2) + kP24[0] * inv_rest_mat(1, 2) + kP34[0] * inv_rest_mat(2, 2);

    F(1, 0) = kP14[1] * inv_rest_mat(0, 0) + kP24[1] * inv_rest_mat(1, 0) + kP34[1] * inv_rest_mat(2, 0);
    F(1, 1) = kP14[1] * inv_rest_mat(0, 1) + kP24[1] * inv_rest_mat(1, 1) + kP34[1] * inv_rest_mat(2, 1);
    F(1, 2) = kP14[1] * inv_rest_mat(0, 2) + kP24[1] * inv_rest_mat(1, 2) + kP34[1] * inv_rest_mat(2, 2);

    F(2, 0) = kP14[2] * inv_rest_mat(0, 0) + kP24[2] * inv_rest_mat(1, 0) + kP34[2] * inv_rest_mat(2, 0);
    F(2, 1) = kP14[2] * inv_rest_mat(0, 1) + kP24[2] * inv_rest_mat(1, 1) + kP34[2] * inv_rest_mat(2, 1);
    F(2, 2) = kP14[2] * inv_rest_mat(0, 2) + kP24[2] * inv_rest_mat(1, 2) + kP34[2] * inv_rest_mat(2, 2);

    // epsilon = 1/2 F^T F - I

    epsilon(0, 0) = static_cast<Real>(0.5) *
                    (F(0, 0) * F(0, 0) + F(1, 0) * F(1, 0) + F(2, 0) * F(2, 0) - static_cast<Real>(1.0));  // xx
    epsilon(1, 1) = static_cast<Real>(0.5) *
                    (F(0, 1) * F(0, 1) + F(1, 1) * F(1, 1) + F(2, 1) * F(2, 1) - static_cast<Real>(1.0));  // yy
    epsilon(2, 2) = static_cast<Real>(0.5) *
                    (F(0, 2) * F(0, 2) + F(1, 2) * F(1, 2) + F(2, 2) * F(2, 2) - static_cast<Real>(1.0));  // zz
    epsilon(0, 1) = static_cast<Real>(0.5) * (F(0, 0) * F(0, 1) + F(1, 0) * F(1, 1) + F(2, 0) * F(2, 1));  // xy
    epsilon(0, 2) = static_cast<Real>(0.5) * (F(0, 0) * F(0, 2) + F(1, 0) * F(1, 2) + F(2, 0) * F(2, 2));  // xz
    epsilon(1, 2) = static_cast<Real>(0.5) * (F(0, 1) * F(0, 2) + F(1, 1) * F(1, 2) + F(2, 1) * F(2, 2));  // yz
    epsilon(1, 0) = epsilon(0, 1);
    epsilon(2, 0) = epsilon(0, 2);
    epsilon(2, 1) = epsilon(1, 2);

    // P(F) = F(2 mu E + lambda tr(E)I) => E = green strain
    const Real kTrace = epsilon(0, 0) + epsilon(1, 1) + epsilon(2, 2);
    const Real kLtrace = lambda * kTrace;
    sigma = epsilon * 2.0 * mu;
    sigma(0, 0) += kLtrace;
    sigma(1, 1) += kLtrace;
    sigma(2, 2) += kLtrace;
    sigma = F * sigma;

    Real psi = 0.0;
    for (unsigned char j = 0; j < 3; j++)
        for (unsigned char k = 0; k < 3; k++) psi += epsilon(j, k) * epsilon(j, k);
    psi = mu * psi + static_cast<Real>(0.5) * lambda * kTrace * kTrace;
    energy = rest_volume * psi;
}

// ----------------------------------------------------------------------------------------------
void PositionBasedDynamics::ComputeGradCGreen(Real rest_volume,
                                              const Matrix3r &inv_rest_mat,
                                              const Matrix3r &sigma,
                                              Vector3r *J) {
    Matrix3r H;
    Matrix3r T;
    T = inv_rest_mat.transpose();
    H = sigma * T * rest_volume;

    J[0][0] = H(0, 0);
    J[1][0] = H(0, 1);
    J[2][0] = H(0, 2);

    J[0][1] = H(1, 0);
    J[1][1] = H(1, 1);
    J[2][1] = H(1, 2);

    J[0][2] = H(2, 0);
    J[1][2] = H(2, 1);
    J[2][2] = H(2, 2);

    J[3] = -J[0] - J[1] - J[2];
}

// ----------------------------------------------------------------------------------------------
void PositionBasedDynamics::ComputeGreenStrainAndPiolaStressInversion(const Vector3r &x1,
                                                                      const Vector3r &x2,
                                                                      const Vector3r &x3,
                                                                      const Vector3r &x4,
                                                                      const Matrix3r &inv_rest_mat,
                                                                      Real rest_volume,
                                                                      Real mu,
                                                                      Real lambda,
                                                                      Matrix3r &epsilon,
                                                                      Matrix3r &sigma,
                                                                      Real &energy) {
    // Determine \partial x/\partial m_i
    Matrix3r F;
    const Vector3r kP14 = x1 - x4;
    const Vector3r kP24 = x2 - x4;
    const Vector3r kP34 = x3 - x4;
    F(0, 0) = kP14[0] * inv_rest_mat(0, 0) + kP24[0] * inv_rest_mat(1, 0) + kP34[0] * inv_rest_mat(2, 0);
    F(0, 1) = kP14[0] * inv_rest_mat(0, 1) + kP24[0] * inv_rest_mat(1, 1) + kP34[0] * inv_rest_mat(2, 1);
    F(0, 2) = kP14[0] * inv_rest_mat(0, 2) + kP24[0] * inv_rest_mat(1, 2) + kP34[0] * inv_rest_mat(2, 2);

    F(1, 0) = kP14[1] * inv_rest_mat(0, 0) + kP24[1] * inv_rest_mat(1, 0) + kP34[1] * inv_rest_mat(2, 0);
    F(1, 1) = kP14[1] * inv_rest_mat(0, 1) + kP24[1] * inv_rest_mat(1, 1) + kP34[1] * inv_rest_mat(2, 1);
    F(1, 2) = kP14[1] * inv_rest_mat(0, 2) + kP24[1] * inv_rest_mat(1, 2) + kP34[1] * inv_rest_mat(2, 2);

    F(2, 0) = kP14[2] * inv_rest_mat(0, 0) + kP24[2] * inv_rest_mat(1, 0) + kP34[2] * inv_rest_mat(2, 0);
    F(2, 1) = kP14[2] * inv_rest_mat(0, 1) + kP24[2] * inv_rest_mat(1, 1) + kP34[2] * inv_rest_mat(2, 1);
    F(2, 2) = kP14[2] * inv_rest_mat(0, 2) + kP24[2] * inv_rest_mat(1, 2) + kP34[2] * inv_rest_mat(2, 2);

    Matrix3r U, VT;
    Vector3r hat_f;
    MathFunctions::SvdWithInversionHandling(F, hat_f, U, VT);

    // Clamp small singular values
    const Real kMinXVal = static_cast<Real>(0.577);

    for (unsigned char j = 0; j < 3; j++) {
        if (hat_f[j] < kMinXVal) hat_f[j] = kMinXVal;
    }

    // epsilon for hatF
    Vector3r epsilon_hat_f(static_cast<Real>(0.5) * (hat_f[0] * hat_f[0] - static_cast<Real>(1.0)),
                           static_cast<Real>(0.5) * (hat_f[1] * hat_f[1] - static_cast<Real>(1.0)),
                           static_cast<Real>(0.5) * (hat_f[2] * hat_f[2] - static_cast<Real>(1.0)));

    const Real kTrace = epsilon_hat_f[0] + epsilon_hat_f[1] + epsilon_hat_f[2];
    const Real kLtrace = lambda * kTrace;
    Vector3r sigma_vec = epsilon_hat_f * 2.0 * mu;
    sigma_vec[0] += kLtrace;
    sigma_vec[1] += kLtrace;
    sigma_vec[2] += kLtrace;
    sigma_vec[0] = hat_f[0] * sigma_vec[0];
    sigma_vec[1] = hat_f[1] * sigma_vec[1];
    sigma_vec[2] = hat_f[2] * sigma_vec[2];

    Matrix3r sigma_diag, eps_diag;

    sigma_diag.row(0) = Vector3r(sigma_vec[0], 0.0, 0.0);
    sigma_diag.row(1) = Vector3r(0.0, sigma_vec[1], 0.0);
    sigma_diag.row(2) = Vector3r(0.0, 0.0, sigma_vec[2]);

    eps_diag.row(0) = Vector3r(epsilon_hat_f[0], 0.0, 0.0);
    eps_diag.row(1) = Vector3r(0.0, epsilon_hat_f[1], 0.0);
    eps_diag.row(2) = Vector3r(0.0, 0.0, epsilon_hat_f[2]);

    epsilon = U * eps_diag * VT;
    sigma = U * sigma_diag * VT;

    Real psi = 0.0;
    for (unsigned char j = 0; j < 3; j++)
        for (unsigned char k = 0; k < 3; k++) psi += epsilon(j, k) * epsilon(j, k);
    psi = mu * psi + static_cast<Real>(0.5) * lambda * kTrace * kTrace;
    energy = rest_volume * psi;
}

// ----------------------------------------------------------------------------------------------
bool PositionBasedDynamics::SolveFemTetraConstraint(const Vector3r &p0,
                                                    Real inv_mass_0,
                                                    const Vector3r &p1,
                                                    Real inv_mass_1,
                                                    const Vector3r &p2,
                                                    Real inv_mass_2,
                                                    const Vector3r &p3,
                                                    Real inv_mass_3,
                                                    Real rest_volume,
                                                    const Matrix3r &inv_rest_mat,
                                                    Real youngs_modulus,
                                                    Real poisson_ratio,
                                                    bool handle_inversion,
                                                    Vector3r &corr0,
                                                    Vector3r &corr1,
                                                    Vector3r &corr2,
                                                    Vector3r &corr3) {
    corr0.setZero();
    corr1.setZero();
    corr2.setZero();
    corr3.setZero();

    if (youngs_modulus <= 0.0) return true;

    if (poisson_ratio < 0.0 || poisson_ratio > 0.49) return false;

    Real C = 0.0;
    Vector3r grad_c[4];
    Matrix3r epsilon, sigma;
    Real volume = (p1 - p0).cross(p2 - p0).dot(p3 - p0) / static_cast<Real>(6.0);

    Real mu = youngs_modulus / static_cast<Real>(2.0) / (static_cast<Real>(1.0) + poisson_ratio);
    Real lambda = youngs_modulus * poisson_ratio / (static_cast<Real>(1.0) + poisson_ratio) /
                  (static_cast<Real>(1.0) - static_cast<Real>(2.0) * poisson_ratio);

    if (!handle_inversion || volume > 0.0) {
        ComputeGreenStrainAndPiolaStress(p0, p1, p2, p3, inv_rest_mat, rest_volume, mu, lambda, epsilon, sigma, C);
        ComputeGradCGreen(rest_volume, inv_rest_mat, sigma, grad_c);
    } else {
        ComputeGreenStrainAndPiolaStressInversion(p0, p1, p2, p3, inv_rest_mat, rest_volume, mu, lambda, epsilon, sigma,
                                                  C);
        ComputeGradCGreen(rest_volume, inv_rest_mat, sigma, grad_c);
    }

    Real sum_norm_grad_c = inv_mass_0 * grad_c[0].squaredNorm() + inv_mass_1 * grad_c[1].squaredNorm() +
                           inv_mass_2 * grad_c[2].squaredNorm() + inv_mass_3 * grad_c[3].squaredNorm();

    if (sum_norm_grad_c < std::numeric_limits<Real>::epsilon()) return false;

    // compute scaling factor
    const Real kS = C / sum_norm_grad_c;

    corr0 = -kS * inv_mass_0 * grad_c[0];
    corr1 = -kS * inv_mass_1 * grad_c[1];
    corr2 = -kS * inv_mass_2 * grad_c[2];
    corr3 = -kS * inv_mass_3 * grad_c[3];

    return true;
}

// ----------------------------------------------------------------------------------------------
bool PositionBasedDynamics::InitParticleTetContactConstraint(
        Real inv_mass_0,         // inverse mass is zero if particle is static
        const Vector3r &x0,      // particle which collides with tet
        const Vector3r &v0,      // velocity of particle
        const Real inv_mass[],   // inverse masses of tet particles
        const Vector3r x[],      // positions of tet particles
        const Vector3r v[],      // velocities of tet particles
        const Vector3r &bary,    // barycentric coordinates of contact point in tet
        const Vector3r &normal,  // contact normal in body 1
        Eigen::Matrix<Real, 3, 3, Eigen::DontAlign> &constraint_info) {
    // constraintInfo contains
    // 0:	contact normal in body 1 (global)
    // 1:	contact tangent (global)
    // 0,1:  1.0 / normal^T * K * normal
    // 1,2: maximal impulse in tangent direction

    const Real kBary0 = static_cast<Real>(1.0) - bary[0] - bary[1] - bary[2];

    // compute world space contact point in body 2
    const Vector3r kV1 = kBary0 * v[0] + bary[0] * v[1] + bary[1] * v[2] + bary[2] * v[3];

    // compute goal velocity in normal direction after collision
    const Vector3r kURel = v0 - kV1;
    const Real kURelN = normal.dot(kURel);

    constraint_info.col(0) = normal;

    // tangent direction
    Vector3r t = kURel - kURelN * normal;
    Real tl2 = t.squaredNorm();
    if (tl2 > 1.0e-6) t *= static_cast<Real>(1.0) / sqrt(tl2);

    constraint_info.col(1) = t;

    // determine 1/(J M^-1 J^T)
    const Real kJMinvJt = inv_mass_0 + kBary0 * kBary0 * inv_mass[0] + bary[0] * bary[0] * inv_mass[1] +
                          bary[1] * bary[1] * inv_mass[2] + bary[2] * bary[2] * inv_mass[3];
    constraint_info(0, 2) = static_cast<Real>(1.0) / kJMinvJt;

    // maximal impulse in tangent direction
    constraint_info(1, 2) = static_cast<Real>(1.0) / kJMinvJt * kURel.dot(t);

    return true;
}

//--------------------------------------------------------------------------------------------
bool PositionBasedDynamics::SolveParticleTetContactConstraint(
        Real inv_mass_0,        // inverse mass is zero if particle is static
        const Vector3r &x0,     // particle which collides with tet
        const Real inv_mass[],  // inverse masses of tet particles
        const Vector3r x[],     // positions of tet particles
        const Vector3r &bary,   // barycentric coordinates of contact point in tet
        Eigen::Matrix<Real, 3, 3, Eigen::DontAlign> &constraint_info,  // precomputed contact info
        Real &lambda,
        Vector3r &corr0,
        Vector3r corr[]) {
    // constraintInfo contains
    // 0:	contact normal in body 1 (global)
    // 1:	contact tangent (global)
    // 0,2:  1.0 / normal^T * K * normal
    // 1,2: maximal impulse in tangent direction

    if ((inv_mass_0 == 0.0) && (inv_mass[0] == 0.0) && (inv_mass[1] == 0.0) && (inv_mass[2] == 0.0)) return false;

    const Real kBary0 = static_cast<Real>(1.0) - bary[0] - bary[1] - bary[2];

    // compute world space contact point in body 2
    const Vector3r kCp1 = kBary0 * x[0] + bary[0] * x[1] + bary[1] * x[2] + bary[2] * x[3];

    const Vector3r &normal = constraint_info.col(0);

    // 1.0 / normal^T * K * normal
    const Real kNKnInv = constraint_info(0, 2);

    // penetration depth
    const Real kC = normal.dot(x0 - kCp1);

    lambda = -kNKnInv * kC;

    Vector3r p(lambda * normal);
    if (inv_mass_0 != 0.0) {
        corr0 = inv_mass_0 * p;
    }

    if (inv_mass[0] != 0.0) corr[0] = -inv_mass[0] * kBary0 * p;
    if (inv_mass[1] != 0.0) corr[1] = -inv_mass[1] * bary[0] * p;
    if (inv_mass[2] != 0.0) corr[2] = -inv_mass[2] * bary[1] * p;
    if (inv_mass[3] != 0.0) corr[3] = -inv_mass[3] * bary[2] * p;

    return true;
}

//--------------------------------------------------------------------------------------------
bool PositionBasedDynamics::VelocitySolveParticleTetContactConstraint(
        Real inv_mass_0,        // inverse mass is zero if particle is static
        const Vector3r &x0,     // particle which collides with tet
        const Vector3r &v0,     // velocity of particle
        const Real inv_mass[],  // inverse masses of tet particles
        const Vector3r x[],     // positions of tet particles
        const Vector3r v[],     // velocities of tet particles
        const Vector3r &bary,   // barycentric coordinates of contact point in tet
        Real lambda,
        Real friction_coeff,                                           // friction coefficient
        Eigen::Matrix<Real, 3, 3, Eigen::DontAlign> &constraint_info,  // precomputed contact info
        Vector3r &corr_v0,
        Vector3r corr_v[]) {
    // constraintInfo contains
    // 0:	contact normal in body 1 (global)
    // 1:	contact tangent (global)
    // 0,2:  1.0 / normal^T * K * normal
    // 1,2: maximal impulse in tangent direction

    if ((inv_mass_0 == 0.0) && (inv_mass[0] == 0.0) && (inv_mass[1] == 0.0) && (inv_mass[2] == 0.0)) return false;

    const Real kBary0 = static_cast<Real>(1.0) - bary[0] - bary[1] - bary[2];

    // Friction
    // maximal impulse in tangent direction
    const Real kPMax = constraint_info(1, 2);
    const Vector3r &tangent = constraint_info.col(1);
    Vector3r pv;
    if (friction_coeff * lambda > kPMax)
        pv = -kPMax * tangent;
    else if (friction_coeff * lambda < -kPMax)
        pv = kPMax * tangent;
    else
        pv = -friction_coeff * lambda * tangent;

    if (inv_mass_0 != 0.0) {
        corr_v0 = inv_mass_0 * pv;
    }

    if (inv_mass[0] != 0.0) corr_v[0] = -inv_mass[0] * kBary0 * pv;
    if (inv_mass[1] != 0.0) corr_v[1] = -inv_mass[1] * bary[0] * pv;
    if (inv_mass[2] != 0.0) corr_v[2] = -inv_mass[2] * bary[1] * pv;
    if (inv_mass[3] != 0.0) corr_v[3] = -inv_mass[3] * bary[2] * pv;

    return true;
}

}  // namespace vox::compute
