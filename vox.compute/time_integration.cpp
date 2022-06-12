//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.compute/time_integration.h"

namespace vox::compute {

// ----------------------------------------------------------------------------------------------
void TimeIntegration::SemiImplicitEuler(
        Real h, Real mass, Vector3r &position, Vector3r &velocity, const Vector3r &acceleration) {
    if (mass != 0.0) {
        velocity += acceleration * h;
        position += velocity * h;
    }
}

// ----------------------------------------------------------------------------------------------
void TimeIntegration::SemiImplicitEulerRotation(Real h,
                                                Real mass,
                                                const Matrix3r &invertia_w,
                                                const Matrix3r &inv_inertia_w,
                                                Quaternionr &rotation,
                                                Vector3r &angular_velocity,
                                                const Vector3r &torque) {
    if (mass != 0.0) {
        angular_velocity += h * inv_inertia_w * (torque - (angular_velocity.cross(invertia_w * angular_velocity)));

        Quaternionr angVelQ(0.0, angular_velocity[0], angular_velocity[1], angular_velocity[2]);
        rotation.coeffs() += h * 0.5 * (angVelQ * rotation).coeffs();
        rotation.normalize();
    }
}

// ----------------------------------------------------------------------------------------------
void TimeIntegration::VelocityUpdateFirstOrder(
        Real h, Real mass, const Vector3r &position, const Vector3r &old_position, Vector3r &velocity) {
    if (mass != 0.0) velocity = (1.0 / h) * (position - old_position);
}

// ----------------------------------------------------------------------------------------------
void TimeIntegration::AngularVelocityUpdateFirstOrder(
        Real h, Real mass, const Quaternionr &rotation, const Quaternionr &old_rotation, Vector3r &angular_velocity) {
    if (mass != 0.0) {
        const Quaternionr relRot = (rotation * old_rotation.conjugate());
        angular_velocity = relRot.vec() * (2.0 / h);
    }
}

// ----------------------------------------------------------------------------------------------
void TimeIntegration::VelocityUpdateSecondOrder(Real h,
                                                Real mass,
                                                const Vector3r &position,
                                                const Vector3r &old_position,
                                                const Vector3r &position_of_last_step,
                                                Vector3r &velocity) {
    if (mass != 0.0) velocity = (1.0 / h) * (1.5 * position - 2.0 * old_position + 0.5 * position_of_last_step);
}

// ----------------------------------------------------------------------------------------------
void TimeIntegration::AngularVelocityUpdateSecondOrder(Real h,
                                                       Real mass,
                                                       const Quaternionr &rotation,
                                                       const Quaternionr &old_rotation,
                                                       const Quaternionr &rotation_of_last_step,
                                                       Vector3r &angular_velocity) {
    // ToDo: is still first order
    if (mass != 0.0) {
        const Quaternionr relRot = (rotation * old_rotation.conjugate());
        angular_velocity = relRot.vec() * (2.0 / h);
    }
}

}  // namespace vox::compute
