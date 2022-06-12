//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.compute/position_based_elastic_rods.h"

#include <cmath>

#include "vox.base/logging.h"
#include "vox.compute/math_functions.h"

namespace vox::compute {

const int permutation[3][3] = {0, 2, 1, 1, 0, 2, 2, 1, 0};

//MARK: ----------------------------------------------------------------------------------------------
bool PositionBasedCosseratRods::SolveStretchShearConstraint(const Vector3r &p0,
                                                            Real inv_mass_0,
                                                            const Vector3r &p1,
                                                            Real inv_mass_1,
                                                            const Quaternionr &q0,
                                                            Real inv_massq_0,
                                                            const Vector3r &stretching_and_shearing_ks,
                                                            Real rest_length,
                                                            Vector3r &corr0,
                                                            Vector3r &corr1,
                                                            Quaternionr &corrq0) {
    Vector3r d3;  // third director d3 = q0 * e_3 * q0_conjugate
    d3[0] = static_cast<Real>(2.0) * (q0.x() * q0.z() + q0.w() * q0.y());
    d3[1] = static_cast<Real>(2.0) * (q0.y() * q0.z() - q0.w() * q0.x());
    d3[2] = q0.w() * q0.w() - q0.x() * q0.x() - q0.y() * q0.y() + q0.z() * q0.z();

    Vector3r gamma = (p1 - p0) / rest_length - d3;
    gamma /= (inv_mass_1 + inv_mass_0) / rest_length + inv_massq_0 * static_cast<Real>(4.0) * rest_length +
             std::numeric_limits<Real>::epsilon();

    if (std::abs(stretching_and_shearing_ks[0] - stretching_and_shearing_ks[1]) <
                std::numeric_limits<Real>::epsilon() &&
        std::abs(stretching_and_shearing_ks[0] - stretching_and_shearing_ks[2]) <
                std::numeric_limits<Real>::epsilon())  // all Ks are approx. equal
        for (int i = 0; i < 3; i++) gamma[i] *= stretching_and_shearing_ks[i];
    else  // diffenent stretching and shearing Ks. Transform diag(Ks[0], Ks[1], Ks[2]) into world space using Ks_w =
          // R(q0) * diag(Ks[0], Ks[1], Ks[2]) * R^T(q0) and multiply it with gamma
    {
        Matrix3r R = q0.toRotationMatrix();
        gamma = (R.transpose() * gamma).eval();
        for (int i = 0; i < 3; i++) gamma[i] *= stretching_and_shearing_ks[i];
        gamma = (R * gamma).eval();
    }

    corr0 = inv_mass_0 * gamma;
    corr1 = -inv_mass_1 * gamma;

    Quaternionr q_e_3_bar(q0.z(), -q0.y(), q0.x(),
                          -q0.w());  // compute q*e_3.conjugate (cheaper than quaternion product)
    corrq0 = Quaternionr(0.0, gamma.x(), gamma.y(), gamma.z()) * q_e_3_bar;
    corrq0.coeffs() *= static_cast<Real>(2.0) * inv_massq_0 * rest_length;

    return true;
}

// ----------------------------------------------------------------------------------------------
bool PositionBasedCosseratRods::SolveBendTwistConstraint(const Quaternionr &q0,
                                                         Real inv_massq_0,
                                                         const Quaternionr &q1,
                                                         Real inv_massq_1,
                                                         const Vector3r &bending_and_twisting_ks,
                                                         const Quaternionr &rest_darboux_vector,
                                                         Quaternionr &corrq0,
                                                         Quaternionr &corrq1) {
    Quaternionr omega = q0.conjugate() * q1;  // darboux vector

    Quaternionr omega_plus;
    omega_plus.coeffs() = omega.coeffs() + rest_darboux_vector.coeffs();  // delta Omega with -Omega_0
    omega.coeffs() = omega.coeffs() - rest_darboux_vector.coeffs();       // delta Omega with + omega_0
    if (omega.squaredNorm() > omega_plus.squaredNorm()) omega = omega_plus;

    for (int i = 0; i < 3; i++)
        omega.coeffs()[i] *= bending_and_twisting_ks[i] / (inv_massq_0 + inv_massq_1 + static_cast<Real>(1.0e-6));
    omega.w() = 0.0;  // discrete Darboux vector does not have vanishing scalar part

    corrq0 = q1 * omega;
    corrq1 = q0 * omega;
    corrq0.coeffs() *= inv_massq_0;
    corrq1.coeffs() *= -inv_massq_1;
    return true;
}

//MARK: ----------------------------------------------------------------------------------------------
bool PositionBasedElasticRods::SolvePerpendiculaBisectorConstraint(const Vector3r &p0,
                                                                   Real inv_mass_0,
                                                                   const Vector3r &p1,
                                                                   Real inv_mass_1,
                                                                   const Vector3r &p2,
                                                                   Real inv_mass_2,
                                                                   Real stiffness,
                                                                   Vector3r &corr0,
                                                                   Vector3r &corr1,
                                                                   Vector3r &corr2) {
    const Vector3r pm = 0.5 * (p0 + p1);
    const Vector3r p0p2 = p0 - p2;
    const Vector3r p2p1 = p2 - p1;
    const Vector3r p1p0 = p1 - p0;
    const Vector3r p2pm = p2 - pm;

    Real w_sum = inv_mass_0 * p0p2.squaredNorm() + inv_mass_1 * p2p1.squaredNorm() + inv_mass_2 * p1p0.squaredNorm();
    if (w_sum < std::numeric_limits<Real>::epsilon()) return false;

    const Real lambda = stiffness * p2pm.dot(p1p0) / w_sum;

    corr0 = -inv_mass_0 * lambda * p0p2;
    corr1 = -inv_mass_1 * lambda * p2p1;
    corr2 = -inv_mass_2 * lambda * p1p0;

    return true;
}

// ----------------------------------------------------------------------------------------------
bool PositionBasedElasticRods::SolveGhostPointEdgeDistanceConstraint(const Vector3r &p0,
                                                                     Real inv_mass_0,
                                                                     const Vector3r &p1,
                                                                     Real inv_mass_1,
                                                                     const Vector3r &p2,
                                                                     Real inv_mass_2,
                                                                     Real stiffness,
                                                                     Real ghost_edge_rest_length,
                                                                     Vector3r &corr0,
                                                                     Vector3r &corr1,
                                                                     Vector3r &corr2) {
    // Ghost-Edge constraint
    Vector3r pm = 0.5 * (p0 + p1);
    Vector3r p2pm = p2 - pm;
    Real wSum = static_cast<Real>(0.25) * inv_mass_0 + static_cast<Real>(0.25) * inv_mass_1 +
                static_cast<Real>(1.0) * inv_mass_2;

    if (wSum < std::numeric_limits<Real>::epsilon()) return false;

    Real p2pm_mag = p2pm.norm();
    p2pm *= static_cast<Real>(1.0) / p2pm_mag;

    const Real lambda = stiffness * (p2pm_mag - ghost_edge_rest_length) / wSum;

    corr0 = 0.5 * inv_mass_0 * lambda * p2pm;
    corr1 = 0.5 * inv_mass_1 * lambda * p2pm;
    corr2 = -1.0 * inv_mass_2 * lambda * p2pm;

    return true;
}

// ----------------------------------------------------------------------------------------------
bool PositionBasedElasticRods::SolveDarbouxVectorConstraint(const Vector3r &p0,
                                                            Real inv_mass_0,
                                                            const Vector3r &p1,
                                                            Real inv_mass_1,
                                                            const Vector3r &p2,
                                                            Real inv_mass_2,
                                                            const Vector3r &p3,
                                                            Real inv_mass_3,
                                                            const Vector3r &p4,
                                                            Real inv_mass_4,
                                                            const Vector3r &bending_and_twisting_ks,
                                                            Real mid_edge_length,
                                                            const Vector3r &rest_darboux_vector,
                                                            Vector3r &oa,
                                                            Vector3r &ob,
                                                            Vector3r &oc,
                                                            Vector3r &od,
                                                            Vector3r &oe) {
    //  Single rod element:
    //      3   4		//ghost points
    //		|	|
    //  --0---1---2--	// rod points

    Vector3r darboux_vector;
    Matrix3r d0, d1;

    PositionBasedElasticRods::ComputeMaterialFrame(p0, p1, p3, d0);
    PositionBasedElasticRods::ComputeMaterialFrame(p1, p2, p4, d1);

    PositionBasedElasticRods::ComputeDarbouxVector(d0, d1, mid_edge_length, darboux_vector);

    Matrix3r dajpi[3][3];
    ComputeMaterialFrameDerivative(p0, p1, p3, d0, dajpi[0][0], dajpi[0][1], dajpi[0][2], dajpi[1][0], dajpi[1][1],
                                   dajpi[1][2], dajpi[2][0], dajpi[2][1], dajpi[2][2]);

    Matrix3r dbjpi[3][3];
    ComputeMaterialFrameDerivative(p1, p2, p4, d1, dbjpi[0][0], dbjpi[0][1], dbjpi[0][2], dbjpi[1][0], dbjpi[1][1],
                                   dbjpi[1][2], dbjpi[2][0], dbjpi[2][1], dbjpi[2][2]);

    Matrix3r constraint_jacobian[5];
    ComputeDarbouxGradient(darboux_vector, mid_edge_length, d0, d1, dajpi, dbjpi,
                           // bendingAndTwistingKs,
                           constraint_jacobian[0], constraint_jacobian[1], constraint_jacobian[2],
                           constraint_jacobian[3], constraint_jacobian[4]);

    const Vector3r constraint_value(bending_and_twisting_ks[0] * (darboux_vector[0] - rest_darboux_vector[0]),
                                    bending_and_twisting_ks[1] * (darboux_vector[1] - rest_darboux_vector[1]),
                                    bending_and_twisting_ks[2] * (darboux_vector[2] - rest_darboux_vector[2]));

    Matrix3r factor_matrix;
    factor_matrix.setZero();

    Matrix3r tmp_mat;
    Real inv_masses[]{inv_mass_0, inv_mass_1, inv_mass_2, inv_mass_3, inv_mass_4};
    for (int i = 0; i < 5; ++i) {
        tmp_mat = constraint_jacobian[i].transpose() * constraint_jacobian[i];
        tmp_mat.col(0) *= inv_masses[i];
        tmp_mat.col(1) *= inv_masses[i];
        tmp_mat.col(2) *= inv_masses[i];

        factor_matrix += tmp_mat;
    }

    Vector3r dp[5];
    tmp_mat = factor_matrix.inverse();

    for (int i = 0; i < 5; ++i) {
        constraint_jacobian[i].col(0) *= inv_masses[i];
        constraint_jacobian[i].col(1) *= inv_masses[i];
        constraint_jacobian[i].col(2) *= inv_masses[i];
        dp[i] = -(constraint_jacobian[i]) * (tmp_mat * constraint_value);
    }

    oa = dp[0];
    ob = dp[1];
    oc = dp[2];
    od = dp[3];
    oe = dp[4];

    return true;
}

// ----------------------------------------------------------------------------------------------
bool PositionBasedElasticRods::ComputeMaterialFrame(const Vector3r &p0,
                                                    const Vector3r &p1,
                                                    const Vector3r &p2,
                                                    Matrix3r &frame) {
    frame.col(2) = (p1 - p0);
    frame.col(2).normalize();

    frame.col(1) = (frame.col(2).cross(p2 - p0));
    frame.col(1).normalize();

    frame.col(0) = frame.col(1).cross(frame.col(2));
    return true;
}

// ----------------------------------------------------------------------------------------------
bool PositionBasedElasticRods::ComputeDarbouxVector(const Matrix3r &dA,
                                                    const Matrix3r &dB,
                                                    Real mid_edge_length,
                                                    Vector3r &darboux_vector) {
    Real factor =
            static_cast<Real>(1.0) + dA.col(0).dot(dB.col(0)) + dA.col(1).dot(dB.col(1)) + dA.col(2).dot(dB.col(2));

    factor = static_cast<Real>(2.0) / (mid_edge_length * factor);

    for (auto c : permutation) {
        const int i = c[0];
        const int j = c[1];
        const int k = c[2];
        darboux_vector[i] = dA.col(j).dot(dB.col(k)) - dA.col(k).dot(dB.col(j));
    }
    darboux_vector *= factor;
    return true;
}

// ----------------------------------------------------------------------------------------------
bool PositionBasedElasticRods::ComputeMaterialFrameDerivative(const Vector3r &p0,
                                                              const Vector3r &p1,
                                                              const Vector3r &p2,
                                                              const Matrix3r &d,
                                                              Matrix3r &d1p0,
                                                              Matrix3r &d1p1,
                                                              Matrix3r &d1p2,
                                                              Matrix3r &d2p0,
                                                              Matrix3r &d2p1,
                                                              Matrix3r &d2p2,
                                                              Matrix3r &d3p0,
                                                              Matrix3r &d3p1,
                                                              Matrix3r &d3p2) {
    //////////////////////////////////////////////////////////////////////////
    // d3pi
    //////////////////////////////////////////////////////////////////////////
    const Vector3r p01 = p1 - p0;
    Real length_p01 = p01.norm();

    d3p0.col(0) = d.col(2)[0] * d.col(2);
    d3p0.col(1) = d.col(2)[1] * d.col(2);
    d3p0.col(2) = d.col(2)[2] * d.col(2);

    d3p0.col(0)[0] -= 1.0;
    d3p0.col(1)[1] -= 1.0;
    d3p0.col(2)[2] -= 1.0;

    d3p0.col(0) *= (static_cast<Real>(1.0) / length_p01);
    d3p0.col(1) *= (static_cast<Real>(1.0) / length_p01);
    d3p0.col(2) *= (static_cast<Real>(1.0) / length_p01);

    d3p1.col(0) = -d3p0.col(0);
    d3p1.col(1) = -d3p0.col(1);
    d3p1.col(2) = -d3p0.col(2);

    d3p2.col(0).setZero();
    d3p2.col(1).setZero();
    d3p2.col(2).setZero();

    //////////////////////////////////////////////////////////////////////////
    // d2pi
    //////////////////////////////////////////////////////////////////////////
    const Vector3r p02 = p2 - p0;
    const Vector3r p01_cross_p02 = p01.cross(p02);

    const Real kLengthCross = p01_cross_p02.norm();

    Matrix3r mat;
    mat.col(0) = d.col(1)[0] * d.col(1);
    mat.col(1) = d.col(1)[1] * d.col(1);
    mat.col(2) = d.col(1)[2] * d.col(1);

    mat.col(0)[0] -= 1.0;
    mat.col(1)[1] -= 1.0;
    mat.col(2)[2] -= 1.0;

    mat.col(0) *= (-static_cast<Real>(1.0) / kLengthCross);
    mat.col(1) *= (-static_cast<Real>(1.0) / kLengthCross);
    mat.col(2) *= (-static_cast<Real>(1.0) / kLengthCross);

    Matrix3r product_matrix;
    MathFunctions::CrossProductMatrix(p2 - p1, product_matrix);
    d2p0 = mat * product_matrix;

    MathFunctions::CrossProductMatrix(p0 - p2, product_matrix);
    d2p1 = mat * product_matrix;

    MathFunctions::CrossProductMatrix(p1 - p0, product_matrix);
    d2p2 = mat * product_matrix;

    //////////////////////////////////////////////////////////////////////////
    // d1pi
    //////////////////////////////////////////////////////////////////////////
    Matrix3r product_mat_d3;
    Matrix3r product_mat_d2;
    MathFunctions::CrossProductMatrix(d.col(2), product_mat_d3);
    MathFunctions::CrossProductMatrix(d.col(1), product_mat_d2);

    d1p0 = product_mat_d2 * d3p0 - product_mat_d3 * d2p0;
    d1p1 = product_mat_d2 * d3p1 - product_mat_d3 * d2p1;
    d1p2 = -product_mat_d3 * d2p2;
    return true;
}

// ----------------------------------------------------------------------------------------------
bool PositionBasedElasticRods::ComputeDarbouxGradient(const Vector3r &darboux_vector,
                                                      Real length,
                                                      const Matrix3r &dA,
                                                      const Matrix3r &dB,
                                                      const Matrix3r dajpi[3][3],
                                                      const Matrix3r dbjpi[3][3],
                                                      // const Vector3r& bendAndTwistKs,
                                                      Matrix3r &omega_pa,
                                                      Matrix3r &omega_pb,
                                                      Matrix3r &omega_pc,
                                                      Matrix3r &omega_pd,
                                                      Matrix3r &omega_pe) {
    Real X = static_cast<Real>(1.0) + dA.col(0).dot(dB.col(0)) + dA.col(1).dot(dB.col(1)) + dA.col(2).dot(dB.col(2));
    X = static_cast<Real>(2.0) / (length * X);

    for (auto c : permutation) {
        const int i = c[0];
        const int j = c[1];
        const int k = c[2];
        // pa
        {
            Vector3r term1(0, 0, 0);
            Vector3r term2(0, 0, 0);
            Vector3r tmp(0, 0, 0);

            // first term
            term1 = dajpi[j][0].transpose() * dB.col(k);
            tmp = dajpi[k][0].transpose() * dB.col(j);
            term1 = term1 - tmp;
            // second term
            for (int n = 0; n < 3; ++n) {
                tmp = dajpi[n][0].transpose() * dB.col(n);
                term2 = term2 + tmp;
            }
            omega_pa.col(i) = X * (term1 - (0.5 * darboux_vector[i] * length) * term2);
            // omega_pa.col(i) *= bendAndTwistKs[i];
        }
        // pb
        {
            Vector3r term1(0, 0, 0);
            Vector3r term2(0, 0, 0);
            Vector3r tmp(0, 0, 0);
            // first term
            term1 = dajpi[j][1].transpose() * dB.col(k);
            tmp = dajpi[k][1].transpose() * dB.col(j);
            term1 = term1 - tmp;
            // third term
            tmp = dbjpi[j][0].transpose() * dA.col(k);
            term1 = term1 - tmp;

            tmp = dbjpi[k][0].transpose() * dA.col(j);
            term1 = term1 + tmp;

            // second term
            for (int n = 0; n < 3; ++n) {
                tmp = dajpi[n][1].transpose() * dB.col(n);
                term2 = term2 + tmp;

                tmp = dbjpi[n][0].transpose() * dA.col(n);
                term2 = term2 + tmp;
            }
            omega_pb.col(i) = X * (term1 - (0.5 * darboux_vector[i] * length) * term2);
            // omega_pb.col(i) *= bendAndTwistKs[i];
        }
        // pc
        {
            Vector3r term1(0, 0, 0);
            Vector3r term2(0, 0, 0);
            Vector3r tmp(0, 0, 0);

            // first term
            term1 = dbjpi[j][1].transpose() * dA.col(k);
            tmp = dbjpi[k][1].transpose() * dA.col(j);
            term1 = term1 - tmp;

            // second term
            for (int n = 0; n < 3; ++n) {
                tmp = dbjpi[n][1].transpose() * dA.col(n);
                term2 = term2 + tmp;
            }
            omega_pc.col(i) = -X * (term1 + (0.5 * darboux_vector[i] * length) * term2);
            // omega_pc.col(i) *= bendAndTwistKs[i];
        }
        // pd
        {
            Vector3r term1(0, 0, 0);
            Vector3r term2(0, 0, 0);
            Vector3r tmp(0, 0, 0);
            // first term
            term1 = dajpi[j][2].transpose() * dB.col(k);
            tmp = dajpi[k][2].transpose() * dB.col(j);
            term1 = term1 - tmp;
            // second term
            for (int n = 0; n < 3; ++n) {
                tmp = dajpi[n][2].transpose() * dB.col(n);
                term2 = term2 + tmp;
            }
            omega_pd.col(i) = X * (term1 - (0.5 * darboux_vector[i] * length) * term2);
            // omega_pd.col(i) *= bendAndTwistKs[i];
        }
        // pe
        {
            Vector3r term1(0, 0, 0);
            Vector3r term2(0, 0, 0);
            Vector3r tmp(0, 0, 0);
            // first term
            term1 = dbjpi[j][2].transpose() * dA.col(k);
            tmp = dbjpi[k][2].transpose() * dA.col(j);
            term1 -= tmp;

            // second term
            for (int n = 0; n < 3; ++n) {
                tmp = dbjpi[n][2].transpose() * dA.col(n);
                term2 += tmp;
            }

            omega_pe.col(i) = -X * (term1 + (0.5 * darboux_vector[i] * length) * term2);
            // omega_pe.col(i) *= bendAndTwistKs[i];
        }
    }
    return true;
}

//MARK: ----------------------------------------------------------------------------------------------
void DirectPositionBasedSolverForStiffRods::InitLists(int number_of_intervals,
                                                      std::list<Node *> *&forward,
                                                      std::list<Node *> *&backward,
                                                      Node *&root) {
    delete[] forward;
    delete[] backward;
    delete[] root;
    forward = new std::list<Node *>[number_of_intervals];
    backward = new std::list<Node *>[number_of_intervals];
    root = new Node[number_of_intervals];
}

bool DirectPositionBasedSolverForStiffRods::IsSegmentInInterval(RodSegment *segment,
                                                                int interval_index,
                                                                Interval *intervals,
                                                                std::vector<RodConstraint *> &rod_constraints,
                                                                std::vector<RodSegment *> &rod_segments) {
    for (int i = intervals[interval_index].start; i <= intervals[interval_index].end; i++) {
        if ((segment == rod_segments[rod_constraints[i]->SegmentIndex(0)]) ||
            (segment == rod_segments[rod_constraints[i]->SegmentIndex(1)]))
            return true;
    }
    return false;
}

bool DirectPositionBasedSolverForStiffRods::IsConstraintInInterval(RodConstraint *constraint,
                                                                   int interval_index,
                                                                   Interval *intervals,
                                                                   std::vector<RodConstraint *> &rod_constraints) {
    for (int i = intervals[interval_index].start; i <= intervals[interval_index].end; i++) {
        if (constraint == rod_constraints[i]) return true;
    }
    return false;
}

void DirectPositionBasedSolverForStiffRods::InitSegmentNode(Node *n,
                                                            int interval_index,
                                                            std::vector<RodConstraint *> &rod_constraints,
                                                            std::vector<RodSegment *> &rod_segments,
                                                            std::vector<RodConstraint *> &marked_constraints,
                                                            Interval *intervals) {
    auto *segment = (RodSegment *)n->object;

    std::vector<RodConstraint *> constraints;
    std::vector<int> constraint_indices;
    for (int j = 0; j < static_cast<int>(rod_constraints.size()); ++j) {
        RodConstraint *constraint(rod_constraints[j]);
        if (rod_segments[constraint->SegmentIndex(0)] == segment ||
            rod_segments[constraint->SegmentIndex(1)] == segment) {
            constraints.push_back(constraint);
            constraint_indices.push_back(j);
        }
    }

    for (unsigned int i = 0; i < constraints.size(); i++) {
        if (!IsConstraintInInterval(constraints[i], interval_index, intervals, rod_constraints)) continue;

        // Test whether the edge has been visited before
        bool marked = false;
        for (auto &marked_constraint : marked_constraints) {
            if (constraints[i] == marked_constraint) {
                marked = true;
                break;
            }
        }
        if (!marked) {
            Node *constraint_node = new Node();
            constraint_node->index = constraint_indices[i];
            constraint_node->object = constraints[i];
            constraint_node->isconstraint = true;
            constraint_node->parent = n;
            constraint_node->D.setZero();
            constraint_node->Dinv.setZero();
            constraint_node->J.setZero();
            constraint_node->soln.setZero();

            n->children.push_back(constraint_node);

            Node *segment_node = new Node();
            segment_node->isconstraint = false;
            segment_node->parent = constraint_node;

            //	get other segment connected to constraint for new node
            if (rod_segments[constraints[i]->SegmentIndex(0)] == segment) {
                segment_node->object = rod_segments[constraints[i]->SegmentIndex(1)];
                segment_node->index = constraints[i]->SegmentIndex(1);
            } else {
                segment_node->object = rod_segments[constraints[i]->SegmentIndex(0)];
                segment_node->index = constraints[i]->SegmentIndex(0);
            }

            segment_node->D.setZero();
            segment_node->Dinv.setZero();
            segment_node->J.setZero();
            segment_node->soln.setZero();

            constraint_node->children.push_back(segment_node);

            // mark constraint
            marked_constraints.push_back(constraints[i]);

            InitSegmentNode(segment_node, interval_index, rod_constraints, rod_segments, marked_constraints, intervals);
        }
    }
}

void DirectPositionBasedSolverForStiffRods::OrderMatrix(Node *n,
                                                        int interval_index,
                                                        std::list<Node *> *forward,
                                                        std::list<Node *> *backward) {
    for (auto &i : n->children) OrderMatrix(i, interval_index, forward, backward);
    forward[interval_index].push_back(n);
    backward[interval_index].push_front(n);
}

void DirectPositionBasedSolverForStiffRods::InitNodes(int interval_index,
                                                      std::vector<RodSegment *> &rod_segments,
                                                      Node *&root,
                                                      Interval *intervals,
                                                      std::vector<RodConstraint *> &rod_constraints,
                                                      std::list<Node *> *forward,
                                                      std::list<Node *> *backward,
                                                      std::vector<RodConstraint *> &marked_constraints) {
    // find root
    for (int i = 0; i < (int)rod_segments.size(); i++) {
        RodSegment *rb(rod_segments[i]);
        if (!IsSegmentInInterval(rb, interval_index, intervals, rod_constraints, rod_segments))
            continue;
        else {
            if (root[interval_index].object == nullptr) {
                root[interval_index].object = rb;
                root[interval_index].index = i;
            }
        }

        if (!rb->IsDynamic()) {
            root[interval_index].object = rb;
            root[interval_index].index = i;
            break;
        }
    }
    root[interval_index].isconstraint = false;
    root[interval_index].parent = nullptr;

    root[interval_index].D.setZero();
    root[interval_index].Dinv.setZero();
    root[interval_index].soln.setZero();

    InitSegmentNode(&root[interval_index], interval_index, rod_constraints, rod_segments, marked_constraints,
                    intervals);
    OrderMatrix(&root[interval_index], interval_index, forward, backward);
}

void DirectPositionBasedSolverForStiffRods::InitTree(std::vector<RodConstraint *> &rod_constraints,
                                                     std::vector<RodSegment *> &rod_segments,
                                                     Interval *&intervals,
                                                     int &number_of_intervals,
                                                     std::list<Node *> *&forward,
                                                     std::list<Node *> *&backward,
                                                     Node *&root) {
    number_of_intervals = 1;
    intervals = new Interval[1];
    intervals[0].start = 0;
    intervals[0].end = (int)rod_constraints.size() - 1;
    InitLists(number_of_intervals, forward, backward, root);

    std::vector<RodConstraint *> marked_constraints;
    for (int i = 0; i < number_of_intervals; i++) {
        InitNodes(i, rod_segments, root, intervals, rod_constraints, forward, backward, marked_constraints);
        marked_constraints.clear();
    }
}

bool DirectPositionBasedSolverForStiffRods::ComputeDarbouxVector(const Quaternionr &q0,
                                                                 const Quaternionr &q1,
                                                                 Real average_segment_length,
                                                                 Vector3r &darboux_vector) {
    darboux_vector = 2. / average_segment_length * (q0.conjugate() * q1).vec();
    return true;
}

bool DirectPositionBasedSolverForStiffRods::ComputeBendingAndTorsionJacobians(const Quaternionr &q0,
                                                                              const Quaternionr &q1,
                                                                              Real average_segment_length,
                                                                              Eigen::Matrix<Real, 3, 4> &j_omega_0,
                                                                              Eigen::Matrix<Real, 3, 4> &j_omega_1) {
    j_omega_0 << -q1.w(), -q1.z(), q1.y(), q1.x(), q1.z(), -q1.w(), -q1.x(), q1.y(), -q1.y(), q1.x(), -q1.w(), q1.z();
    j_omega_1 << q0.w(), q0.z(), -q0.y(), -q0.x(), -q0.z(), q0.w(), q0.x(), -q0.y(), q0.y(), -q0.x(), q0.w(), -q0.z();
    j_omega_0 *= static_cast<Real>(2.0) / average_segment_length;
    j_omega_1 *= static_cast<Real>(2.0) / average_segment_length;
    return true;
}

bool DirectPositionBasedSolverForStiffRods::ComputeMatrixG(const Quaternionr &q, Eigen::Matrix<Real, 4, 3> &G) {
    // w component at index 3
    G << static_cast<Real>(0.5) * q.w(), static_cast<Real>(0.5) * q.z(), -static_cast<Real>(0.5) * q.y(),
            -static_cast<Real>(0.5) * q.z(), static_cast<Real>(0.5) * q.w(), static_cast<Real>(0.5) * q.x(),
            static_cast<Real>(0.5) * q.y(), -static_cast<Real>(0.5) * q.x(), static_cast<Real>(0.5) * q.w(),
            -static_cast<Real>(0.5) * q.x(), -static_cast<Real>(0.5) * q.y(), -static_cast<Real>(0.5) * q.z();
    return true;
}

void DirectPositionBasedSolverForStiffRods::ComputeMatrixK(
        const Vector3r &connector, Real inv_mass, const Vector3r &x, const Matrix3r &inertia_inverse_w, Matrix3r &K) {
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

        K(0, 0) = c * c * j22 - b * c * (j23 + j23) + b * b * j33 + inv_mass;
        K(0, 1) = -(c * c * j12) + a * c * j23 + b * c * j13 - a * b * j33;
        K(0, 2) = b * c * j12 - a * c * j22 - b * b * j13 + a * b * j23;
        K(1, 0) = K(0, 1);
        K(1, 1) = c * c * j11 - a * c * (j13 + j13) + a * a * j33 + inv_mass;
        K(1, 2) = -(b * c * j11) + a * c * j12 + a * b * j13 - a * a * j23;
        K(2, 0) = K(0, 2);
        K(2, 1) = K(1, 2);
        K(2, 2) = b * b * j11 - a * b * (j12 + j12) + a * a * j22 + inv_mass;
    } else
        K.setZero();
}

void DirectPositionBasedSolverForStiffRods::GetMassMatrix(RodSegment *segment, Matrix6r &M) {
    if (!segment->IsDynamic()) {
        M = Matrix6r::Identity();
        return;
    }

    const Vector3r &inertia_local = segment->InertiaTensor();
    Matrix3r rotation_matrix(segment->Rotation().toRotationMatrix());
    Matrix3r inertia(rotation_matrix * Eigen::DiagonalMatrix<Real, 3>(inertia_local) * rotation_matrix.transpose());

    Real mass = segment->Mass();

    // Upper half
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 6; j++)
            if (i == j)
                M(i, j) = mass;
            else
                M(i, j) = 0.0;

    // lower left
    for (int i = 3; i < 6; i++)
        for (int j = 0; j < 3; j++) M(i, j) = 0.0;

    // lower right
    for (int i = 3; i < 6; i++)
        for (int j = 3; j < 6; j++) M(i, j) = inertia(i - 3, j - 3);
}

Real DirectPositionBasedSolverForStiffRods::Factor(const int interval_index,
                                                   const std::vector<RodConstraint *> &rod_constraints,
                                                   std::vector<RodSegment *> &rod_segments,
                                                   const Interval *&intervals,
                                                   std::list<Node *> *forward,
                                                   std::list<Node *> *backward,
                                                   std::vector<Vector6r> &RHS,
                                                   std::vector<Vector6r> &lambda_sums,
                                                   std::vector<std::vector<Matrix3r>> &bending_and_torsion_jacobians) {
    Real max_error(0.);

    // compute right hand side of linear equation system
    for (size_t current_constraint_index = 0; current_constraint_index < rod_constraints.size();
         ++current_constraint_index) {
        RodConstraint *current_constraint = rod_constraints[current_constraint_index];

        RodSegment *segment0 = rod_segments[current_constraint->SegmentIndex(0)];
        RodSegment *segment1 = rod_segments[current_constraint->SegmentIndex(1)];

        const Quaternionr &q0 = segment0->Rotation();
        const Quaternionr &q1 = segment1->Rotation();

        const Eigen::Matrix<Real, 3, 4, Eigen::DontAlign> &constraint_info(current_constraint->GetConstraintInfo());
        Vector6r &rhs(RHS[current_constraint_index]);

        // Compute zero-stretch part of constraint violation
        const Vector3r &connector0 = constraint_info.col(2);
        const Vector3r &connector1 = constraint_info.col(3);
        Vector3r stretch_violation = connector0 - connector1;

        // compute Darboux vector (Equation (7))
        Vector3r omega;
        ComputeDarbouxVector(q0, q1, current_constraint->GetAverageSegmentLength(), omega);

        // Compute bending and torsion part of constraint violation
        Vector3r bending_and_torsion_violation = omega - current_constraint->GetRestDarbouxVector();

        // fill right hand side of the linear equation system
        const Vector6r &lambda_sum(lambda_sums[current_constraint_index]);
        rhs.block<3, 1>(0, 0) = -stretch_violation - Vector3r(current_constraint->GetStretchCompliance().array() *
                                                              lambda_sum.block<3, 1>(0, 0).array());

        rhs.block<3, 1>(3, 0) =
                -bending_and_torsion_violation - Vector3r(current_constraint->GetBendingAndTorsionCompliance().array() *
                                                          lambda_sum.block<3, 1>(3, 0).array());

        // compute max error
        for (unsigned char i(0); i < 6; ++i) {
            max_error = std::max(max_error, std::abs(rhs[i]));
        }

        // Compute a part of the Jacobian here, because the relationship
        // of the first and second segment to the constraint can be determined directly

        // compute G matrices
        Eigen::Matrix<Real, 4, 3> G0, G1;
        ComputeMatrixG(q0, G0);
        ComputeMatrixG(q1, G1);

        // compute stretching bending Jacobians (Equation (10) and Equation (11))
        Eigen::Matrix<Real, 3, 4> jOmega0, jOmega1;
        ComputeBendingAndTorsionJacobians(q0, q1, current_constraint->GetAverageSegmentLength(), jOmega0, jOmega1);

        bending_and_torsion_jacobians[current_constraint_index][0] = jOmega0 * G0;
        bending_and_torsion_jacobians[current_constraint_index][1] = jOmega1 * G1;
    }

    std::list<Node *>::iterator node_iter;
    for (node_iter = forward[interval_index].begin(); node_iter != forward[interval_index].end(); node_iter++) {
        Node *node = *node_iter;
        // compute system matrix diagonal
        if (node->isconstraint) {
            auto *current_constraint = (RodConstraint *)node->object;
            // insert compliance
            node->D.setZero();
            const Vector3r &stretch_compliance(current_constraint->GetStretchCompliance());

            node->D(0, 0) -= stretch_compliance[0];
            node->D(1, 1) -= stretch_compliance[1];
            node->D(2, 2) -= stretch_compliance[2];

            const Vector3r &bending_and_torsion_compliance(current_constraint->GetBendingAndTorsionCompliance());
            node->D(3, 3) -= bending_and_torsion_compliance[0];
            node->D(4, 4) -= bending_and_torsion_compliance[1];
            node->D(5, 5) -= bending_and_torsion_compliance[2];
        } else {
            GetMassMatrix((RodSegment *)node->object, node->D);
        }

        // compute Jacobian
        if (node->parent != nullptr) {
            if (node->isconstraint) {
                // compute J
                auto *constraint = (RodConstraint *)node->object;
                auto *segment = (RodSegment *)node->parent->object;

                Real sign = 1;
                int segment_index = 0;
                if (segment == rod_segments[constraint->SegmentIndex(1)]) {
                    segment_index = 1;
                    sign = -1;
                }

                const Eigen::Matrix<Real, 3, 4, Eigen::DontAlign> &constraint_info(constraint->GetConstraintInfo());
                const Vector3r r = constraint_info.col(2 + segment_index) - segment->Position();
                Matrix3r r_cross;
                Real cross_sign(-static_cast<Real>(1.0) * sign);
                MathFunctions::CrossProductMatrix(cross_sign * r, r_cross);

                Eigen::DiagonalMatrix<Real, 3> upper_left(sign, sign, sign);
                node->J.block<3, 3>(0, 0) = upper_left;

                Matrix3r lower_left(Matrix3r::Zero());
                node->J.block<3, 3>(3, 0) = lower_left;

                node->J.block<3, 3>(0, 3) = r_cross;

                Matrix3r &lower_right(bending_and_torsion_jacobians[node->index][segment_index]);
                node->J.block<3, 3>(3, 3) = lower_right;
            } else {
                // compute JT
                auto *constraint = (RodConstraint *)node->parent->object;
                auto *segment = (RodSegment *)node->object;

                Real sign = 1;
                int segment_index = 0;
                if (segment == rod_segments[constraint->SegmentIndex(1)]) {
                    segment_index = 1;
                    sign = -1;
                }

                const Eigen::Matrix<Real, 3, 4, Eigen::DontAlign> &constraint_info(constraint->GetConstraintInfo());
                const Vector3r r = constraint_info.col(2 + segment_index) - segment->Position();
                Matrix3r r_crossT;
                MathFunctions::CrossProductMatrix(sign * r, r_crossT);

                Eigen::DiagonalMatrix<Real, 3> upper_left(sign, sign, sign);
                node->J.block<3, 3>(0, 0) = upper_left;

                node->J.block<3, 3>(3, 0) = r_crossT;

                Matrix3r upper_right(Matrix3r::Zero());
                node->J.block<3, 3>(0, 3) = upper_right;

                Matrix3r lower_right(bending_and_torsion_jacobians[node->parent->index][segment_index].transpose());
                node->J.block<3, 3>(3, 3) = lower_right;
            }
        }
    }

    for (node_iter = forward[interval_index].begin(); node_iter != forward[interval_index].end(); node_iter++) {
        Node *node = *node_iter;
        std::vector<Node *> children = node->children;
        for (auto &i : children) {
            Matrix6r JT = (i->J).transpose();
            Matrix6r &D = i->D;
            Matrix6r &J = i->J;
            Matrix6r JTDJ = ((JT * D) * J);
            node->D = node->D - JTDJ;
        }
        bool chk = false;
        if (!node->isconstraint) {
            auto *segment = (RodSegment *)node->object;
            if (!segment->IsDynamic()) {
                node->Dinv.setZero();
                chk = true;
            }
        }

        node->DLDLT.compute(node->D);  // result reused in solve()
        if (node->parent != nullptr) {
            if (!chk) {
                node->J = node->DLDLT.solve(node->J);
            } else {
                node->J.setZero();
            }
        }
    }
    return max_error;
}

bool DirectPositionBasedSolverForStiffRods::Solve(int interval_index,
                                                  std::list<Node *> *forward,
                                                  std::list<Node *> *backward,
                                                  std::vector<Vector6r> &RHS,
                                                  std::vector<Vector6r> &lambda_sums,
                                                  std::vector<Vector3r> &corr_x,
                                                  std::vector<Quaternionr> &corr_q) {
    std::list<Node *>::iterator node_iter;
    for (node_iter = forward[interval_index].begin(); node_iter != forward[interval_index].end(); node_iter++) {
        Node *node = *node_iter;
        if (node->isconstraint) {
            node->soln = -RHS[node->index];
        } else {
            node->soln.setZero();
        }
        std::vector<Node *> &children = node->children;
        for (auto &i : children) {
            Matrix6r cJT = i->J.transpose();
            Vector6r &csoln = i->soln;
            Vector6r v = cJT * csoln;
            node->soln = node->soln - v;
        }
    }

    for (node_iter = backward[interval_index].begin(); node_iter != backward[interval_index].end(); node_iter++) {
        Node *node = *node_iter;

        bool no_zero_dinv(true);
        if (!node->isconstraint) {
            auto *segment = (RodSegment *)node->object;
            no_zero_dinv = segment->IsDynamic();
        }
        if (no_zero_dinv)  // if DInv == 0 child value is 0 and node->soln is not altered
        {
            node->soln = node->DLDLT.solve(node->soln);

            if (node->parent != nullptr) {
                node->soln -= node->J * node->parent->soln;
            }
        } else {
            node->soln.setZero();  // segment of node is not dynamic
        }

        if (node->isconstraint) {
            lambda_sums[node->index] += node->soln;
        }
    }

    // compute position and orientation updates
    for (node_iter = forward[interval_index].begin(); node_iter != forward[interval_index].end(); node_iter++) {
        Node *node = *node_iter;
        if (!node->isconstraint) {
            auto *segment = (RodSegment *)node->object;
            if (!segment->IsDynamic()) {
                break;
            }

            const Vector6r &soln(node->soln);
            Vector3r delta_x_soln = Vector3r(-soln[0], -soln[1], -soln[2]);
            corr_x[node->index] = delta_x_soln;

            Eigen::Matrix<Real, 4, 3> G;
            ComputeMatrixG(segment->Rotation(), G);
            Quaternionr delta_q_soln;
            delta_q_soln.coeffs() = G * Vector3r(-soln[3], -soln[4], -soln[5]);
            corr_q[node->index] = delta_q_soln;
        }
    }
    return true;
}

bool DirectPositionBasedSolverForStiffRods::InitDirectPositionBasedSolverForStiffRodsConstraint(
        std::vector<RodConstraint *> &rod_constraints,
        std::vector<RodSegment *> &rod_segments,
        Interval *&intervals,
        int &number_of_intervals,
        std::list<Node *> *&forward,
        std::list<Node *> *&backward,
        Node *&root,
        const std::vector<Vector3r> &constraint_positions,
        const std::vector<Real> &average_radii,
        const std::vector<Real> &youngs_moduli,
        const std::vector<Real> &torsion_moduli,
        std::vector<Vector6r> &RHS,
        std::vector<Vector6r> &lambda_sums,
        std::vector<std::vector<Matrix3r>> &bending_and_torsion_jacobians,
        std::vector<Vector3r> &corr_x,
        std::vector<Quaternionr> &corr_q) {
    // init constraints
    for (size_t cIdx(0); cIdx < rod_constraints.size(); ++cIdx) {
        RodConstraint *constraint(rod_constraints[cIdx]);
        RodSegment *segment0(rod_segments[constraint->SegmentIndex(0)]);
        RodSegment *segment1(rod_segments[constraint->SegmentIndex(1)]);

        InitStretchBendingTwistingConstraint(
                segment0->Position(), segment0->Rotation(), segment1->Position(), segment1->Rotation(),
                constraint_positions[cIdx], average_radii[cIdx], constraint->GetAverageSegmentLength(),
                youngs_moduli[cIdx], torsion_moduli[cIdx], constraint->GetConstraintInfo(),
                constraint->GetStiffnessCoefficientK(), constraint->GetRestDarbouxVector());
    }

    // compute tree data structure for direct solver
    InitTree(rod_constraints, rod_segments, intervals, number_of_intervals, forward, backward, root);

    RHS.resize(rod_constraints.size());
    std::fill(RHS.begin(), RHS.end(), Vector6r::Zero());

    lambda_sums.resize(rod_constraints.size());
    std::fill(lambda_sums.begin(), lambda_sums.end(), Vector6r::Zero());

    bending_and_torsion_jacobians.resize(rod_constraints.size());
    std::vector<Matrix3r> sample_jacobians(2);
    sample_jacobians[0].setZero();
    sample_jacobians[1].setZero();
    std::fill(bending_and_torsion_jacobians.begin(), bending_and_torsion_jacobians.end(), sample_jacobians);

    corr_x.resize(rod_segments.size());
    std::fill(corr_x.begin(), corr_x.end(), Vector3r::Zero());

    corr_q.resize(rod_segments.size());
    std::fill(corr_q.begin(), corr_q.end(), Quaternionr::Identity());

    return true;
}

bool DirectPositionBasedSolverForStiffRods::InitBeforeProjectionDirectPositionBasedSolverForStiffRodsConstraint(
        const std::vector<RodConstraint *> &rod_constraints,
        Real inverse_time_step_size,
        std::vector<Vector6r> &lambda_sums) {
    for (size_t cIdx(0); cIdx < rod_constraints.size(); ++cIdx) {
        RodConstraint *constraint(rod_constraints[cIdx]);

        InitBeforeProjectionStretchBendingTwistingConstraint(
                constraint->GetStiffnessCoefficientK(), inverse_time_step_size, constraint->GetAverageSegmentLength(),
                constraint->GetStretchCompliance(), constraint->GetBendingAndTorsionCompliance(), lambda_sums[cIdx]);
    }
    return true;
}

bool DirectPositionBasedSolverForStiffRods::UpdateDirectPositionBasedSolverForStiffRodsConstraint(
        const std::vector<RodConstraint *> &rod_constraints, const std::vector<RodSegment *> &rod_segments) {
    // update rod constraints
    for (size_t cIdx(0); cIdx < rod_constraints.size(); ++cIdx) {
        RodConstraint *constraint(rod_constraints[cIdx]);
        RodSegment *segment0(rod_segments[constraint->SegmentIndex(0)]);
        RodSegment *segment1(rod_segments[constraint->SegmentIndex(1)]);

        UpdateStretchBendingTwistingConstraint(segment0->Position(), segment0->Rotation(), segment1->Position(),
                                               segment1->Rotation(), constraint->GetConstraintInfo());
    }
    return true;
}

bool DirectPositionBasedSolverForStiffRods::SolveDirectPositionBasedSolverForStiffRodsConstraint(
        const std::vector<RodConstraint *> &rod_constraints,
        std::vector<RodSegment *> &rod_segments,
        const Interval *intervals,
        const int &number_of_intervals,
        std::list<Node *> *forward,
        std::list<Node *> *backward,
        std::vector<Vector6r> &RHS,
        std::vector<Vector6r> &lambda_sums,
        std::vector<std::vector<Matrix3r>> &bending_and_torsion_jacobians,
        std::vector<Vector3r> &corr_x,
        std::vector<Quaternionr> &corr_q) {
    for (int i = 0; i < number_of_intervals; i++) {
        Factor(i, rod_constraints, rod_segments, intervals, forward, backward, RHS, lambda_sums,
               bending_and_torsion_jacobians);
    }
    for (int i = 0; i < number_of_intervals; i++) {
        Solve(i, forward, backward, RHS, lambda_sums, corr_x, corr_q);
    }
    return true;
}

bool DirectPositionBasedSolverForStiffRods::InitStretchBendingTwistingConstraint(
        const Vector3r &x0,
        const Quaternionr &q0,
        const Vector3r &x1,
        const Quaternionr &q1,
        const Vector3r &constraint_position,
        Real average_radius,
        Real average_segment_length,
        Real youngs_modulus,
        Real torsion_modulus,
        Eigen::Matrix<Real, 3, 4, Eigen::DontAlign> &joint_info,
        Vector3r &stiffness_coefficient_k,
        Vector3r &rest_darboux_vector) {
    // constraintInfo contains
    // 0:	connector in segment 0 (local)
    // 1:	connector in segment 1 (local)
    // 2:	connector in segment 0 (global)
    // 3:	connector in segment 1 (global)

    // transform in local coordinates
    const Matrix3r rot0T = q0.matrix().transpose();
    const Matrix3r rot1T = q1.matrix().transpose();

    joint_info.col(0) = rot0T * (constraint_position - x0);
    joint_info.col(1) = rot1T * (constraint_position - x1);
    joint_info.col(2) = constraint_position;
    joint_info.col(3) = constraint_position;

    // compute bending and torsion stiffness of the K matrix diagonal; assumption: the rod axis follows the y-axis of
    // the local frame as with Blender's armatures
    Real second_moment_of_area(static_cast<Real>(M_PI_4) * std::pow(average_radius, static_cast<Real>(4.0)));
    Real bending_stiffness(youngs_modulus * second_moment_of_area);
    Real torsion_stiffness(static_cast<Real>(2.0) * torsion_modulus * second_moment_of_area);
    stiffness_coefficient_k = Vector3r(bending_stiffness, torsion_stiffness, bending_stiffness);

    // compute rest Darboux vector
    ComputeDarbouxVector(q0, q1, average_segment_length, rest_darboux_vector);

    return true;
}

bool DirectPositionBasedSolverForStiffRods::InitBeforeProjectionStretchBendingTwistingConstraint(
        const Vector3r &stiffness_coefficient_k,
        Real inverse_time_step_size,
        Real average_segment_length,
        Vector3r &stretch_compliance,
        Vector3r &bending_and_torsion_compliance,
        Vector6r &lambda_sum) {
    Real inverse_ts_quadratic(inverse_time_step_size * inverse_time_step_size);

    // compute compliance parameter of the stretch constraint part
    const Real kStretchRegularizationParameter(static_cast<Real>(1.E-10));
    stretch_compliance << kStretchRegularizationParameter * inverse_ts_quadratic,
            kStretchRegularizationParameter * inverse_ts_quadratic,
            kStretchRegularizationParameter * inverse_ts_quadratic;

    // compute compliance parameter of the bending and torsion constraint part
    bending_and_torsion_compliance << inverse_ts_quadratic / stiffness_coefficient_k(0),
            inverse_ts_quadratic / stiffness_coefficient_k(1), inverse_ts_quadratic / stiffness_coefficient_k(2);
    bending_and_torsion_compliance *= static_cast<Real>(1.0) / average_segment_length;

    // set sum of delta lambda values to zero
    lambda_sum.setZero();
    return true;
}

bool DirectPositionBasedSolverForStiffRods::UpdateStretchBendingTwistingConstraint(
        const Vector3r &x0,
        const Quaternionr &q0,
        const Vector3r &x1,
        const Quaternionr &q1,
        Eigen::Matrix<Real, 3, 4, Eigen::DontAlign> &joint_info) {
    // constraintInfo contains
    // 0:	connector in segment 0 (local)
    // 1:	connector in segment 1 (local)
    // 2:	connector in segment 0 (global)
    // 3:	connector in segment 1 (global)

    // compute world space positions of connectors
    const Matrix3r rot0 = q0.matrix();
    const Matrix3r rot1 = q1.matrix();
    joint_info.col(2) = rot0 * joint_info.col(0) + x0;
    joint_info.col(3) = rot1 * joint_info.col(1) + x1;

    return true;
}

bool DirectPositionBasedSolverForStiffRods::SolveStretchBendingTwistingConstraint(
        Real inv_mass_0,
        const Vector3r &x0,
        const Matrix3r &inertia_inverse_w_0,
        const Quaternionr &q0,
        Real inv_mass_1,
        const Vector3r &x1,
        const Matrix3r &inertia_inverse_w_1,
        const Quaternionr &q1,
        const Vector3r &rest_darboux_vector,
        Real average_segment_length,
        const Vector3r &stretch_compliance,
        const Vector3r &bending_and_torsion_compliance,
        const Eigen::Matrix<Real, 3, 4, Eigen::DontAlign> &joint_info,
        Vector3r &corr_x0,
        Quaternionr &corr_q0,
        Vector3r &corr_x1,
        Quaternionr &corr_q1,
        Vector6r &lambda_sum) {
    // compute Darboux vector (Equation (7))
    Vector3r omega;
    ComputeDarbouxVector(q0, q1, average_segment_length, omega);

    // compute bending and torsion Jacobians (Equation (10) and Equation (11))
    Eigen::Matrix<Real, 3, 4> jOmega0, jOmega1;
    ComputeBendingAndTorsionJacobians(q0, q1, average_segment_length, jOmega0, jOmega1);

    // compute G matrices (Equation (27))
    Eigen::Matrix<Real, 4, 3> G0, G1;
    ComputeMatrixG(q0, G0);
    ComputeMatrixG(q1, G1);

    Matrix3r jOmegaG0(jOmega0 * G0), jOmegaG1(jOmega1 * G1);

    // Compute zero-stretch part of constraint violation (Equation (23))
    const Vector3r &connector0 = joint_info.col(2);
    const Vector3r &connector1 = joint_info.col(3);
    Vector3r stretch_violation = connector0 - connector1;

    // Compute bending and torsion part of constraint violation  (Equation (23))
    Vector3r bending_and_torsion_violation = omega - rest_darboux_vector;

    // fill right hand side of the linear equation system (Equation (19))
    Vector6r rhs;
    rhs.block<3, 1>(0, 0) =
            -stretch_violation - Vector3r(stretch_compliance.array() * lambda_sum.block<3, 1>(0, 0).array());

    rhs.block<3, 1>(3, 0) = -bending_and_torsion_violation -
                            Vector3r(bending_and_torsion_compliance.array() * lambda_sum.block<3, 1>(3, 0).array());

    // compute matrix of the linear equation system (using Equations (25), (26), and (28) in Equation (19))
    Matrix6r JMJT(Matrix6r::Zero());

    // compute stretch block
    Matrix3r K1, K2;
    ComputeMatrixK(connector0, inv_mass_0, x0, inertia_inverse_w_0, K1);
    ComputeMatrixK(connector1, inv_mass_1, x1, inertia_inverse_w_1, K2);
    JMJT.block<3, 3>(0, 0) = K1 + K2;

    // compute coupling blocks
    const Vector3r ra = connector0 - x0;
    const Vector3r rb = connector1 - x1;

    Matrix3r ra_crossT, rb_crossT;
    MathFunctions::CrossProductMatrix(-ra, ra_crossT);  // use -ra to get the transpose
    MathFunctions::CrossProductMatrix(-rb, rb_crossT);  // use -rb to get the transpose

    Matrix3r offdiag(Matrix3r::Zero());
    if (inv_mass_0 != 0.0) {
        offdiag = jOmegaG0 * inertia_inverse_w_0 * ra_crossT * (-1);
    }

    if (inv_mass_1 != 0.0) {
        offdiag += jOmegaG1 * inertia_inverse_w_1 * rb_crossT;
    }
    JMJT.block<3, 3>(3, 0) = offdiag;
    JMJT.block<3, 3>(0, 3) = offdiag.transpose();

    // compute bending and torsion block
    Matrix3r MInvJT0(inertia_inverse_w_0 * jOmegaG0.transpose());
    Matrix3r MInvJT1(inertia_inverse_w_1 * jOmegaG1.transpose());

    Matrix3r jmjt_omega(Matrix3r::Zero());
    if (inv_mass_0 != 0.0) {
        jmjt_omega = jOmegaG0 * MInvJT0;
    }

    if (inv_mass_1 != 0.0) {
        jmjt_omega += jOmegaG1 * MInvJT1;
    }
    JMJT.block<3, 3>(3, 3) = jmjt_omega;

    // add compliance
    JMJT(0, 0) += stretch_compliance(0);
    JMJT(1, 1) += stretch_compliance(1);
    JMJT(2, 2) += stretch_compliance(2);
    JMJT(3, 3) += bending_and_torsion_compliance(0);
    JMJT(4, 4) += bending_and_torsion_compliance(1);
    JMJT(5, 5) += bending_and_torsion_compliance(2);

    // solve linear equation system (Equation 19)
    auto decomposition(JMJT.ldlt());
    Vector6r delta_lambda(decomposition.solve(rhs));

    // update sum of delta lambda values for next Gauss-Seidel solver iteration step
    lambda_sum += delta_lambda;

    // compute position and orientation updates (using Equations (25), (26), and (28) in Equation (20))
    Vector3r delta_lambda_stretch(delta_lambda.block<3, 1>(0, 0)),
            delta_lambda_bending_and_torsion(delta_lambda.block<3, 1>(3, 0));
    corr_x0.setZero();
    corr_x1.setZero();
    corr_q0.coeffs().setZero();
    corr_q1.coeffs().setZero();

    if (inv_mass_0 != 0.) {
        corr_x0 += inv_mass_0 * delta_lambda_stretch;
        corr_q0.coeffs() += G0 * (inertia_inverse_w_0 * ra_crossT * (-1 * delta_lambda_stretch) +
                                  MInvJT0 * delta_lambda_bending_and_torsion);
    }

    if (inv_mass_1 != 0.) {
        corr_x1 -= inv_mass_1 * delta_lambda_stretch;
        corr_q1.coeffs() += G1 * (inertia_inverse_w_1 * rb_crossT * delta_lambda_stretch +
                                  MInvJT1 * delta_lambda_bending_and_torsion);
    }

    return true;
}

}  // namespace vox::compute
