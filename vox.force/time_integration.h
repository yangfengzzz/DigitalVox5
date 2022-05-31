//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.force/common.h"

// ------------------------------------------------------------------------------------
namespace vox::force {
class TimeIntegration {
public:
    /** Perform an integration step for a particle using the semi-implicit Euler
     * (symplectic Euler) method:
     * \f{align*}{
     * \mathbf{v}(t+h) &= \mathbf{v}(t) + \mathbf{a}(t) h\\
     * \mathbf{x}(t+h) &= \mathbf{x}(t) + \mathbf{v}(t+h) h
     * \f}
     *
     * @param  h time step size
     * @param  mass mass of the particle
     * @param  position position of the particle
     * @param  velocity velocity of the particle
     * @param  acceleration acceleration of the particle
     */
    static void SemiImplicitEuler(
            Real h, Real mass, Vector3r &position, Vector3r &velocity, const Vector3r &acceleration);

    // -------------- semi-implicit Euler (symplectic Euler) for rotational part of a rigid body -----------------
    static void SemiImplicitEulerRotation(Real h,
                                          Real mass,
                                          const Matrix3r &invertia_w,
                                          const Matrix3r &inv_inertia_w,
                                          Quaternionr &rotation,
                                          Vector3r &angular_velocity,
                                          const Vector3r &torque);

    // -------------- velocity update (first order) -----------------------------------------------------
    /** Perform a velocity update (first order) for the linear velocity:
     * \f{equation*}{
     * \mathbf{v}(t+h) = \frac{1}{h} (\mathbf{p}(t+h) - \mathbf{p}(t)
     * \f}
     *
     * @param  h time step size
     * @param  mass mass of the particle
     * @param  position new position \f$\mathbf{p}(t+h)\f$ of the particle
     * @param  old_position position \f$\mathbf{p}(t)\f$ of the particle before the time step
     * @param  velocity resulting velocity of the particle
     */
    static void VelocityUpdateFirstOrder(
            Real h,
            Real mass,
            const Vector3r &position,     // position after constraint projection	at time t+h
            const Vector3r &old_position,  // position before constraint projection at time t
            Vector3r &velocity);

    // -------------- angular velocity update (first order)  ------------------------------------------------
    static void AngularVelocityUpdateFirstOrder(
            Real h,
            Real mass,
            const Quaternionr &rotation,     // rotation after constraint projection	at time t+h
            const Quaternionr &old_rotation,  // rotation before constraint projection at time t
            Vector3r &angular_velocity);

    // -------------- velocity update (second order) -----------------------------------------------------
    static void VelocityUpdateSecondOrder(
            Real h,
            Real mass,
            const Vector3r &position,            // position after constraint projection	at time t+h
            const Vector3r &old_position,         // position before constraint projection at time t
            const Vector3r &position_of_last_step,  // position of last simulation step at time t-h
            Vector3r &velocity);

    // -------------- angular velocity update (second order)  ------------------------------------------------
    static void AngularVelocityUpdateSecondOrder(
            Real h,
            Real mass,
            const Quaternionr &rotation,            // rotation after constraint projection	at time t+h
            const Quaternionr &old_rotation,         // rotation before constraint projection at time t
            const Quaternionr &rotation_of_last_step,  // rotation of last simulation step at time t-h
            Vector3r &angular_velocity);
};

}  // namespace vox::force