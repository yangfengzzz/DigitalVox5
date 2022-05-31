//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.force/common.h"

// ------------------------------------------------------------------------------------
namespace vox::force {
class PositionBasedRigidBodyDynamics {
    // -------------- Position Based Rigid Body Dynamics  -----------------------------------------------------
private:
    static void ComputeMatrixK(const Vector3r &connector,
                               Real inv_mass,
                               const Vector3r &x,
                               const Matrix3r &inertia_inverse_w,
                               Matrix3r &k);

    static void ComputeMatrixK(const Vector3r &connector0,
                               const Vector3r &connector1,
                               Real inv_mass,
                               const Vector3r &x,
                               const Matrix3r &inertia_inverse_w,
                               Matrix3r &K);

    /** Compute matrix that is required to transform quaternion in
     * a 3D representation. */
    static void ComputeMatrixG(const Quaternionr &q, Eigen::Matrix<Real, 4, 3, Eigen::DontAlign> &G);
    static void ComputeMatrixQ(const Quaternionr &q, Eigen::Matrix<Real, 4, 4, Eigen::DontAlign> &Q);
    static void ComputeMatrixQHat(const Quaternionr &q, Eigen::Matrix<Real, 4, 4, Eigen::DontAlign> &Q);

public:
    //MARK: -
    /** Initialize ball joint and return info which is required by the solver step.
     *
     * @param x0 center of mass of first body
     * @param q0 rotation of first body
     * @param x1 center of mass of second body
     * @param q1 rotation of second body
     * @param joint_position position of ball joint
     * @param joint_info Stores the local and global positions of the connector points.
     * The first two columns store the local connectors in body 0 and 1, respectively, while
     * the last two columns contain the global connector positions which have to be
     * updated in each simulation step by calling UpdateBallJoint().\n
     * The joint info contains the following columns:\n
     * 0:	connector in body 0 (local)\n
     * 1:	connector in body 1 (local)\n
     * 2:	connector in body 0 (global)\n
     * 3:	connector in body 1 (global)
     */
    static bool InitBallJoint(const Vector3r &x0,              // center of mass of body 0
                              const Quaternionr &q0,           // rotation of body 0
                              const Vector3r &x1,              // center of mass of body 1
                              const Quaternionr &q1,           // rotation of body 1
                              const Vector3r &joint_position,  // position of balljoint
                              Eigen::Matrix<Real, 3, 4, Eigen::DontAlign> &joint_info);

    /** Update ball joint info which is required by the solver step.
     * The ball joint info must be generated in the initialization process of the model
     * by calling the function InitBallJoint().
     * This method should be called once per simulation step before executing the solver.\n\n
     *
     * @param x0 center of mass of first body
     * @param q0 rotation of first body
     * @param x1 center of mass of second body
     * @param q1 rotation of second body
     * @param ball_joint_info ball joint information which should be updated
     */
    static bool UpdateBallJoint(const Vector3r &x0,     // center of mass of body 0
                                const Quaternionr &q0,  // rotation of body 0
                                const Vector3r &x1,     // center of mass of body 1
                                const Quaternionr &q1,  // rotation of body 1
                                Eigen::Matrix<Real, 3, 4, Eigen::DontAlign> &ball_joint_info);

    /** Perform a solver step for a ball joint which links two rigid bodies.
     * A ball joint removes three translational degrees of freedom between the bodies.
     * The ball joint info must be generated in the initialization process of the model
     * by calling the function InitBallJoint() and updated each time the bodies
     * change their state by UpdateBallJoint().\n\n
     * More information can be found in: \cite Deul2014
     *
     * \image html balljoint.jpg "ball joint"
     * \image latex balljoint.jpg "ball joint" width=0.5\textwidth
     *
     * @param inv_mass_0 inverse mass of first body
     * @param x0 center of mass of first body
     * @param inertia_inverse_w_0 inverse inertia tensor in world coordinates of first body
     * @param q0 rotation of first body
     * @param invMass1 inverse mass of second body
     * @param x1 center of mass of second body
     * @param inertia_inverse_w_1 inverse inertia tensor in world coordinates of second body
     * @param q1 rotation of second body
     * @param ball_joint_info Ball joint information which is required by the solver. This
     * information must be generated in the beginning by calling InitBallJoint()
     * and updated each time the bodies change their state by UpdateBallJoint().
     * @param corr_x0 position correction of center of mass of first body
     * @param corr_q0 rotation correction of first body
     * @param corr_x1 position correction of center of mass of second body
     * @param corr_q1 rotation correction of second body
     */
    static bool SolveBallJoint(
            Real inv_mass_0,                      // inverse mass is zero if body is static
            const Vector3r &x0,                   // center of mass of body 0
            const Matrix3r &inertia_inverse_w_0,  // inverse inertia tensor (world space) of body 0
            const Quaternionr &q0,                // rotation of body 0
            Real invMass1,                        // inverse mass is zero if body is static
            const Vector3r &x1,                   // center of mass of body 1
            const Matrix3r &inertia_inverse_w_1,  // inverse inertia tensor (world space) of body 1
            const Quaternionr &q1,                // rotation of body 1
            const Eigen::Matrix<Real, 3, 4, Eigen::DontAlign> &ball_joint_info,  // precomputed ball joint info
            Vector3r &corr_x0,
            Quaternionr &corr_q0,
            Vector3r &corr_x1,
            Quaternionr &corr_q1);

    //MARK: -
    /** Initialize ball-on-line-joint and return information which is required by the solver step.
     *
     * @param x0 center of mass of first body
     * @param q0 rotation of first body
     * @param x1 center of mass of second body
     * @param q1 rotation of second body
     * @param position position of joint
     * @param direction direction of line
     * @param joint_info Stores the local and global positions of the connector points.
     * The first two columns store the local connectors in body 0 and 1, respectively.
     * The next three columns contain a coordinate system for the constraint correction (a
     * full coordinate system where the x-axis is the slider axis)
     * in local coordinates. The last five columns contain the global connector positions
     * and the constraint coordinate system in world space which have to be
     * updated in each simulation step by calling UpdateBallOnLineJoint().\n
     * The joint info contains the following columns:\n
     * 0:	connector in body 0 (local)\n
     * 1:	connector in body 1 (local)\n
     * 2-4:	coordinate system of body 0 (local)\n
     * 5:	connector in body 0 (global)\n
     * 6:	connector in body 1 (global)\n
     * 7-9:	coordinate system of body 0 (global)\n
     */
    static bool InitBallOnLineJoint(const Vector3r &x0,         // center of mass of body 0
                                    const Quaternionr &q0,      // rotation of body 0
                                    const Vector3r &x1,         // center of mass of body 1
                                    const Quaternionr &q1,      // rotation of body 1
                                    const Vector3r &position,   // position of joint
                                    const Vector3r &direction,  // direction of line
                                    Eigen::Matrix<Real, 3, 10, Eigen::DontAlign> &joint_info);

    /** Update ball-on-line-joint information which is required by the solver step.
     * The ball-on-line-joint info must be generated in the initialization process of the model
     * by calling the function InitBallOnLineJoint().
     * This method should be called once per simulation step before executing the solver.\n\n
     *
     * @param x0 center of mass of first body
     * @param q0 rotation of first body
     * @param x1 center of mass of second body
     * @param q1 rotation of second body
     * @param joint_info ball-on-line-joint information which should be updated
     */
    static bool UpdateBallOnLineJoint(const Vector3r &x0,     // center of mass of body 0
                                      const Quaternionr &q0,  // rotation of body 0
                                      const Vector3r &x1,     // center of mass of body 1
                                      const Quaternionr &q1,  // rotation of body 1
                                      Eigen::Matrix<Real, 3, 10, Eigen::DontAlign> &joint_info);

    /** Perform a solver step for a ball-on-line-joint which links two rigid bodies.
     * A ball-on-line-joint removes two translational degrees of freedom between the bodies.
     * The joint info must be generated in the initialization process of the model
     * by calling the function InitBallOnLineJoint() and updated each time the bodies
     * change their state by UpdateBallOnLineJoint().\n\n
     * More information can be found in: \cite Deul2014
     *
     * \image html ballonlinejoint.jpg "ball-on-line joint"
     * \image latex ballonlinejoint.jpg "ball-on-line joint" width=0.5\textwidth
     *
     * @param inv_mass_0 inverse mass of first body
     * @param x0 center of mass of first body
     * @param inertia_inverse_w_0 inverse inertia tensor in world coordinates of first body
     * @param q0 rotation of first body
     * @param inv_mass_1 inverse mass of second body
     * @param x1 center of mass of second body
     * @param inertia_inverse_w_1 inverse inertia tensor in world coordinates of second body
     * @param q1 rotation of second body
     * @param joint_info Ball joint information which is required by the solver. This
     * information must be generated in the beginning by calling InitBallJoint()
     * and updated each time the bodies change their state by UpdateBallJoint().
     * @param corr_x0 position correction of center of mass of first body
     * @param corr_q0 rotation correction of first body
     * @param corr_x1 position correction of center of mass of second body
     * @param corr_q1 rotation correction of second body
     */
    static bool SolveBallOnLineJoint(
            Real inv_mass_0,                      // inverse mass is zero if body is static
            const Vector3r &x0,                   // center of mass of body 0
            const Matrix3r &inertia_inverse_w_0,  // inverse inertia tensor (world space) of body 0
            const Quaternionr &q0,                // rotation of body 0
            Real inv_mass_1,                      // inverse mass is zero if body is static
            const Vector3r &x1,                   // center of mass of body 1
            const Matrix3r &inertia_inverse_w_1,  // inverse inertia tensor (world space) of body 1
            const Quaternionr &q1,                // rotation of body 1
            const Eigen::Matrix<Real, 3, 10, Eigen::DontAlign> &joint_info,  // precomputed joint info
            Vector3r &corr_x0,
            Quaternionr &corr_q0,
            Vector3r &corr_x1,
            Quaternionr &corr_q1);

    //MARK: -
    /** Initialize hinge joint and return info which is required by the solver step.
     *
     * @param x0 center of mass of first body
     * @param q0 rotation of first body
     * @param x1 center of mass of second body
     * @param q1 rotation of second body
     * @param hinge_joint_position position of hinge joint
     * @param hinge_joint_axis axis of hinge joint
     * @param hinge_joint_info Stores the local and global positions of the connector points.
     * The joint info contains the following columns:\n
     * 0-1:	projection matrix Pr for the rotational part\n
     * 2:	connector in body 0 (local)\n
     * 3:	connector in body 1 (local)\n
     * 4:	connector in body 0 (global)\n
     * 5:	connector in body 1 (global)\n
     * 6:	hinge axis in body 0 (local) used for rendering\n\n
     * The joint info stores first the info of the first body (the connector point and a
     * full coordinate system where the x-axis is the hinge axis) and then the info of
     * the second body (the connector point and the hinge axis).
     * The info must be updated in each simulation step
     * by calling UpdateHingeJoint().
     */
    static bool InitHingeJoint(const Vector3r &x0,                    // center of mass of body 0
                               const Quaternionr &q0,                 // rotation of body 0
                               const Vector3r &x1,                    // center of mass of body 1
                               const Quaternionr &q1,                 // rotation of body 1
                               const Vector3r &hinge_joint_position,  // position of hinge joint
                               const Vector3r &hinge_joint_axis,      // axis of hinge joint
                               Eigen::Matrix<Real, 4, 7, Eigen::DontAlign> &hinge_joint_info);

    /** Update hinge joint info which is required by the solver step.
     * The joint info must be generated in the initialization process of the model
     * by calling the function InitHingeJoint().
     * This method should be called once per simulation step before executing the solver.\n\n
     *
     * @param x0 center of mass of first body
     * @param q0 rotation of first body
     * @param x1 center of mass of second body
     * @param q1 rotation of second body
     * @param hinge_joint_info hinge joint information which should be updated
     */
    static bool UpdateHingeJoint(const Vector3r &x0,     // center of mass of body 0
                                 const Quaternionr &q0,  // rotation of body 0
                                 const Vector3r &x1,     // center of mass of body 1
                                 const Quaternionr &q1,  // rotation of body 1
                                 Eigen::Matrix<Real, 4, 7, Eigen::DontAlign> &hinge_joint_info);

    /** Perform a solver step for a hinge joint which links two rigid bodies.
     * A hinge joint removes three translational and two rotational  degrees of freedom between the bodies.
     * The hinge joint info must be generated in the initialization process of the model
     * by calling the function InitHingeJoint() and updated each time the bodies
     * change their state by UpdateHingeJoint().\n\n
     * More information can be found in: \cite Deul2014
     *
     * \image html hingejoint.jpg "hinge joint"
     * \image latex hingejoint.jpg "hinge joint" width=0.5\textwidth
     *
     * @param inv_mass_0 inverse mass of first body
     * @param x0 center of mass of first body
     * @param inertia_inverse_w_0 inverse inertia tensor in world coordinates of first body
     * @param q0 rotation of first body
     * @param inv_mass_1 inverse mass of second body
     * @param x1 center of mass of second body
     * @param inertia_inverse_w_1 inverse inertia tensor in world coordinates of second body
     * @param q1 rotation of second body
     * @param hinge_joint_info Hinge joint information which is required by the solver. This
     * information must be generated in the beginning by calling InitHingeJoint()
     * and updated each time the bodies change their state by UpdateHingeJoint().
     * @param corr_x0 position correction of center of mass of first body
     * @param corr_q0 rotation correction of first body
     * @param corr_x1 position correction of center of mass of second body
     * @param corr_q1 rotation correction of second body
     */
    static bool SolveHingeJoint(
            Real inv_mass_0,                      // inverse  mass is zero if body is static
            const Vector3r &x0,                   // center of mass of body 0
            const Matrix3r &inertia_inverse_w_0,  // inverse inertia tensor (world space) of body 0
            const Quaternionr &q0,                // rotation of body 0
            Real inv_mass_1,                      // inverse mass is zero if body is static
            const Vector3r &x1,                   // center of mass of body 1
            const Matrix3r &inertia_inverse_w_1,  // inverse inertia tensor (world space) of body 1
            const Quaternionr &q1,                // rotation of body 1
            const Eigen::Matrix<Real, 4, 7, Eigen::DontAlign> &hinge_joint_info,  // precomputed hinge joint info
            Vector3r &corr_x0,
            Quaternionr &corr_q0,
            Vector3r &corr_x1,
            Quaternionr &corr_q1);

    //MARK: -
    /** Initialize universal joint and return info which is required by the solver step.
     *
     * @param x0 center of mass of first body
     * @param q0 rotation of first body
     * @param x1 center of mass of second body
     * @param q1 rotation of second body
     * @param joint_position position of universal joint
     * @param joint_axis_0 first axis of universal joint
     * @param joint_axis_1 second axis of universal joint
     * @param joint_info Stores the local and global positions of the connector points.
     * The joint info contains the following columns:\n
     * 0:	connector in body 0 (local)\n
     * 1:	connector in body 1 (local)\n
     * 2:	constraint axis 0 in body 0 (local)\n
     * 3:	constraint axis 1 in body 1 (local)\n
     * 4:	connector in body 0 (global)\n
     * 5:	connector in body 1 (global)\n
     * 6:	constraint axis 0 in body 0 (global)\n
     * 7:	constraint axis 1 in body 1 (global)\n\n
     * The info must be updated in each simulation step
     * by calling UpdateUniversalJoint().
     */
    static bool InitUniversalJoint(const Vector3r &x0,              // center of mass of body 0
                                   const Quaternionr &q0,           // rotation of body 0
                                   const Vector3r &x1,              // center of mass of body 1
                                   const Quaternionr &q1,           // rotation of body 1
                                   const Vector3r &joint_position,  // position of universal joint
                                   const Vector3r &joint_axis_0,    // first axis of universal joint
                                   const Vector3r &joint_axis_1,    // second axis of universal joint
                                   Eigen::Matrix<Real, 3, 8, Eigen::DontAlign> &joint_info);

    /** Update universal joint info which is required by the solver step.
     * The joint info must be generated in the initializatgion process of the model
     * by calling the function InitUniversalJoint().
     * This method should be called once per simulation step before executing the solver.\n\n
     *
     * @param x0 center of mass of first body
     * @param q0 rotation of first body
     * @param x1 center of mass of second body
     * @param q1 rotation of second body
     * @param joint_info universal joint information which should be updated
     */
    static bool UpdateUniversalJoint(const Vector3r &x0,     // center of mass of body 0
                                     const Quaternionr &q0,  // rotation of body 0
                                     const Vector3r &x1,     // center of mass of body 1
                                     const Quaternionr &q1,  // rotation of body 1
                                     Eigen::Matrix<Real, 3, 8, Eigen::DontAlign> &joint_info);

    /** Perform a solver step for a universal joint which links two rigid bodies.
     * A universal joint removes three translational and one rotational degree of freedom between the bodies.
     * The universal joint info must be generated in the initialization process of the model
     * by calling the function InitUniversalJoint() and updated each time the bodies
     * change their state by UpdateUniversalJoint().\n\n
     * More information can be found in: \cite Deul2014
     *
     * \image html universaljoint.jpg "universal joint"
     * \image latex universaljoint.jpg "universal joint" width=0.5\textwidth
     *
     * @param inv_mass_0 inverse mass of first body
     * @param x0 center of mass of first body
     * @param inertia_inverse_w_0 inverse inertia tensor in world coordinates of first body
     * @param q0 rotation of first body
     * @param inv_mass_1 inverse mass of second body
     * @param x1 center of mass of second body
     * @param inertia_inverse_w_1 inverse inertia tensor in world coordinates of second body
     * @param q1 rotation of second body
     * @param joint_info Universal joint information which is required by the solver. This
     * information must be generated in the beginning by calling InitUniversalJoint()
     * and updated each time the bodies change their state by UpdateUniversalJoint().
     * @param corr_x0 position correction of center of mass of first body
     * @param corr_q0 rotation correction of first body
     * @param corr_x1 position correction of center of mass of second body
     * @param corr_q1 rotation correction of second body
     */
    static bool SolveUniversalJoint(
            Real inv_mass_0,                      // inverse mass is zero if body is static
            const Vector3r &x0,                   // center of mass of body 0
            const Matrix3r &inertia_inverse_w_0,  // inverse inertia tensor (world space) of body 0
            const Quaternionr &q0,                // rotation of body 0
            Real inv_mass_1,                      // inverse mass is zero if body is static
            const Vector3r &x1,                   // center of mass of body 1
            const Matrix3r &inertia_inverse_w_1,  // inverse inertia tensor (world space) of body 1
            const Quaternionr &q1,                // rotation of body 1
            const Eigen::Matrix<Real, 3, 8, Eigen::DontAlign> &joint_info,  // precomputed universal joint info
            Vector3r &corr_x0,
            Quaternionr &corr_q0,
            Vector3r &corr_x1,
            Quaternionr &corr_q1);

    //MARK: -
    /** Initialize slider joint and return info which is required by the solver step.
     *
     * @param x0 center of mass of first body
     * @param q0 rotation of first body
     * @param x1 center of mass of second body
     * @param q1 rotation of second body
     * @param slider_joint_axis axis of slider joint
     * @param joint_info Stores the local and global positions of the connector points.
     * The joint info contains the following columns:\n
     * jointInfo contains\n
     * 0:   coordinate system in body 0, where the x-axis is the slider axis (local)\n
     * 1:   coordinate system in body 0, where the x-axis is the slider axis (global)\n
     * 2:   2D vector d = P * (x0 - x1), where P projects the vector onto a plane perpendicular to the slider axis\n
     * 3-5: projection matrix Pr for the rotational part\n\n
     * The info must be updated in each simulation step
     * by calling UpdateSliderJoint().
     */
    static bool InitSliderJoint(const Vector3r &x0,                 // center of mass of body 0
                                const Quaternionr &q0,              // rotation of body 0
                                const Vector3r &x1,                 // center of mass of body 1
                                const Quaternionr &q1,              // rotation of body 1
                                const Vector3r &slider_joint_axis,  // axis of slider joint
                                Eigen::Matrix<Real, 4, 6, Eigen::DontAlign> &joint_info);

    /** Update slider joint info which is required by the solver step.
     * The joint info must be generated in the initialization process of the model
     * by calling the function InitSliderJoint().
     * This method should be called once per simulation step before executing the solver.\n\n
     *
     * @param x0 center of mass of first body
     * @param q0 rotation of first body
     * @param x1 center of mass of second body
     * @param q1 rotation of second body
     * @param joint_info slider joint information which should be updated
     */
    static bool UpdateSliderJoint(const Vector3r &x0,     // center of mass of body 0
                                  const Quaternionr &q0,  // rotation of body 0
                                  const Vector3r &x1,     // center of mass of body 1
                                  const Quaternionr &q1,  // rotation of body 1
                                  Eigen::Matrix<Real, 4, 6, Eigen::DontAlign> &joint_info);

    /** Perform a solver step for a slider joint which links two rigid bodies.
     * A slider joint removes two translational and three rotational degrees of freedom between the bodies.
     * The slider joint info must be generated in the initialization process of the model
     * by calling the function InitSliderJoint() and updated each time the bodies
     * change their state by UpdateSliderJoint().\n\n
     * More information can be found in: \cite Deul2014
     *
     * \image html sliderjoint.jpg "slider joint"
     * \image latex sliderjoint.jpg "slider joint" width=0.5\textwidth
     *
     * @param inv_mass_0 inverse mass of first body
     * @param x0 center of mass of first body
     * @param inertia_inverse_w_0 inverse inertia tensor in world coordinates of first body
     * @param q0 rotation of first body
     * @param inv_mass_1 inverse mass of second body
     * @param x1 center of mass of second body
     * @param inertia_inverse_w_1 inverse inertia tensor in world coordinates of second body
     * @param q1 rotation of second body
     * @param joint_info Slider joint information which is required by the solver. This
     * information must be generated in the beginning by calling InitSliderJoint()
     * and updated each time the bodies change their state by UpdateSliderJoint().
     * @param corr_x0 position correction of center of mass of first body
     * @param corr_q0 rotation correction of first body
     * @param corr_x1 position correction of center of mass of second body
     * @param corr_q1 rotation correction of second body
     */
    static bool SolveSliderJoint(
            Real inv_mass_0,                      // inverse  mass is zero if body is static
            const Vector3r &x0,                   // center of mass of body 0
            const Matrix3r &inertia_inverse_w_0,  // inverse inertia tensor (world space) of body 0
            const Quaternionr &q0,                // rotation of body 0
            Real inv_mass_1,                      // inverse mass is zero if body is static
            const Vector3r &x1,                   // center of mass of body 1
            const Matrix3r &inertia_inverse_w_1,  // inverse inertia tensor (world space) of body 1
            const Quaternionr &q1,                // rotation of body 1
            const Eigen::Matrix<Real, 4, 6, Eigen::DontAlign> &joint_info,  // precomputed slider joint info
            Vector3r &corr_x0,
            Quaternionr &corr_q0,
            Vector3r &corr_x1,
            Quaternionr &corr_q1);

    //MARK: -
    /** Initialize a motor slider joint which is able to enforce
     * a target position and return info which is required by the solver step.
     *
     * @param x0 center of mass of first body
     * @param q0 rotation of first body
     * @param x1 center of mass of second body
     * @param q1 rotation of second body
     * @param slider_joint_axis axis of slider joint
     * @param joint_info Stores the local and global positions of the connector points.
     * The joint info contains the following columns:\n
     * 0:	slider axis in body 0 (local)\n
     * 1:	slider axis in body 0 (global)\n
     * 2:   distance vector d = (x0 - x1)\n
     * 3-5:	projection matrix Pr for the rotational part\n\n
     * The info must be updated in each simulation step
     * by calling UpdateTargetPositionMotorSliderJoint().
     */
    static bool InitTargetPositionMotorSliderJoint(const Vector3r &x0,                 // center of mass of body 0
                                                   const Quaternionr &q0,              // rotation of body 0
                                                   const Vector3r &x1,                 // center of mass of body 1
                                                   const Quaternionr &q1,              // rotation of body 1
                                                   const Vector3r &slider_joint_axis,  // axis of slider joint
                                                   Eigen::Matrix<Real, 4, 6, Eigen::DontAlign> &joint_info);

    /** Update motor slider joint info which is required by the solver step.
     * The joint info must be generated in the initialization process of the model
     * by calling the function InitTargetPositionMotorSliderJoint().
     * This method should be called once per simulation step before executing the solver.\n\n
     *
     * @param x0 center of mass of first body
     * @param q0 rotation of first body
     * @param x1 center of mass of second body
     * @param q1 rotation of second body
     * @param joint_info slider joint information which should be updated
     */
    static bool UpdateTargetPositionMotorSliderJoint(const Vector3r &x0,     // center of mass of body 0
                                                     const Quaternionr &q0,  // rotation of body 0
                                                     const Vector3r &x1,     // center of mass of body 1
                                                     const Quaternionr &q1,  // rotation of body 1
                                                     Eigen::Matrix<Real, 4, 6, Eigen::DontAlign> &joint_info);

    /** Perform a solver step for a motor slider joint which links two rigid bodies.
     * A motor slider joint removes two translational and three rotational degrees of freedom between the bodies.
     * Moreover, a target position can be enforced on the remaining translation axis.
     * The motor slider joint info must be generated in the initialization process of the model
     * by calling the function InitTargetPositionMotorSliderJoint() and updated each time the bodies
     * change their state by UpdateTargetPositionMotorSliderJoint().\n\n
     * More information can be found in: \cite Deul2014
     *
     * \image html motorsliderjoint.jpg "motor slider joint"
     * \image latex motorsliderjoint.jpg "motor slider joint" width=0.5\textwidth
     *
     * @param inv_mass_0 inverse mass of first body
     * @param x0 center of mass of first body
     * @param inertia_inverse_w_0 inverse inertia tensor in world coordinates of first body
     * @param q0 rotation of first body
     * @param inv_mass_1 inverse mass of second body
     * @param x1 center of mass of second body
     * @param inertia_inverse_w_1 inverse inertia tensor in world coordinates of second body
     * @param q1 rotation of second body
     * @param target_position target position of the servo motor
     * @param joint_info Motor slider joint information which is required by the solver. This
     * information must be generated in the beginning by calling InitTargetPositionMotorSliderJoint()
     * and updated each time the bodies change their state by UpdateTargetPositionMotorSliderJoint().
     * @param corr_x0 position correction of center of mass of first body
     * @param corr_q0 rotation correction of first body
     * @param corr_x1 position correction of center of mass of second body
     * @param corr_q1 rotation correction of second body
     */
    static bool SolveTargetPositionMotorSliderJoint(
            Real inv_mass_0,                      // inverse  mass is zero if body is static
            const Vector3r &x0,                   // center of mass of body 0
            const Matrix3r &inertia_inverse_w_0,  // inverse inertia tensor (world space) of body 0
            const Quaternionr &q0,                // rotation of body 0
            Real inv_mass_1,                      // inverse mass is zero if body is static
            const Vector3r &x1,                   // center of mass of body 1
            const Matrix3r &inertia_inverse_w_1,  // inverse inertia tensor (world space) of body 1
            const Quaternionr &q1,                // rotation of body 1
            Real target_position,                 // target position of the servo motor
            const Eigen::Matrix<Real, 4, 6, Eigen::DontAlign> &joint_info,  // precomputed slider joint info
            Vector3r &corr_x0,
            Quaternionr &corr_q0,
            Vector3r &corr_x1,
            Quaternionr &corr_q1);

    //MARK: -
    /** Initialize a motor slider joint which is able to enforce
     * a target velocity and return info which is required by the solver step.
     *
     * @param x0 center of mass of first body
     * @param q0 rotation of first body
     * @param x1 center of mass of second body
     * @param q1 rotation of second body
     * @param slider_joint_axis axis of slider joint
     * @param joint_info Stores the local and global positions of the connector points.
     * The joint info contains the following columns:\n
     * 0:   coordinate system in body 0, where the x-axis is the slider axis (local)\n
     * 1:   coordinate system in body 0, where the x-axis is the slider axis (global)\n
     * 2:   2D vector d = P * (x0 - x1), where P projects the vector onto a plane perpendicular to the slider axis\n
     * 3-5: projection matrix Pr for the rotational part\n\n
     * The info must be updated in each simulation step
     * by calling UpdateTargetVelocityMotorSliderJoint().
     */
    static bool InitTargetVelocityMotorSliderJoint(const Vector3r &x0,                 // center of mass of body 0
                                                   const Quaternionr &q0,              // rotation of body 0
                                                   const Vector3r &x1,                 // center of mass of body 1
                                                   const Quaternionr &q1,              // rotation of body 1
                                                   const Vector3r &slider_joint_axis,  // axis of slider joint
                                                   Eigen::Matrix<Real, 4, 6, Eigen::DontAlign> &joint_info);

    /** Update motor slider joint info which is required by the solver step.
     * The joint info must be generated in the initialization process of the model
     * by calling the function InitTargetVelocityMotorSliderJoint().
     * This method should be called once per simulation step before executing the solver.\n\n
     *
     * @param x0 center of mass of first body
     * @param q0 rotation of first body
     * @param x1 center of mass of second body
     * @param q1 rotation of second body
     * @param joint_info slider joint information which should be updated
     */
    static bool UpdateTargetVelocityMotorSliderJoint(const Vector3r &x0,     // center of mass of body 0
                                                     const Quaternionr &q0,  // rotation of body 0
                                                     const Vector3r &x1,     // center of mass of body 1
                                                     const Quaternionr &q1,  // rotation of body 1
                                                     Eigen::Matrix<Real, 4, 6, Eigen::DontAlign> &joint_info);

    /** Perform a solver step for a motor slider joint which links two rigid bodies.
     * A motor slider joint removes two translational and three rotational degrees of freedom between the bodies.
     * Moreover, a target velocity can be enforced on the remaining translation axis.
     * The motor slider joint info must be generated in the initialization process of the model
     * by calling the function InitTargetVelocityMotorSliderJoint() and updated each time the bodies
     * change their state by UpdateTargetVelocityMotorSliderJoint().\n\n
     * More information can be found in: \cite Deul2014
     *
     * \image html motorsliderjoint.jpg "motor slider joint"
     * \image latex motorsliderjoint.jpg "motor slider joint" width=0.5\textwidth
     *
     * @param inv_mass_0 inverse mass of first body
     * @param x0 center of mass of first body
     * @param inertia_inverse_w_0 inverse inertia tensor in world coordinates of first body
     * @param q0 rotation of first body
     * @param inv_mass_1 inverse mass of second body
     * @param x1 center of mass of second body
     * @param inertia_inverse_w_1 inverse inertia tensor in world coordinates of second body
     * @param q1 rotation of second body
     * @param joint_info Motor slider joint information which is required by the solver. This
     * information must be generated in the beginning by calling InitTargetVelocityMotorSliderJoint()
     * and updated each time the bodies change their state by UpdateTargetVelocityMotorSliderJoint().
     * @param corr_x0 position correction of center of mass of first body
     * @param corr_q0 rotation correction of first body
     * @param corr_x1 position correction of center of mass of second body
     * @param corr_q1 rotation correction of second body
     */
    static bool SolveTargetVelocityMotorSliderJoint(
            Real inv_mass_0,                      // inverse  mass is zero if body is static
            const Vector3r &x0,                   // center of mass of body 0
            const Matrix3r &inertia_inverse_w_0,  // inverse inertia tensor (world space) of body 0
            const Quaternionr &q0,                // rotation of body 0
            Real inv_mass_1,                      // inverse mass is zero if body is static
            const Vector3r &x1,                   // center of mass of body 1
            const Matrix3r &inertia_inverse_w_1,  // inverse inertia tensor (world space) of body 1
            const Quaternionr &q1,                // rotation of body 1
            const Eigen::Matrix<Real, 4, 6, Eigen::DontAlign> &joint_info,  // precomputed slider joint info
            Vector3r &corr_x0,
            Quaternionr &corr_q0,
            Vector3r &corr_x1,
            Quaternionr &corr_q1);

    /** Perform a velocity solver step for a motor slider joint which links two rigid bodies.
     * A motor slider joint removes two translational and three rotational degrees of freedom between the bodies.
     * Moreover, a target velocity can be enforced on the remaining translation axis.
     * The motor slider joint info must be generated in the initialization process of the model
     * by calling the function InitTargetVelocityMotorSliderJoint() and updated each time the bodies
     * change their state by UpdateTargetVelocityMotorSliderJoint().\n\n
     * More information can be found in: \cite Deul2014
     *
     * \image html motorsliderjoint.jpg "motor slider joint"
     * \image latex motorsliderjoint.jpg "motor slider joint" width=0.5\textwidth
     *
     * @param inv_mass_0 inverse mass of first body
     * @param x0 center of mass of first body
     * @param v0 velocity of body 0
     * @param inertia_inverse_w_0 inverse inertia tensor in world coordinates of first body
     * @param q0 rotation of first body
     * @param omega_0 angular velocity of first body
     * @param inv_mass_1 inverse mass of second body
     * @param x1 center of mass of second body
     * @param v1 velocity of body 1
     * @param inertia_inverse_w_1 inverse inertia tensor in world coordinates of second body
     * @param q1 rotation of second body
     * @param omega_1 angular velocity of second body
     * @param target_velocity target velocity of the motor
     * @param joint_info Motor slider joint information which is required by the solver. This
     * information must be generated in the beginning by calling InitTargetVelocityMotorSliderJoint()
     * and updated each time the bodies change their state by UpdateTargetVelocityMotorSliderJoint().
     * @param corr_v0 velocity correction of first body
     * @param corr_omega0 angular velocity correction of first body
     * @param corr_v1 velocity correction of second body
     * @param corr_omega1 angular velocity correction of second body
     */
    static bool VelocitySolveTargetVelocityMotorSliderJoint(
            Real inv_mass_0,                      // inverse  mass is zero if body is static
            const Vector3r &x0,                   // center of mass of body 0
            const Vector3r &v0,                   // velocity of body 0
            const Matrix3r &inertia_inverse_w_0,  // inverse inertia tensor (world space) of body 0
            const Quaternionr &q0,                // rotation of body 0
            const Vector3r &omega_0,
            Real inv_mass_1,     // inverse mass is zero if body is static
            const Vector3r &x1,  // center of mass of body 1
            const Vector3r &v1,
            const Matrix3r &inertia_inverse_w_1,  // inverse inertia tensor (world space) of body 1
            const Quaternionr &q1,                // rotation of body 1
            const Vector3r &omega_1,
            Real target_velocity,                                           // target velocity of the servo motor
            const Eigen::Matrix<Real, 4, 6, Eigen::DontAlign> &joint_info,  // precomputed joint info
            Vector3r &corr_v0,
            Vector3r &corr_omega0,
            Vector3r &corr_v1,
            Vector3r &corr_omega1);

    //MARK: -
    /** Initialize a motor hinge joint which is able to enforce
     * a target angle and return info which is required by the solver step.
     *
     * @param x0 center of mass of first body
     * @param q0 rotation of first body
     * @param x1 center of mass of second body
     * @param q1 rotation of second body
     * @param hinge_joint_position position of hinge joint
     * @param hinge_joint_axis axis of hinge joint
     * @param joint_info Stores the local and global positions of the connector points.
     * The joint info contains the following columns:\n
     * 0-2:	projection matrix Pr for the rotational part\n
     * 3:	connector in body 0 (local)\n
     * 4:	connector in body 1 (local)\n
     * 5:	connector in body 0 (global)\n
     * 6:	connector in body 1 (global)\n
     * 7:	hinge axis in body 0 (local) used for rendering \n\n
     * The info must be updated in each simulation step
     * by calling UpdateTargetAngleMotorHingeJoint().
     */
    static bool InitTargetAngleMotorHingeJoint(const Vector3r &x0,                    // center of mass of body 0
                                               const Quaternionr &q0,                 // rotation of body 0
                                               const Vector3r &x1,                    // center of mass of body 1
                                               const Quaternionr &q1,                 // rotation of body 1
                                               const Vector3r &hinge_joint_position,  // position of hinge joint
                                               const Vector3r &hinge_joint_axis,      // axis of hinge joint
                                               Eigen::Matrix<Real, 4, 8, Eigen::DontAlign> &joint_info);

    /** Update motor hinge joint info which is required by the solver step.
     * The joint info must be generated in the initialization process of the model
     * by calling the function InitTargetAngleMotorHingeJoint().
     * This method should be called once per simulation step before executing the solver.\n\n
     *
     * @param x0 center of mass of first body
     * @param q0 rotation of first body
     * @param x1 center of mass of second body
     * @param q1 rotation of second body
     * @param joint_info motor hinge joint information which should be updated
     */
    static bool UpdateTargetAngleMotorHingeJoint(const Vector3r &x0,     // center of mass of body 0
                                                 const Quaternionr &q0,  // rotation of body 0
                                                 const Vector3r &x1,     // center of mass of body 1
                                                 const Quaternionr &q1,  // rotation of body 1
                                                 Eigen::Matrix<Real, 4, 8, Eigen::DontAlign> &joint_info);

    /** Perform a solver step for a motor hinge joint which links two rigid bodies.
     * A motor hinge joint removes three translational and two rotational degrees of freedom between the bodies.
     * Moreover, a target angle can be enforced on the remaining rotation axis.
     * The motor hinge joint info must be generated in the initialization process of the model
     * by calling the function InitTargetAngleMotorHingeJoint() and updated each time the bodies
     * change their state by UpdateTargetAngleMotorHingeJoint().\n\n
     * More information can be found in: \cite Deul2014
     *
     * \image html motorhingejoint.jpg "motor hinge joint"
     * \image latex motorhingejoint.jpg "motor hinge joint" width=0.5\textwidth
     *
     * @param inv_mass_0 inverse mass of first body
     * @param x0 center of mass of first body
     * @param inertia_inverse_w_0 inverse inertia tensor in world coordinates of first body
     * @param q0 rotation of first body
     * @param inv_mass_1 inverse mass of second body
     * @param x1 center of mass of second body
     * @param inertia_inverse_w_1 inverse inertia tensor in world coordinates of second body
     * @param q1 rotation of second body
     * @param target_angle target angle of the servo motor
     * @param joint_info Motor hinge joint information which is required by the solver. This
     * information must be generated in the beginning by calling InitTargetAngleMotorHingeJoint()
     * and updated each time the bodies change their state by UpdateTargetAngleMotorHingeJoint().
     * @param corr_x0 position correction of center of mass of first body
     * @param corr_q0 rotation correction of first body
     * @param corr_x1 position correction of center of mass of second body
     * @param corr_q1 rotation correction of second body
     */
    static bool SolveTargetAngleMotorHingeJoint(
            Real inv_mass_0,                      // inverse  mass is zero if body is static
            const Vector3r &x0,                   // center of mass of body 0
            const Matrix3r &inertia_inverse_w_0,  // inverse inertia tensor (world space) of body 0
            const Quaternionr &q0,                // rotation of body 0
            Real inv_mass_1,                      // inverse mass is zero if body is static
            const Vector3r &x1,                   // center of mass of body 1
            const Matrix3r &inertia_inverse_w_1,  // inverse inertia tensor (world space) of body 1
            const Quaternionr &q1,                // rotation of body 1
            Real target_angle,                    // target angle of the servo motor
            const Eigen::Matrix<Real, 4, 8, Eigen::DontAlign> &joint_info,  // precomputed hinge joint info
            Vector3r &corr_x0,
            Quaternionr &corr_q0,
            Vector3r &corr_x1,
            Quaternionr &corr_q1);

    //MARK: -
    /** Initialize a motor hinge joint which is able to enforce
     * a target velocity and return info which is required by the solver step.
     *
     * @param x0 center of mass of first body
     * @param q0 rotation of first body
     * @param x1 center of mass of second body
     * @param q1 rotation of second body
     * @param hinge_joint_position position of hinge joint
     * @param hinge_joint_axis axis of hinge joint
     * @param joint_info Stores the local and global positions of the connector points.
     * The joint info contains the following columns:\n
     * 0-1:	projection matrix Pr for the rotational part\n
     * 2:	connector in body 0 (local)\n
     * 3:	connector in body 1 (local)\n
     * 4:	connector in body 0 (global)\n
     * 5:	connector in body 1 (global)\n
     * 6:	hinge axis in body 0 (local)\n
     * 7:   hinge axis in body 0 (global)\n\n
     * The info must be updated in each simulation step
     * by calling UpdateTargetVelocityMotorHingeJoint().
     */
    static bool InitTargetVelocityMotorHingeJoint(const Vector3r &x0,                    // center of mass of body 0
                                                  const Quaternionr &q0,                 // rotation of body 0
                                                  const Vector3r &x1,                    // center of mass of body 1
                                                  const Quaternionr &q1,                 // rotation of body 1
                                                  const Vector3r &hinge_joint_position,  // position of hinge joint
                                                  const Vector3r &hinge_joint_axis,      // axis of hinge joint
                                                  Eigen::Matrix<Real, 4, 8, Eigen::DontAlign> &joint_info);

    /** Update motor hinge joint info which is required by the solver step.
     * The joint info must be generated in the initialization process of the model
     * by calling the function InitTargetVelocityMotorHingeJoint().
     * This method should be called once per simulation step before executing the solver.\n\n
     *
     * @param x0 center of mass of first body
     * @param q0 rotation of first body
     * @param x1 center of mass of second body
     * @param q1 rotation of second body
     * @param joint_info motor hinge joint information which should be updated
     */
    static bool UpdateTargetVelocityMotorHingeJoint(const Vector3r &x0,     // center of mass of body 0
                                                    const Quaternionr &q0,  // rotation of body 0
                                                    const Vector3r &x1,     // center of mass of body 1
                                                    const Quaternionr &q1,  // rotation of body 1
                                                    Eigen::Matrix<Real, 4, 8, Eigen::DontAlign> &joint_info);

    /** Perform a solver step for a motor hinge joint which links two rigid bodies.
     * A motor hinge joint removes three translational and two rotational degrees of freedom between the bodies.
     * Moreover, a target velocity can be enforced on the remaining rotation axis by
     * calling the function VelocitySolveTargetVelocityMotorHingeJoint().
     * The motor hinge joint info must be generated in the initialization process of the model
     * by calling the function InitTargetVelocityMotorHingeJoint() and updated each time the bodies
     * change their state by UpdateTargetVelocityMotorHingeJoint().\n\n
     * More information can be found in: \cite Deul2014
     *
     * \image html motorhingejoint.jpg "motor hinge joint"
     * \image latex motorhingejoint.jpg "motor hinge joint" width=0.5\textwidth
     *
     * @param inv_mass_0 inverse mass of first body
     * @param x0 center of mass of first body
     * @param inertia_inverse_w_0 inverse inertia tensor in world coordinates of first body
     * @param q0 rotation of first body
     * @param inv_mass_1 inverse mass of second body
     * @param x1 center of mass of second body
     * @param inertia_inverse_w_1 inverse inertia tensor in world coordinates of second body
     * @param q1 rotation of second body
     * @param joint_info Motor hinge joint information which is required by the solver. This
     * information must be generated in the beginning by calling InitTargetVelocityMotorHingeJoint()
     * and updated each time the bodies change their state by UpdateTargetVelocityMotorHingeJoint().
     * @param corr_x0 position correction of center of mass of first body
     * @param corr_q0 rotation correction of first body
     * @param corr_x1 position correction of center of mass of second body
     * @param corr_q1 rotation correction of second body
     */
    static bool SolveTargetVelocityMotorHingeJoint(
            Real inv_mass_0,                      // inverse  mass is zero if body is static
            const Vector3r &x0,                   // center of mass of body 0
            const Matrix3r &inertia_inverse_w_0,  // inverse inertia tensor (world space) of body 0
            const Quaternionr &q0,                // rotation of body 0
            Real inv_mass_1,                      // inverse mass is zero if body is static
            const Vector3r &x1,                   // center of mass of body 1
            const Matrix3r &inertia_inverse_w_1,  // inverse inertia tensor (world space) of body 1
            const Quaternionr &q1,                // rotation of body 1
            const Eigen::Matrix<Real, 4, 8, Eigen::DontAlign> &joint_info,  // precomputed hinge joint info
            Vector3r &corr_x0,
            Quaternionr &corr_q0,
            Vector3r &corr_x1,
            Quaternionr &corr_q1);

    /** Perform a velocity solver step for a motor hinge joint which links two rigid bodies.
     * A motor hinge joint removes three translational and two rotational degrees of freedom between the bodies.
     * Moreover, a target velocity can be enforced on the remaining rotation axis.
     * The motor hinge joint info must be generated in the initialization process of the model
     * by calling the function InitTargetVelocityMotorHingeJoint() and updated each time the bodies
     * change their state by UpdateTargetVelocityMotorHingeJoint().\n\n
     * More information can be found in: \cite Deul2014
     *
     * \image html motorhingejoint.jpg "motor hinge joint"
     * \image latex motorhingejoint.jpg "motor hinge joint" width=0.5\textwidth
     *
     * @param inv_mass_0 inverse mass of first body
     * @param x0 center of mass of first body
     * @param v0 velocity of body 0
     * @param inertia_inverse_w_0 inverse inertia tensor in world coordinates of first body
     * @param omega0 angular velocity of first body
     * @param inv_mass_1 inverse mass of second body
     * @param x1 center of mass of second body
     * @param v1 velocity of body 1
     * @param inertia_inverse_w_1 inverse inertia tensor in world coordinates of second body
     * @param omega1 angular velocity of second body
     * @param target_angular_velocity target angular velocity of the motor
     * @param joint_info Motor hinge joint information which is required by the solver. This
     * information must be generated in the beginning by calling InitTargetVelocityMotorHingeJoint()
     * and updated each time the bodies change their state by UpdateTargetVelocityMotorHingeJoint().
     * @param corr_v0 velocity correction of first body
     * @param corr_omega0 angular velocity correction of first body
     * @param corr_v1 velocity correction of second body
     * @param corr_omega1 angular velocity correction of second body
     */
    static bool VelocitySolveTargetVelocityMotorHingeJoint(
            Real inv_mass_0,                      // inverse  mass is zero if body is static
            const Vector3r &x0,                   // center of mass of body 0
            const Vector3r &v0,                   // velocity of body 0
            const Matrix3r &inertia_inverse_w_0,  // inverse inertia tensor (world space) of body 0
            const Vector3r &omega0,
            Real inv_mass_1,     // inverse mass is zero if body is static
            const Vector3r &x1,  // center of mass of body 1
            const Vector3r &v1,
            const Matrix3r &inertia_inverse_w_1,  // inverse inertia tensor (world space) of body 1
            const Vector3r &omega1,
            Real target_angular_velocity,  // target angular velocity of the servo motor
            const Eigen::Matrix<Real, 4, 8, Eigen::DontAlign> &joint_info,  // precomputed joint info
            Vector3r &corr_v0,
            Vector3r &corr_omega0,
            Vector3r &corr_v1,
            Vector3r &corr_omega1);

    //MARK: -
    /** Initialize ball joint between a rigid body and a particle
     * at the position of the particle and return info which is required by the solver step.
     *
     * @param x0 center of mass of the rigid body
     * @param q0 rotation of the rigid body body
     * @param x1 position of the particle
     * @param joint_info Stores the local and global position of the connector point in the rigid body.
     * The info must be updated in each simulation step by calling UpdateRigidBodyParticleBallJoint().\n
     * The joint info contains the following columns:\n
     * 0:	connector in rigid body (local)\n
     * 1:	connector in rigid body (global)\n
     */
    static bool InitRigidBodyParticleBallJoint(const Vector3r &x0,     // center of mass of the rigid body
                                               const Quaternionr &q0,  // rotation of the rigid body body
                                               const Vector3r &x1,     // position of the particle
                                               Eigen::Matrix<Real, 3, 2, Eigen::DontAlign> &joint_info);

    /** Update joint info which is required by the solver step.
     * The joint info must be generated in the initialization process of the model
     * by calling the function InitRigidBodyParticleBallJoint().
     * This method should be called once per simulation step before executing the solver.\n\n
     *
     * @param x0 center of mass of the rigid body
     * @param q0 rotation of the rigid body body
     * @param x1 position of the particle
     * @param joint_info ball joint information which should be updated
     */
    static bool UpdateRigidBodyParticleBallJoint(const Vector3r &x0,     // center of mass of the rigid body
                                                 const Quaternionr &q0,  // rotation of the rigid body body
                                                 const Vector3r &x1,     // position of the particle
                                                 Eigen::Matrix<Real, 3, 2, Eigen::DontAlign> &joint_info);

    /** Perform a solver step for a ball joint which links a rigid body and a particle.
     * The joint info must be generated in the initialization process of the model
     * by calling the function InitRigidBodyParticleBallJoint() and updated each time the rigid body
     * changes its state by UpdateRigidBodyParticleBallJoint().\n\n
     * More information can be found in: \cite Deul2014
     *
     * @param inv_mass_0 inverse mass of rigid body
     * @param x0 center of mass of rigid body
     * @param inertia_inverse_w_0 inverse inertia tensor in world coordinates of rigid body
     * @param q0 rotation of rigid body
     * @param inv_mass_1 inverse mass of particle
     * @param x1 position of particle
     * @param joint_info Joint information which is required by the solver. This
     * information must be generated in the beginning by calling InitRigidBodyParticleBallJoint()
     * and updated each time the rigid body changes its state by UpdateRigidBodyParticleBallJoint().
     * @param corr_x0 position correction of the center of mass of the rigid body
     * @param corr_q0 rotation correction of the rigid body
     * @param corr_x1 position correction of the particle
     */
    static bool SolveRigidBodyParticleBallJoint(
            Real inv_mass_0,                      // inverse mass is zero if body is static
            const Vector3r &x0,                   // center of mass of body 0
            const Matrix3r &inertia_inverse_w_0,  // inverse inertia tensor (world space) of body 0
            const Quaternionr &q0,                // rotation of body 0
            Real inv_mass_1,                      // inverse mass is zero if particle is static
            const Vector3r &x1,                   // position of particle
            const Eigen::Matrix<Real, 3, 2, Eigen::DontAlign> &joint_info,  // precomputed joint info
            Vector3r &corr_x0,
            Quaternionr &corr_q0,
            Vector3r &corr_x1);

    //MARK: -
    /** Initialize contact between two rigid bodies and return
     * info which is required by the solver step.
     *
     * @param inv_mass_0 inverse mass of rigid body
     * @param x0 center of mass of first body
     * @param v0 velocity of body 0
     * @param inertia_inverse_w_0 inverse inertia tensor in world coordinates of first body
     * @param q0 rotation of first body
     * @param omega0 angular velocity of first body
     * @param inv_mass_1 inverse mass of second body
     * @param x1 center of mass of second body
     * @param v1 velocity of body 1
     * @param inertia_inverse_w_1 inverse inertia tensor in world coordinates of second body
     * @param q1 rotation of second body
     * @param omega1 angular velocity of second body
     * @param cp0 contact point of body 0
     * @param cp1 contact point of body 1
     * @param normal contact normal in body 1
     * @param restitution_coeff coefficient of restitution
     * @param constraint_info Stores the local and global position of the contact points and
     * the contact normal. \n
     * The joint info contains the following columns:\n
     * 0:	connector in body 0 (global)\n
     * 1:	connector in body 1 (global)\n
     * 2:	contact normal in body 1 (global)\n
     * 3:	contact tangent (global)\n
     * 0,4:   1.0 / normal^T * K * normal\n
     * 1,4:  maximal impulse in tangent direction\n
     * 2,4:  goal velocity in normal direction after collision
     */
    static bool InitRigidBodyContactConstraint(
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
            Eigen::Matrix<Real, 3, 5, Eigen::DontAlign> &constraint_info);

    /** Perform a solver step for a contact constraint between two rigid bodies.
     * A contact constraint handles collisions and resting contacts between the bodies.
     * The contact info must be generated in each time step.
     *
     * @param inv_mass_0 inverse mass of rigid body
     * @param x0 center of mass of first body
     * @param v0 velocity of body 0
     * @param inertia_inverse_w_0 inverse inertia tensor in world coordinates of first body
     * @param omega0 angular velocity of first body
     * @param inv_mass_1 inverse mass of second body
     * @param x1 center of mass of second body
     * @param v1 velocity of body 1
     * @param inertia_inverse_w_1 inverse inertia tensor in world coordinates of second body
     * @param omega1 angular velocity of second body
     * @param stiffness stiffness parameter of penalty impulse
     * @param friction_coeff friction coefficient
     * @param sum_impulses sum of all correction impulses in normal direction
     * @param constraint_info information which is required by the solver. This
     * information must be generated in the beginning by calling InitRigidBodyContactConstraint().
     * @param corr_v0 velocity correction of first body
     * @param corr_omega0 angular velocity correction of first body
     * @param corr_v1 velocity correction of second body
     * @param corr_omega1 angular velocity correction of second body
     */
    static bool VelocitySolveRigidBodyContactConstraint(
            Real inv_mass_0,                      // inverse mass is zero if body is static
            const Vector3r &x0,                   // center of mass of body 0
            const Vector3r &v0,                   // velocity of body 0
            const Matrix3r &inertia_inverse_w_0,  // inverse inertia tensor (world space) of body 0
            const Vector3r &omega0,               // angular velocity of body 0
            Real inv_mass_1,                      // inverse mass is zero if body is static
            const Vector3r &x1,                   // center of mass of body 1
            const Vector3r &v1,                   // velocity of body 1
            const Matrix3r &inertia_inverse_w_1,  // inverse inertia tensor (world space) of body 1
            const Vector3r &omega1,               // angular velocity of body 1
            Real stiffness,                       // stiffness parameter of penalty impulse
            Real friction_coeff,                  // friction coefficient
            Real &sum_impulses,                   // sum of all impulses
            Eigen::Matrix<Real, 3, 5, Eigen::DontAlign> &constraint_info,  // precomputed contact info
            Vector3r &corr_v0,
            Vector3r &corr_omega0,
            Vector3r &corr_v1,
            Vector3r &corr_omega1);

    //MARK: -
    /** Initialize contact between a rigid body and a particle and return
     * info which is required by the solver step.
     *
     * @param invMass0 inverse mass of rigid body
     * @param x0 center of mass of first body
     * @param v0 velocity of body 0
     * @param invMass1 inverse mass of second body
     * @param x1 center of mass of second body
     * @param v1 velocity of body 1
     * @param inertia_inverse_w_1 inverse inertia tensor in world coordinates of second body
     * @param q1 rotation of second body
     * @param omega1 angular velocity of second body
     * @param cp0 contact point of body 0
     * @param cp1 contact point of body 1
     * @param normal contact normal in body 1
     * @param restitution_coeff coefficient of restitution
     * @param constraint_info Stores the local and global position of the contact points and
     * the contact normal. \n
     * The joint info contains the following columns:\n
     * 0:	connector in body 0 (global)\n
     * 1:	connector in body 1 (global)\n
     * 2:	contact normal in body 1 (global)\n
     * 3:	contact tangent (global)\n
     * 0,4:   1.0 / normal^T * K * normal\n
     * 1,4:  maximal impulse in tangent direction\n
     * 2,4:  goal velocity in normal direction after collision
     */
    static bool InitParticleRigidBodyContactConstraint(
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
            Eigen::Matrix<Real, 3, 5, Eigen::DontAlign> &constraint_info);

    /** Perform a solver step for a contact constraint between a rigid body and a particle.
     * A contact constraint handles collisions and resting contacts between the bodies.
     * The contact info must be generated in each time step.
     *
     * @param inv_mass_0 inverse mass of rigid body
     * @param x0 center of mass of first body
     * @param v0 velocity of body 0
     * @param inv_mass_1 inverse mass of second body
     * @param x1 center of mass of second body
     * @param v1 velocity of body 1
     * @param inertia_inverse_w_1 inverse inertia tensor in world coordinates of second body
     * @param omega1 angular velocity of second body
     * @param stiffness stiffness parameter of penalty impulse
     * @param friction_coeff friction coefficient
     * @param sum_impulses sum of all correction impulses in normal direction
     * @param constraint_info information which is required by the solver. This
     * information must be generated in the beginning by calling InitRigidBodyContactConstraint().
     * @param corr_v0 velocity correction of first body
     * @param corr_v1 velocity correction of second body
     * @param corr_omega1 angular velocity correction of second body
     */
    static bool VelocitySolveParticleRigidBodyContactConstraint(
            Real inv_mass_0,                      // inverse mass is zero if body is static
            const Vector3r &x0,                   // center of mass of body 0
            const Vector3r &v0,                   // velocity of body 0
            Real inv_mass_1,                      // inverse mass is zero if body is static
            const Vector3r &x1,                   // center of mass of body 1
            const Vector3r &v1,                   // velocity of body 1
            const Matrix3r &inertia_inverse_w_1,  // inverse inertia tensor (world space) of body 1
            const Vector3r &omega1,               // angular velocity of body 1
            Real stiffness,                       // stiffness parameter of penalty impulse
            Real friction_coeff,                  // friction coefficient
            Real &sum_impulses,                   // sum of all impulses
            Eigen::Matrix<Real, 3, 5, Eigen::DontAlign> &constraint_info,  // precomputed contact info
            Vector3r &corr_v0,
            Vector3r &corr_v1,
            Vector3r &corr_omega1);

    //MARK: -
    /** Initialize distance joint and return info which is required by the solver step.
     *
     * @param x0 center of mass of first body
     * @param q0 rotation of first body
     * @param x1 center of mass of second body
     * @param q1 rotation of second body
     * @param joint_info Stores the local and global positions of the connector points.
     * The first two columns store the local connectors in body 0 and 1, respectively, while
     * the last two columns contain the global connector positions which have to be
     * updated in each simulation step by calling UpdateDistanceJoint().\n
     * The joint info contains the following columns:\n
     * 0:	connector in body 0 (local)\n
     * 1:	connector in body 1 (local)\n
     * 2:	connector in body 0 (global)\n
     * 3:	connector in body 1 (global)
     */
    static bool InitDistanceJoint(const Vector3r &x0,     // center of mass of body 0
                                  const Quaternionr &q0,  // rotation of body 0
                                  const Vector3r &x1,     // center of mass of body 1
                                  const Quaternionr &q1,  // rotation of body 1
                                  const Vector3r &pos0,
                                  const Vector3r &pos1,
                                  Eigen::Matrix<Real, 3, 4, Eigen::DontAlign> &joint_info);

    /** Update distance joint info which is required by the solver step.
     * The distance joint info must be generated in the initialization process of the model
     * by calling the function InitDistanceJoint().
     * This method should be called once per simulation step before executing the solver.\n\n
     *
     * @param x0 center of mass of first body
     * @param q0 rotation of first body
     * @param x1 center of mass of second body
     * @param q1 rotation of second body
     * @param joint_info joint information which should be updated
     */
    static bool UpdateDistanceJoint(const Vector3r &x0,     // center of mass of body 0
                                    const Quaternionr &q0,  // rotation of body 0
                                    const Vector3r &x1,     // center of mass of body 1
                                    const Quaternionr &q1,  // rotation of body 1
                                    Eigen::Matrix<Real, 3, 4, Eigen::DontAlign> &joint_info);

    /** Perform a solver step for a distance joint which links two rigid bodies.
     * A distance joint removes one translational degrees of freedom between the bodies.
     * When setting a stiffness value which is not zero, we get an implicit spring.
     * The distance joint info must be generated in the initialization process of the model
     * by calling the function InitDistanceJoint() and updated each time the bodies
     * change their state by UpdateDistanceJoint().\n\n
     * More information can be found in: \cite Deul2014
     *
     * @param inv_mass_0 inverse mass of first body
     * @param x0 center of mass of first body
     * @param inertia_inverse_w_0 inverse inertia tensor in world coordinates of first body
     * @param q0 rotation of first body
     * @param inv_mass_1 inverse mass of second body
     * @param x1 center of mass of second body
     * @param inertia_inverse_w_1 inverse inertia tensor in world coordinates of second body
     * @param q1 rotation of second body
     * @param stiffness Stiffness of the constraint. 0 means it is totally stiff and we get a distance joint otherwise
     * we get an implicit spring.
     * @param rest_length Rest length of the joint.
     * @param dt Time step size (required for XPBD when simulating a spring)
     * @param joint_info Joint information which is required by the solver. This
     * information must be generated in the beginning by calling InitDistanceJoint()
     * and updated each time the bodies change their state by UpdateDistanceJoint().
     * @param lambda Lagrange multiplier (required for XPBD). Must be 0 in the first iteration.
     * @param corr_x0 position correction of center of mass of first body
     * @param corr_q0 rotation correction of first body
     * @param corr_x1 position correction of center of mass of second body
     * @param corr_q1 rotation correction of second body
     */
    static bool SolveDistanceJoint(
            Real inv_mass_0,                      // inverse mass is zero if body is static
            const Vector3r &x0,                   // center of mass of body 0
            const Matrix3r &inertia_inverse_w_0,  // inverse inertia tensor (world space) of body 0
            const Quaternionr &q0,                // rotation of body 0
            Real inv_mass_1,                      // inverse mass is zero if body is static
            const Vector3r &x1,                   // center of mass of body 1
            const Matrix3r &inertia_inverse_w_1,  // inverse inertia tensor (world space) of body 1
            const Quaternionr &q1,                // rotation of body 1
            Real stiffness,
            Real rest_length,
            Real dt,
            const Eigen::Matrix<Real, 3, 4, Eigen::DontAlign> &joint_info,  // precomputed joint info
            Real &lambda,
            Vector3r &corr_x0,
            Quaternionr &corr_q0,
            Vector3r &corr_x1,
            Quaternionr &corr_q1);

    //MARK: -
    /** Initialize a motor slider joint which is able to enforce
     * a target position and return info which is required by the solver step.
     *
     * @param x0 center of mass of first body
     * @param q0 rotation of first body
     * @param x1 center of mass of second body
     * @param q1 rotation of second body
     * @param joint_info Stores the local and global positions of the connector points.
     * The joint info contains the following columns:\n
     * 0:	coordinate system in body 0, where the x-axis is the slider axis (local)\n
     * 1:	coordinate system in body 0, where the x-axis is the slider axis (global)\n
     * 2:    3D vector d = R^T * (x0 - x1), where R is a rotation matrix with the slider axis as first column\n
     * 3-5:	projection matrix Pr for the rotational part\n\n
     * The info must be updated in each simulation step
     * by calling UpdateTargetPositionMotorSliderJoint().
     */
    static bool InitDamperJoint(const Vector3r &x0,     // center of mass of body 0
                                const Quaternionr &q0,  // rotation of body 0
                                const Vector3r &x1,     // center of mass of body 1
                                const Quaternionr &q1,  // rotation of body 1
                                const Vector3r &direction,
                                Eigen::Matrix<Real, 4, 6, Eigen::DontAlign> &joint_info);

    /** Update motor slider joint info which is required by the solver step.
     * The joint info must be generated in the initialization process of the model
     * by calling the function InitTargetPositionMotorSliderJoint().
     * This method should be called once per simulation step before executing the solver.\n\n
     *
     * @param x0 center of mass of first body
     * @param q0 rotation of first body
     * @param x1 center of mass of second body
     * @param q1 rotation of second body
     * @param joint_info slider joint information which should be updated
     */
    static bool UpdateDamperJoint(const Vector3r &x0,     // center of mass of body 0
                                  const Quaternionr &q0,  // rotation of body 0
                                  const Vector3r &x1,     // center of mass of body 1
                                  const Quaternionr &q1,  // rotation of body 1
                                  Eigen::Matrix<Real, 4, 6, Eigen::DontAlign> &joint_info);

    /** Perform a solver step for a motor slider joint which links two rigid bodies.
     * A motor slider joint removes two translational and three rotational degrees of freedom between the bodies.
     * Moreover, a target position can be enforced on the remaining translation axis.
     * The motor slider joint info must be generated in the initialization process of the model
     * by calling the function InitTargetPositionMotorSliderJoint() and updated each time the bodies
     * change their state by UpdateTargetPositionMotorSliderJoint().\n\n
     * More information can be found in: \cite Deul2014
     *
     * \image html motorsliderjoint.jpg "motor slider joint"
     * \image latex motorsliderjoint.jpg "motor slider joint" width=0.5\textwidth
     *
     * @param inv_mass_0 inverse mass of first body
     * @param x0 center of mass of first body
     * @param inertia_inverse_w_0 inverse inertia tensor in world coordinates of first body
     * @param q0 rotation of first body
     * @param inv_mass_1 inverse mass of second body
     * @param x1 center of mass of second body
     * @param inertia_inverse_w_1 inverse inertia tensor in world coordinates of second body
     * @param q1 rotation of second body
     * @param joint_info Motor slider joint information which is required by the solver. This
     * information must be generated in the beginning by calling InitTargetPositionMotorSliderJoint()
     * and updated each time the bodies change their state by UpdateTargetPositionMotorSliderJoint().
     * @param corr_x0 position correction of center of mass of first body
     * @param corr_q0 rotation correction of first body
     * @param corr_x1 position correction of center of mass of second body
     * @param corr_q1 rotation correction of second body
     */
    static bool SolveDamperJoint(
            Real inv_mass_0,                      // inverse  mass is zero if body is static
            const Vector3r &x0,                   // center of mass of body 0
            const Matrix3r &inertia_inverse_w_0,  // inverse inertia tensor (world space) of body 0
            const Quaternionr &q0,                // rotation of body 0
            Real inv_mass_1,                      // inverse mass is zero if body is static
            const Vector3r &x1,                   // center of mass of body 1
            const Matrix3r &inertia_inverse_w_1,  // inverse inertia tensor (world space) of body 1
            const Quaternionr &q1,                // rotation of body 1
            Real stiffness,
            Real dt,
            const Eigen::Matrix<Real, 4, 6, Eigen::DontAlign> &joint_info,  // precomputed slider joint info
            Real &lambda,
            Vector3r &corr_x0,
            Quaternionr &corr_q0,
            Vector3r &corr_x1,
            Quaternionr &corr_q1);
};

}  // namespace vox::force
