//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <Eigen/Dense>
#include <list>
#include <vector>

#include "vox.force/common.h"
#include "vox.force/direct_position_based_solver_for_stiff_rods_interface.h"

// ------------------------------------------------------------------------------------
namespace vox::force {
// Implementation of "Direct Position-Based Solver for Stiff Rods" paper
// (https://animation.rwth-aachen.de/publication/0557/)
//
//	Implemented by:
//
//	Crispin Deul
//	Graduate School CE
//	Technische Universit�t Darmstadt
//
//  deul[at] gsc.tu-darmstadt.de
//

using Matrix6r = Eigen::Matrix<Real, 6, 6, Eigen::DontAlign>;
using Vector6r = Eigen::Matrix<Real, 6, 1, Eigen::DontAlign>;
/** Node in the simulated tree structure */
struct Node {
    Node() {
        object = nullptr;
        D = Dinv = J = Matrix6r::Zero();
        parent = nullptr;
        soln.setZero();
        index = 0;
    };
    bool isconstraint{};
    void *object;
    Matrix6r D, Dinv, J;
    std::vector<Node *> children;
    Node *parent;
    Vector6r soln;
    int index;
    Eigen::LDLT<Matrix6r> DLDLT;
};

struct Interval {
    int start;
    int end;
};

class DirectPositionBasedSolverForStiffRods {
private:
    static void InitLists(int number_of_intervals,
                          std::list<Node *> *&forward,
                          std::list<Node *> *&backward,
                          Node *&root);

    /**
     * Returns, whether the passed segment is connected to a constraint in the
     * passed index range of the entire constraints.
     */
    static bool IsSegmentInInterval(RodSegment *segment,
                                    int interval_index,
                                    Interval *intervals,
                                    std::vector<RodConstraint *> &rod_constraints,
                                    std::vector<RodSegment *> &rod_segments);

    /**
     * Returns, whether the passed constraint is within the passed index range
     * of the entire constraints.
     */
    static bool IsConstraintInInterval(RodConstraint *constraint,
                                       int interval_index,
                                       Interval *intervals,
                                       std::vector<RodConstraint *> &rod_constraints);

    /**
     * This method recursively visits all segment nodes and inserts constraint nodes between them.
     */
    static void InitSegmentNode(Node *n,
                                int interval_index,
                                std::vector<RodConstraint *> &rod_constraints,
                                std::vector<RodSegment *> &rod_segments,
                                std::vector<RodConstraint *> &marked_constraints,
                                Interval *intervals);

    /**
     * Sorts matrix H.
     */
    static void OrderMatrix(Node *n, int interval_index, std::list<Node *> *forward, std::list<Node *> *backward);

    /**
     * Initializes the nodes.
     * The first static node is selected as the root of the tree.
     * Then, starting from this node, all edges (joints) are followed
     * and the children and the parent node are saved.
     */
    static void InitNodes(int interval_index,
                          std::vector<RodSegment *> &rod_segments,
                          Node *&root,
                          Interval *intervals,
                          std::vector<RodConstraint *> &rod_constraints,
                          std::list<Node *> *forward,
                          std::list<Node *> *backward,
                          std::vector<RodConstraint *> &marked_constraints);

    static void InitTree(std::vector<RodConstraint *> &rod_constraints,
                         std::vector<RodSegment *> &rod_segments,
                         Interval *&intervals,
                         int &number_of_intervals,
                         std::list<Node *> *&forward,
                         std::list<Node *> *&backward,
                         Node *&root);

    /**
     * Compute the discrete Darboux vector based on Equation (7)
     */
    static bool ComputeDarbouxVector(const Quaternionr &q0,
                                     const Quaternionr &q1,
                                     Real average_segment_length,
                                     Vector3r &darboux_vector);

    static bool ComputeBendingAndTorsionJacobians(const Quaternionr &q0,
                                                  const Quaternionr &q1,
                                                  Real average_segment_length,
                                                  Eigen::Matrix<Real, 3, 4> &j_omega_0,
                                                  Eigen::Matrix<Real, 3, 4> &j_omega_1);

    static bool ComputeMatrixG(const Quaternionr &q, Eigen::Matrix<Real, 4, 3> &G);

    static void ComputeMatrixK(const Vector3r &connector,
                               Real inv_mass,
                               const Vector3r &x,
                               const Matrix3r &inertia_inverse_w,
                               Matrix3r &K);

    /**
     * Returns the 6x6 mass matrix M of a segment
     */
    static void GetMassMatrix(RodSegment *segment, Matrix6r &M);

    /**
     * Factorizes matrix H and computes the right hand side vector -b.
     */
    static Real Factor(int interval_index,
                       const std::vector<RodConstraint *> &rod_constraints,
                       std::vector<RodSegment *> &rod_segments,
                       const Interval *&intervals,
                       std::list<Node *> *forward,
                       std::list<Node *> *backward,
                       std::vector<Vector6r> &RHS,
                       std::vector<Vector6r> &lambda_sums,
                       std::vector<std::vector<Matrix3r>> &bending_and_torsion_jacobians);

    /**
     * Solves the system of equations with the factorized matrix H.
     */
    static bool Solve(int interval_index,
                      std::list<Node *> *forward,
                      std::list<Node *> *backward,
                      std::vector<Vector6r> &RHS,
                      std::vector<Vector6r> &lambda_sums,
                      std::vector<Vector3r> &corr_x,
                      std::vector<Quaternionr> &corr_q);

public:
    /**
     * Initialize the zero-stretch, bending, and torsion constraints of the rod.
     * Computes constraint connectors in segment space, computes the diagonal stiffness matrices
     * and the Darboux vectors of the initial state. Initializes the forward and backward lists
     * of nodes  for the direct solver
     *
     * @param rod_constraints contains the combined zero-stretch, bending
     * and torsion constraints of the rod. The set of constraints must by acyclic.
     * @param rod_segments contains the segments of the rod
     * @param forward list of nodes in the acyclic tree of rod segments and zero-stretch,
     * bending and torsion constraints so that parent nodes occur later in the list than their children
     * @param backward list of nodes in the acyclic tree of rod segments and zero-stretch,
     * bending and torsion constraints. The reverse of the forward list.
     * @param constraint_positions positions of the rod's constraints in world coordinates
     * @param average_radii the average radii at the constraint positions of the rod. Value in Meters (m)
     * @param average_segment_lengths vector of the average lengths of the two rod segments
     * connected by a constraint of the rod. Value in Meters (m)
     * @param youngs_moduli vector of the Young's modulus of every constraint of the rod.
     * The Young's modulus of the rod material measures
     * stiffness against bending. Value in Pascal (Pa)
     * @param torsion_moduli vector of the torsion modulus of every constraint of the rod.
     * The torsion modulus (also called shear modulus)
     * of the rod material measures stiffness against torsion. Value in Pascal (Pa)
     * @param RHS vector with entries for each constraint. In concatenation these entries represent the right hand side
     * of the system of equations to be solved. (eq. 22 in the paper)
     * @param lambda_sums contains entries of the sum of all lambda updates for
     * each constraint in the rod during one time step which is needed by the solver to handle
     * compliance in the correct way (cf. the right hand side of eq. 22 in the paper).
     * @param bending_and_torsion_jacobians this vector is used to temporary
     * save the Jacobians of the bending and torsion part of each constraint
     * during the solution process of the system of equations. Allocating this
     * vector outside of the solve-method avoids repeated reallocation between iterations of the solver
     * @param corr_x vector of position corrections for every segment of the rod (part of delta-x in eq. 22 in the
     * paper)
     * @param corr_q vector of rotation corrections for every segment of the rod (part of delta-x in eq. 22 in the
     * paper)
     */
    static bool InitDirectPositionBasedSolverForStiffRodsConstraint(
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
            std::vector<Quaternionr> &corr_q);

    /**
     * Update the constraint info data.
     *
     * @param rod_constraints contains the combined zero-stretch, bending and torsion constraints of the rod.
     * @param rod_segments contains the segments of the rod
     */
    static bool UpdateDirectPositionBasedSolverForStiffRodsConstraint(
            const std::vector<RodConstraint *> &rod_constraints, const std::vector<RodSegment *> &rod_segments);

    /**
     * Initialize the constraint before the projection iterations in each time step.
     *
     * @param rod_constraints contains the combined zero-stretch, bending and torsion constraints of the rod.
     * @param inverse_time_step_size inverse of the current time step size used to compute compliance (see computation
     * of alpha-tilde in eq. 17)
     * @param lambda_sums contains entries of the sum of all lambda updates for
     * each constraint in the rod during one time step which is needed by the solver to handle
     * compliance in the correct way (cf. the right hand side of eq. 22 in the paper).
     */
    static bool InitBeforeProjectionDirectPositionBasedSolverForStiffRodsConstraint(
            const std::vector<RodConstraint *> &rod_constraints,
            Real inverse_time_step_size,
            std::vector<Vector6r> &lambda_sums);

    /**
     * Determine the position and orientation corrections for all combined zero-stretch, bending and twisting
     * constraints of the rod (eq. 22 in the paper).
     *
     * @param rod_constraints contains the combined zero-stretch, bending and torsion constraints of the rod. The set of
     * constraints must by acyclic.
     * @param rod_segments contains the segments of the rod
     * @param forward list of nodes in the acyclic tree of rod segments and zero-stretch, bending and torsion
     * constraints so that parent nodes occur later in the list than their children
     * @param backward list of nodes in the acyclic tree of rod segments and zero-stretch, bending and torsion
     * constraints. The reverse of the forward list.
     * @param RHS vector with entries for each constraint. In concatenation these entries represent the right hand side
     * of the system of equations to be solved. (eq. 22 in the paper)
     * @param lambda_sums contains entries of the sum of all lambda updates for
     * each constraint in the rod during one time step which is needed by the solver to handle
     * compliance in the correct way.
     * @param bending_and_torsion_jacobians this vector is used to temporary
     * save the Jacobians of the bending and torsion part of each constraint
     * during the solution process of the system of equations. Allocating this
     * vector outside of the solve-method avoids repeated reallocation between iterations of the solver
     * @param corr_x vector of position corrections for every segment of the rod (part of delta-x in eq. 22 in the
     * paper)
     * @param corr_q vector of rotation corrections for every segment of the rod (part of delta-x in eq. 22 in the
     * paper)
     */
    static bool SolveDirectPositionBasedSolverForStiffRodsConstraint(
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
            std::vector<Quaternionr> &corr_q);

    /**
     * Initialize the zero-stretch, bending, and torsion constraint.
     * Computes constraint connectors in segment space, computes the diagonal stiffness matrix
     * and the Darboux vector of the initial state.
     *
     * @param x0 center of mass of body 0
     * @param q0 rotation of body 0
     * @param x1 center of mass of body 1
     * @param q1 rotation of body 1
     * @param constraint_position position of the constraint in world coordinates
     * @param average_radius the average radius of the two rod segments connected by the constraint. Value in Meters (m)
     * @param average_segment_length the average length of the two rod segments connected by the constraint. Value in
     * Meters (m)
     * @param youngs_modulus the Young's modulus of the rod material measures stiffness against bending. Value in
     * Pascal(Pa)
     * @param torsion_modulus the torsion modulus (also called shear modulus) of the rod material measures stiffness
     * against torsion. Value in Pascal (Pa)
     * @param joint_info joint information which is required by the solver.This
     * information is generated in this method
     * and updated each time the bodies change their state by UpdateStretchBendingTwistingConstraint().
     * @param stiffness_coefficient_k diagonal matrix with material parameters for bending and torsion stiffness (eq. 5
     * in the paper)
     * @param rest_darboux_vector the rest Darboux vector computed in this method with the initial constraint
     * configuration
     */
    static bool InitStretchBendingTwistingConstraint(const Vector3r &x0,
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
                                                     Vector3r &rest_darboux_vector);

    /**
     * Initialize the constraint before the projection iterations in each time step.
     *
     * @param stiffness_coefficient_k diagonal matrix with material parameters for bending and torsion stiffness (eq. 5
     * in the paper)
     * @param inverse_time_step_size inverse of the current time step size used to compute compliance (see computation
     * of alpha-tilde in eq. 17)
     * @param bending_and_torsion_compliance the compliance of the bending and torsion constraint part (eq. 24 in the
     * paper)
     * @param stretch_compliance the compliance of the stretch constraint part (eq. 24 in the paper)
     * @param lambda_sum the sum of all lambda updates of
     * this constraint during one time step which is needed by the solver to handle
     * compliance in the correct way. Is set to zero. (see eq. 19 in the paper)
     */
    static bool InitBeforeProjectionStretchBendingTwistingConstraint(const Vector3r &stiffness_coefficient_k,
                                                                     Real inverse_time_step_size,
                                                                     Real average_segment_length,
                                                                     Vector3r &stretch_compliance,
                                                                     Vector3r &bending_and_torsion_compliance,
                                                                     Vector6r &lambda_sum);

    /** Update the joint info data.
     *
     * @param x0 center of mass of body 0
     * @param q0 rotation of body 0
     * @param x1 center of mass of body 1
     * @param q1 rotation of body 1
     * @param joint_info joint information which is required by the solver.This
     * information is updated by calling this method.
     */
    static bool UpdateStretchBendingTwistingConstraint(const Vector3r &x0,
                                                       const Quaternionr &q0,
                                                       const Vector3r &x1,
                                                       const Quaternionr &q1,
                                                       Eigen::Matrix<Real, 3, 4, Eigen::DontAlign> &joint_info);

    /** Determine the position and orientation corrections for the combined zero-stretch, bending and twisting
    /// constraint (eq. 23 in the paper).
    *
    * @param inv_mass_0 inverse mass of first body; inverse mass is zero if body is static
    * @param x0 center of mass of body 0
    * @param inertia_inverse_w_0 inverse inertia tensor (world space) of body 0
    * @param q0 rotation of body 0
    * @param inv_mass_1 inverse mass of second body; inverse mass is zero if body is static
    * @param x1 center of mass of body 1
    * @param inertia_inverse_w_1 inverse inertia tensor (world space) of body 1
    * @param q1 rotation of body 1
    * @param rest_darboux_vector the rest Darboux vector of the initial constraint configuration
    * @param average_segment_length the average length of the two rod segments connected by the constraint
    * @param stretch_compliance the compliance of the stretch constraint part (eq. 24 in the paper)
    * @param bending_and_torsion_compliance the compliance of the bending and torsion constraint part (eq. 24 in the
    // paper)
    * @param joint_info joint information which is required by the solver.This
    * information must be generated in the beginning by calling InitStretchBendingTwistingConstraint()
    * and updated each time the bodies change their state by UpdateStretchBendingTwistingConstraint().
    * @param corr_x0 position correction of center of mass of first body
    * @param corr_q0 rotation correction of first body
    * @param corr_x1 position correction of center of mass of second body
    * @param corr_q1 rotation correction of second body
    * @param lambda_sum the sum of all lambda updates of
    * this constraint during one time step which is needed by the solver to handle
    * compliance in the correct way. Must be set to zero before the position
    * projection iterations start at each time step by calling
    * InitBeforeProjectionStretchBendingTwistingConstraint(). (see eq. 19 in the paper)
    */
    static bool SolveStretchBendingTwistingConstraint(Real inv_mass_0,
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
                                                      Vector6r &lambda_sum);
};

// Implementation of "Position And Orientation Based Cosserat Rods" paper
// (https://animation.rwth-aachen.de/publication/0550/)
//
//	Implemented by:
//
//	Tassilo Kugelstadt
//	Computer Animation Group
//	RWTH Aachen University
//
//  kugelstadt[at] cs.rwth-aachen.de
//
class PositionBasedCosseratRods {
public:
    /**
     * Determine the position and orientation corrections for the stretch and shear constraint constraint (eq. 37 in
     * the paper).
     *
     * @param p0 position of first particle
     * @param inv_mass_0 inverse mass of first particle
     * @param p1 position of second particle
     * @param inv_mass_1 inverse mass of second particle
     * @param q0 Quaternionr at the center of the edge
     * @param inv_mass_q0 inverse mass of the quaternion
     * @param stretching_and_shearing_ks stiffness coefficients for stretching and shearing
     * @param rest_length rest edge length
     * @param corr0 position correction of first particle
     * @param corr1 position correction of second particle
     * @param corr_q0 orientation correction of quaternion
     */
    static bool SolveStretchShearConstraint(const Vector3r &p0,
                                            Real inv_mass_0,
                                            const Vector3r &p1,
                                            Real inv_mass_1,
                                            const Quaternionr &q0,
                                            Real inv_mass_q0,
                                            const Vector3r &stretching_and_shearing_ks,
                                            Real rest_length,
                                            Vector3r &corr0,
                                            Vector3r &corr1,
                                            Quaternionr &corr_q0);

    /**
     * Determine the position corrections for the bending and torsion constraint constraint (eq. 40 in the paper).
     *
     * @param q0 first quaternion
     * @param inv_mass_q0 inverse mass of the first quaternion
     * @param q1 second quaternion
     * @param inv_mass_q1 inverse Mass of the second quaternion
     * @param bending_and_twisting_ks stiffness coefficients for stretching and shearing
     * @param rest_darboux_vector rest Darboux vector
     * @param corr_q0 position correction of first particle
     * @param corr_q1 position correction of second particle
     */
    static bool SolveBendTwistConstraint(const Quaternionr &q0,
                                         Real inv_mass_q0,
                                         const Quaternionr &q1,
                                         Real inv_mass_q1,
                                         const Vector3r &bending_and_twisting_ks,
                                         const Quaternionr &rest_darboux_vector,
                                         Quaternionr &corr_q0,
                                         Quaternionr &corr_q1);
};

// Implementation of "Position Based Elastic Rods" paper
// (http://www.nobuyuki-umetani.com/PositionBasedElasticRod/2014_sca_PositionBasedElasticRod.html)
//
//	The code is based on the implementation of
//
//	Przemyslaw Korzeniowski
//	Department of Surgery and Cancer
//	Imperial College London
//
//	http://github.com/korzen/PositionBasedDynamics-ElasticRod
//  korzenio[at] gmail.com
//
class PositionBasedElasticRods {
public:
    /**
     * Determine the position corrections for a perpendicular bisector constraint:
     * \f$C(\mathbf{p}_0, \mathbf{p}_1, \mathbf{p}_2) = ( \mathbf{p}_1 - 0.5 (\mathbf{p}_0 + \mathbf{p}_1))^T
     * (\mathbf{p}_1 - \mathbf{p}_0) = 0\f$
     *
     * @param p0 position of first particle
     * @param inv_mass_0 inverse mass of first particle
     * @param p1 position of second particle
     * @param inv_mass_1 inverse mass of second particle
     * @param p2 position of third particle
     * @param inv_mass_2 inverse mass of third particle
     * @param stiffness stiffness coefficient
     * @param corr0 position correction of first particle
     * @param corr1 position correction of second particle
     * @param corr2 position correction of third particle
     */
    static bool SolvePerpendiculaBisectorConstraint(const Vector3r &p0,
                                                    Real inv_mass_0,
                                                    const Vector3r &p1,
                                                    Real inv_mass_1,
                                                    const Vector3r &p2,
                                                    Real inv_mass_2,
                                                    Real stiffness,
                                                    Vector3r &corr0,
                                                    Vector3r &corr1,
                                                    Vector3r &corr2);

    /**
     * Determine the position corrections for a constraint that enforces a rest length between an edge and a ghost
     * point: \f$C(\mathbf{p}_0, \mathbf{p}_1, \mathbf{p}_2) = \| ( 0.5 (\mathbf{p}_0 + \mathbf{p}_1) - \mathbf{p}_2
     * \| - L_0 = 0\f$
     *
     * @param p0 position of first particle
     * @param inv_mass_0 inverse mass of first particle
     * @param p1 position of second particle
     * @param inv_mass_1 inverse mass of second particle
     * @param p2 position of third particle
     * @param inv_mass_2 inverse mass of third particle
     * @param stiffness stiffness coefficient
     * @param ghost_edge_rest_length rest length
     * @param corr0 position correction of first particle
     * @param corr1 position correction of second particle
     * @param corr2 position correction of third particle
     */
    static bool SolveGhostPointEdgeDistanceConstraint(const Vector3r &p0,
                                                      Real inv_mass_0,
                                                      const Vector3r &p1,
                                                      Real inv_mass_1,
                                                      const Vector3r &p2,
                                                      Real inv_mass_2,
                                                      Real stiffness,
                                                      Real ghost_edge_rest_length,
                                                      Vector3r &corr0,
                                                      Vector3r &corr1,
                                                      Vector3r &corr2);

    /**
     * Determine the position corrections for the Darboux vector constraint (eq. 21 in the paper). See the paper
     * appendix for derivation details
     *
     * @param p0 position of first particle
     * @param inv_mass_0 inverse mass of first particle
     * @param p1 position of second particle
     * @param inv_mass_1 inverse mass of second particle
     * @param p2 position of third particle
     * @param inv_mass_2 inverse mass of third particle
     * @param p3 position of fourth particle
     * @param inv_mass_3 inverse mass of fourth particle
     * @param p4 position of fifth particle
     * @param inv_mass_4 inverse mass of fifth particle
     * @param bending_and_twisting_ks stiffness coefficients for bending and twisting
     * @param mid_edge_length average edge length
     * @param rest_darboux_vector Darboux vector in rest state
     * @param corr0 position correction of first particle
     * @param corr1 position correction of second particle
     * @param corr2 position correction of third particle
     * @param corr3 position correction of fourth particle
     * @param corr4 position correction of fifth particle
     */
    static bool SolveDarbouxVectorConstraint(const Vector3r &p0,
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
                                             Vector3r &oe);

    /**
     * Computes the material frame (eq. 3 in the paper)
     */
    static bool ComputeMaterialFrame(const Vector3r &p0,  // 1st centreline point id
                                     const Vector3r &p1,  // 2nd centreline point id
                                     const Vector3r &p2,  // corresponding ghost point
                                     Matrix3r &frame);    // resulting material frame

    /**
     * Computes the Darboux Vector (eq. 10 in the paper)
     */
    static bool ComputeDarbouxVector(const Matrix3r &dA,         // 1st material frame
                                     const Matrix3r &dB,         // 2nd material frame
                                     Real mid_edge_length,       //
                                     Vector3r &darboux_vector);  // resulting darboux vector

    /**
     * Computes the material frame derivatives (eq. 43, 44 and 45 in the appendix)
     */
    static bool ComputeMaterialFrameDerivative(const Vector3r &p0,
                                               const Vector3r &p1,
                                               const Vector3r &p2,  // points
                                               const Matrix3r &d,   // corresponding material frame
                                               Matrix3r &d1p0,
                                               Matrix3r &d1p1,
                                               Matrix3r &d1p2,  // resulting matrices
                                               Matrix3r &d2p0,
                                               Matrix3r &d2p1,
                                               Matrix3r &d2p2,  // resulting matrices
                                               Matrix3r &d3p0,
                                               Matrix3r &d3p1,
                                               Matrix3r &d3p2);  // resulting matrices

    /**
     * Compute the Darboux gradient in respect to each point (eq. 49-53 in the appendix)
     */
    static bool ComputeDarbouxGradient(const Vector3r &darboux_vector,  // Darboux vector
                                       Real length,                     // element length
                                       const Matrix3r &dA,              // 1st material frame
                                       const Matrix3r &dB,              // 2nd material frame
                                       const Matrix3r[3][3],
                                       const Matrix3r[3][3],  // material frames derivatives
                                       // const Vector3r& bendAndTwistKs, //bending (x,y) and twisting (z) stiffness
                                       Matrix3r &omega_pa,
                                       Matrix3r &omega_pb,
                                       Matrix3r &omega_pc,
                                       Matrix3r &omega_pd,
                                       Matrix3r &omega_pe);  // resulting matrices
};
}  // namespace vox::force