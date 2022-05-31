//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.force/common.h"

// ------------------------------------------------------------------------------------
namespace vox::force {
class XPBD {
public:
    // -------------- Distance constraint -----------------------------------------------------

    /** Determine the position corrections for a distance constraint between two particles using XPBD:\n\n
     * \f$C(\mathbf{p}_0, \mathbf{p}_1) = \| \mathbf{p}_0 - \mathbf{p}_1\| - l_0 = 0\f$\n\n
     * More information can be found in the following papers: \cite Mueller07, \cite BMOT2013, \cite BMOTM2014, \cite
     * BMM2015, \cite MMC16
     *
     * @param p0 position of first particle
     * @param inv_mass_0 inverse mass of first particle
     * @param p1 position of second particle
     * @param inv_mass_1 inverse mass of second particle
     * @param rest_length rest length of distance constraint
     * @param stiffness stiffness coefficient
     * @param dt Time step size
     * @param lambda Lagrange multiplier (XPBD)
     * @param corr0 position correction of first particle
     * @param corr1 position correction of second particle
     */
    static bool SolveDistanceConstraint(const Vector3r& p0,
                                        Real inv_mass_0,
                                        const Vector3r& p1,
                                        Real inv_mass_1,
                                        Real rest_length,
                                        Real stiffness,
                                        Real dt,
                                        Real& lambda,
                                        Vector3r& corr0,
                                        Vector3r& corr1);

    /** Determine the position corrections for a constraint that conserves the volume
     * of single tetrahedron. Such a constraint has the form
     * \f{equation*}{
     * C(\mathbf{p}_1, \mathbf{p}_2, \mathbf{p}_3, \mathbf{p}_4) = \frac{1}{6}
     * \left(\mathbf{p}_{2,1} \times \mathbf{p}_{3,1}\right) \cdot \mathbf{p}_{4,1} - V_0,
     * \f}
     * where \f$\mathbf{p}_1\f$, \f$\mathbf{p}_2\f$, \f$\mathbf{p}_3\f$ and \f$\mathbf{p}_4\f$
     * are the four corners of the tetrahedron and \f$V_0\f$ is its rest volume.\n\n
     * More information can be found in the following papers: \cite Mueller07, \cite BMOT2013, \cite BMOTM2014, \cite
     * BMM2015,
     *
     * @param p0 position of first particle
     * @param inv_mass_0 inverse mass of first particle
     * @param p1 position of second particle
     * @param inv_mass_1 inverse mass of second particle
     * @param p2 position of third particle
     * @param inv_mass_2 inverse mass of third particle
     * @param p3 position of fourth particle
     * @param inv_mass_3 inverse mass of fourth particle
     * @param rest_volume rest angle \f$V_0\f$
     * @param stiffness stiffness coefficient
     * @param dt Time step size
     * @param lambda Lagrange multiplier (XPBD)
     * @param corr0 position correction of first particle
     * @param corr1 position correction of second particle
     * @param corr2 position correction of third particle
     * @param corr3 position correction of fourth particle
     */
    static bool SolveVolumeConstraint(const Vector3r& p0,
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
                                      Vector3r& corr3);

    // -------------- Isometric bending -----------------------------------------------------

    /** Initialize the local stiffness matrix Q. The matrix is
     * required by the solver step. It must only be recomputed
     * if the rest shape changes. \n\n
     * Bending is simulated for the angle on (p2, p3) between
     * the triangles (p0, p2, p3) and (p1, p3, p2).
     *
     * @param  p0 point 0 of stencil
     * @param  p1 point 1 of stencil
     * @param  p2 point 2 of stencil
     * @param  p3 point 3 of stencil
     * @param  Q returns the local stiffness matrix which is required by the solver
     */
    static bool InitIsometricBendingConstraint(
            const Vector3r& p0, const Vector3r& p1, const Vector3r& p2, const Vector3r& p3, Matrix4r& Q);

    /** Determine the position corrections for the isometric bending constraint using XPBD.
     * This constraint can be used for almost inextensible surface models.\n\n
     * More information can be found in: \cite BMM2015, \cite Bender2014
     *
     * @param p0 position of first particle
     * @param inv_mass_0 inverse mass of first particle
     * @param p1 position of second particle
     * @param inv_mass_1 inverse mass of second particle
     * @param p2 position of third particle
     * @param inv_mass_2 inverse mass of third particle
     * @param p3 position of fourth particle
     * @param inv_mass_3 inverse mass of fourth particle
     * @param  Q local stiffness matrix which must be initialized by calling InitIsometricBendingConstraint()
     * @param  stiffness stiffness coefficient for bending
     * @param dt Time step size
     * @param lambda Lagrange multiplier (XPBD)
     * @param corr0 position correction of first particle
     * @param corr1 position correction of second particle
     * @param corr2 position correction of third particle
     * @param corr3 position correction of fourth particle
     */
    static bool SolveIsometricBendingConstraint(
            const Vector3r& p0,
            Real inv_mass_0,  // angle on (p2, p3) between triangles (p0, p2, p3) and (p1, p3, p2)
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
            Vector3r& corr3);
};
}  // namespace vox::force
