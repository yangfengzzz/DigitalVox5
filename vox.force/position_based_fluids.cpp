//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.force/position_based_fluids.h"

#include "vox.force/sph_kernels.h"

namespace vox::force {

// ----------------------------------------------------------------------------------------------
bool PositionBasedFluids::ComputePbfDensity(unsigned int particle_index,
                                            unsigned int number_of_particles,
                                            const Vector3r x[],
                                            const Real mass[],
                                            const Vector3r boundary_x[],
                                            const Real boundary_psi[],
                                            unsigned int num_neighbors,
                                            const unsigned int neighbors[],
                                            Real density0,
                                            bool boundary_handling,
                                            Real &density_err,
                                            Real &density) {
    // Compute current density for particle i
    density = mass[particle_index] * CubicKernel::W_zero();
    for (unsigned int j = 0; j < num_neighbors; j++) {
        const unsigned int kNeighborIndex = neighbors[j];
        if (kNeighborIndex < number_of_particles)  // Test if fluid particle
        {
            density += mass[kNeighborIndex] * CubicKernel::W(x[particle_index] - x[kNeighborIndex]);
        } else if (boundary_handling) {
            // Boundary: Akinci2012
            density += boundary_psi[kNeighborIndex - number_of_particles] *
                       CubicKernel::W(x[particle_index] - boundary_x[kNeighborIndex - number_of_particles]);
        }
    }

    density_err = std::max(density, density0) - density0;
    return true;
}

// ----------------------------------------------------------------------------------------------
bool PositionBasedFluids::ComputePbfLagrangeMultiplier(unsigned int particle_index,
                                                       unsigned int number_of_particles,
                                                       const Vector3r x[],
                                                       const Real mass[],
                                                       const Vector3r boundary_x[],
                                                       const Real boundary_psi[],
                                                       Real density,
                                                       unsigned int num_neighbors,
                                                       const unsigned int neighbors[],
                                                       Real density0,
                                                       bool boundary_handling,
                                                       Real &lambda) {
    const Real eps = static_cast<Real>(1.0e-6);

    // Evaluate constraint function
    const Real C = std::max(density / density0 - static_cast<Real>(1.0),
                            static_cast<Real>(0.0));  // clamp to prevent particle clumping at surface

    if (C != 0.0) {
        // Compute gradients dC/dx_j
        Real sum_grad_C2 = 0.0;
        Vector3r gradC_i(0.0, 0.0, 0.0);

        for (unsigned int j = 0; j < num_neighbors; j++) {
            const unsigned int neighborIndex = neighbors[j];
            if (neighborIndex < number_of_particles)  // Test if fluid particle
            {
                const Vector3r gradC_j =
                        -mass[neighborIndex] / density0 * CubicKernel::gradW(x[particle_index] - x[neighborIndex]);
                sum_grad_C2 += gradC_j.squaredNorm();
                gradC_i -= gradC_j;
            } else if (boundary_handling) {
                // Boundary: Akinci2012
                const Vector3r gradC_j =
                        -boundary_psi[neighborIndex - number_of_particles] / density0 *
                        CubicKernel::gradW(x[particle_index] - boundary_x[neighborIndex - number_of_particles]);
                sum_grad_C2 += gradC_j.squaredNorm();
                gradC_i -= gradC_j;
            }
        }

        sum_grad_C2 += gradC_i.squaredNorm();

        // Compute lambda
        lambda = -C / (sum_grad_C2 + eps);
    } else
        lambda = 0.0;

    return true;
}

// ----------------------------------------------------------------------------------------------
bool PositionBasedFluids::SolveDensityConstraint(unsigned int particle_index,
                                                 unsigned int number_of_particles,
                                                 const Vector3r x[],
                                                 const Real mass[],
                                                 const Vector3r boundary_x[],
                                                 const Real boundary_psi[],
                                                 unsigned int num_neighbors,
                                                 const unsigned int neighbors[],
                                                 Real density0,
                                                 bool boundary_handling,
                                                 const Real lambda[],
                                                 Vector3r &corr) {
    // Compute position correction
    corr.setZero();
    for (unsigned int j = 0; j < num_neighbors; j++) {
        const unsigned int kNeighborIndex = neighbors[j];
        if (kNeighborIndex < number_of_particles)  // Test if fluid particle
        {
            const Vector3r gradC_j =
                    -mass[kNeighborIndex] / density0 * CubicKernel::gradW(x[particle_index] - x[kNeighborIndex]);
            corr -= (lambda[particle_index] + lambda[kNeighborIndex]) * gradC_j;
        } else if (boundary_handling) {
            // Boundary: Akinci2012
            const Vector3r gradC_j =
                    -boundary_psi[kNeighborIndex - number_of_particles] / density0 *
                    CubicKernel::gradW(x[particle_index] - boundary_x[kNeighborIndex - number_of_particles]);
            corr -= (lambda[particle_index]) * gradC_j;
        }
    }

    return true;
}

}  // namespace vox::force