//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.force/common.h"

// ------------------------------------------------------------------------------------
namespace vox::force {
class PositionBasedDynamics {
public:
    //MARK: -------------- standard PBD -----------------------------------------------------

    /** Determine the position corrections for a distance constraint between two particles:
     * \f$ C(\mathbf{p}_0, \mathbf{p}_1) = \| \mathbf{p}_0 - \mathbf{p}_1\| - l_0 = 0\f$
     * More information can be found in the following papers: \cite Mueller07, \cite BMOT2013, \cite BMOTM2014, \cite
     * BMM2015,
     *
     * @param p0 position of first particle
     * @param inv_mass_0 inverse mass of first particle
     * @param p1 position of second particle
     * @param inv_mass_1 inverse mass of second particle
     * @param rest_length rest length of distance constraint
     * @param stiffness stiffness coefficient
     * @param corr0 position correction of first particle
     * @param corr1 position correction of second particle
     */
    static bool SolveDistanceConstraint(const Vector3r &p0,
                                        Real inv_mass_0,
                                        const Vector3r &p1,
                                        Real inv_mass_1,
                                        Real rest_length,
                                        Real stiffness,
                                        Vector3r &corr0,
                                        Vector3r &corr1);

    /** Determine the position corrections for a dihedral bending constraint.
     * For a pair of adjacent triangles
     * \f$(\mathbf{p}_1, \mathbf{p}_3, \mathbf{p}_2)\f$ and
     * \f$(\mathbf{p}_1, \mathbf{p}_2, \mathbf{p}_4)\f$
     * with the common edge \f$(\mathbf{p}_3, \mathbf{p}_4)\f$ a bilateral bending
     * constraint is added by the constraint function
     * \f{equation*}{
     * C_{bend}(\mathbf{p}_1, \mathbf{p}_2,\mathbf{p}_3, \mathbf{p}_4) =
     * \text{acos}\left( \frac{\mathbf{p}_{2,1} \times
     * \mathbf{p}_{3,1}}{|\mathbf{p}_{2,1} \times
     * \mathbf{p}_{3,1}|} \cdot
     * \frac{\mathbf{p}_{2,1} \times
     * \mathbf{p}_{4,1}}{|\mathbf{p}_{2,1} \times
     *	\mathbf{p}_{4,1}|}\right)-\varphi_0
     * \f}
     * and stiffness \f$k_{bend}\f$. The scalar \f$\varphi_0\f$
     * is the initial dihedral angle between the two triangles and
     * \f$k_{bend}\f$ is a global user parameter defining the bending stiffness.\n\n
     * More information can be found in the following papers: \cite Mueller07, \cite BMOT2013, \cite BMOTM2014, \cite
     *BMM2015,
     *
     * @param p0 position of first particle
     * @param inv_mass_0 inverse mass of first particle
     * @param p1 position of second particle
     * @param inv_mass_1 inverse mass of second particle
     * @param p2 position of third particle
     * @param inv_mass_2 inverse mass of third particle
     * @param p3 position of fourth particle
     * @param inv_mass_3 inverse mass of fourth particle
     * @param rest_angle rest angle \f$\varphi_0\f$
     * @param stiffness stiffness coefficient
     * @param corr0 position correction of first particle
     * @param corr1 position correction of second particle
     * @param corr2 position correction of third particle
     * @param corr3 position correction of fourth particle
     */
    static bool SolveDihedralConstraint(
            const Vector3r &p0,
            Real inv_mass_0,  // angle on (p2, p3) between triangles (p0, p2, p3) and (p1, p3, p2)
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
            Vector3r &corr3);

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
     * @param corr0 position correction of first particle
     * @param corr1 position correction of second particle
     * @param corr2 position correction of third particle
     * @param corr3 position correction of fourth particle
     */
    static bool SolveVolumeConstraint(const Vector3r &p0,
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
                                      Vector3r &corr3);

    /** Determine the position corrections for a constraint that preserves a
     * rest distance between a point and an edge.
     *
     * @param  p position of point particle
     * @param  inv_mass inverse mass of point particle
     * @param  p0 position of first edge particle
     * @param  inv_mass_0 inverse mass of first edge particle
     * @param  p1 position of second edge particle
     * @param  inv_mass_1 inverse mass of second edge particle
     * @param  rest_dist rest distance of point and edge
     * @param  compression_stiffness stiffness coefficient for compression
     * @param  stretch_stiffness stiffness coefficient for stretching
     * @param  corr position correction of point particle
     * @param  corr0 position correction of first edge particle
     * @param  corr1 position correction of second edge particle
     */
    static bool SolveEdgePointDistanceConstraint(const Vector3r &p,
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
                                                 Vector3r &corr1);

    /** Determine the position corrections for a constraint that preserves a
     * rest distance between a point and a triangle.
     *
     * @param  p position of point particle
     * @param  inv_mass inverse mass of point particle
     * @param  p0 position of first triangle particle
     * @param  inv_mass_0 inverse mass of first triangle particle
     * @param  p1 position of second triangle particle
     * @param  inv_mass_1 inverse mass of second triangle particle
     * @param  p2 position of third triangle particle
     * @param  inv_mass_2 inverse mass of third triangle particle
     * @param  rest_dist rest distance of point and triangle
     * @param  compression_stiffness stiffness coefficient for compression
     * @param  stretch_stiffness stiffness coefficient for stretching
     * @param  corr position correction of point particle
     * @param  corr0 position correction of first triangle particle
     * @param  corr1 position correction of second triangle particle
     * @param  corr2 position correction of third triangle particle
     */
    static bool SolveTrianglePointDistanceConstraint(const Vector3r &p,
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
                                                     Vector3r &corr2);

    /** Determine the position corrections for a constraint that preserves a
     * rest distance between two edges.
     *
     * @param  p0 position of first particle of edge 0
     * @param  inv_mass_0 inverse mass of first particle of edge 0
     * @param  p1 position of second particle of edge 0
     * @param  inv_mass_1 inverse mass of second particle of edge 0
     * @param  p2 position of first particle of edge 1
     * @param  inv_mass_2 inverse mass of first particle of edge 1
     * @param  p3 position of second particle of edge 1
     * @param  inv_mass_3 inverse mass of second particle of edge 1
     * @param  rest_dist rest distance between both edges
     * @param  compression_stiffness stiffness coefficient for compression
     * @param  stretch_stiffness stiffness coefficient for stretching
     * @param  corr0 position correction of first particle of edge 0
     * @param  corr1 position correction of second particle of edge 0
     * @param  corr2 position correction of first particle of edge 1
     * @param  corr3 position correction of second particle of edge 1
     */
    static bool SolveEdgeEdgeDistanceConstraint(const Vector3r &p0,
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
                                                Vector3r &corr3);

    //MARK: -------------- Isometric bending -----------------------------------------------------

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
            const Vector3r &p0, const Vector3r &p1, const Vector3r &p2, const Vector3r &p3, Matrix4r &Q);

    /** Determine the position corrections for the isometric bending constraint.
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
     * @param corr0 position correction of first particle
     * @param corr1 position correction of second particle
     * @param corr2 position correction of third particle
     * @param corr3 position correction of fourth particle
     */
    static bool SolveIsometricBendingConstraint(
            const Vector3r &p0,
            Real inv_mass_0,  // angle on (p2, p3) between triangles (p0, p2, p3) and (p1, p3, p2)
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
            Vector3r &corr3);

    //MARK: -------------- Shape Matching  -----------------------------------------------------

    /** Initialize rest configuration infos for one shape matching cluster
     * which are required by the solver step. It must only be reinitialized
     * if the rest shape changes.
     *
     * @param  x0 rest configuration of all particles in the cluster
     * @param  inv_masses inverse masses of all particles in the cluster
     * @param  num_points number of particles in the cluster
     * @param  rest_cm returns the center of mass of the rest configuration
     */
    static bool InitShapeMatchingConstraint(const Vector3r x0[],
                                            const Real inv_masses[],
                                            int num_points,
                                            Vector3r &rest_cm);

    /** Determine the position corrections for a shape matching constraint.\n\n
     * More information can be found in: \cite BMM2015, \cite BMOT2013, \cite BMOTM2014,
     * \cite Muller2005, \cite Bender2013, \cite Diziol2011
     *
     * @param  x0			rest configuration of all particles in the cluster
     * @param  x			current configuration of all particles in the cluster
     * @param  inv_masses	invMasses inverse masses of all particles in the cluster
     * @param  num_points	number of particles in the cluster
     * @param  rest_cm		center of mass of the rest configuration
     * @param  stiffness	stiffness coefficient
     * @param  allow_stretch allow stretching
     * @param  corr			position corrections for all particles in the cluster
     * @param  rot			returns determined rotation matrix
     */
    static bool SolveShapeMatchingConstraint(const Vector3r x0[],
                                             const Vector3r x[],
                                             const Real inv_masses[],
                                             int num_points,
                                             const Vector3r &rest_cm,
                                             Real stiffness,
                                             bool allow_stretch,  // default false
                                             Vector3r corr[],
                                             Matrix3r *rot = nullptr);

    //MARK: -------------- Strain Based Dynamics  -----------------------------------------------------

    /** Initialize rest configuration infos which are required by the solver step.
     * Recomputation is only necessary when rest shape changes.\n\n
     * The triangle is defined in the xy plane.
     *
     * @param  p0 point 0 of triangle
     * @param  p1 point 1 of triangle
     * @param  p2 point 2 of triangle
     * @param  inv_rest_mat returns a matrix required by the solver
     */
    static bool InitStrainTriangleConstraint(const Vector3r &p0,
                                             const Vector3r &p1,
                                             const Vector3r &p2,
                                             Matrix2r &inv_rest_mat);

    /** Solve triangle constraint with strain based dynamics and return position corrections.\n\n
     * More information can be found in: \cite BMM2015, \cite Mueller2014
     *
     * @param p0 position of first particle
     * @param inv_mass_0 inverse mass of first particle
     * @param p1 position of second particle
     * @param inv_mass_1 inverse mass of second particle
     * @param p2 position of third particle
     * @param inv_mass_2 inverse mass of third particle
     * @param  inv_rest_mat precomputed matrix determined by InitStrainTriangleConstraint()
     * @param  xx_stiffness stiffness coefficient for xx stretching
     * @param  yy_stiffness stiffness coefficient for yy stretching
     * @param  xy_stiffness stiffness coefficient for xy shearing
     * @param  normalize_stretch	should stretching be normalized
     * @param  normalize_shear should shearing be normalized
     * @param  corr0 position correction for point 0
     * @param  corr1 position correction for point 1
     * @param  corr2 position correction for point 2
     */
    static bool SolveStrainTriangleConstraint(const Vector3r &p0,
                                              Real inv_mass_0,
                                              const Vector3r &p1,
                                              Real inv_mass_1,
                                              const Vector3r &p2,
                                              Real inv_mass_2,
                                              const Matrix2r &inv_rest_mat,
                                              Real xx_stiffness,
                                              Real yy_stiffness,
                                              Real xy_stiffness,
                                              bool normalize_stretch,  // use false as default
                                              bool normalize_shear,    // use false as default
                                              Vector3r &corr0,
                                              Vector3r &corr1,
                                              Vector3r &corr2);

    /** Initialize rest configuration infos which are required by the solver step.
     * Recomputation is only necessary when rest shape changes.
     *
     * @param  p0 point 0 of tet
     * @param  p1 point 1 of tet
     * @param  p2 point 2 of tet
     * @param  p3 point 3 of tet
     * @param  inv_rest_mat returns a matrix required by the solver
     */
    static bool InitStrainTetraConstraint(
            const Vector3r &p0, const Vector3r &p1, const Vector3r &p2, const Vector3r &p3, Matrix3r &inv_rest_mat);

    // has no inversion handling. Possible simple solution: if the volume is negative,
    // scale corrections down and use the volume constraint to fix the volume sign
    static bool SolveStrainTetraConstraint(const Vector3r &p0,
                                           Real inv_mass_0,
                                           const Vector3r &p1,
                                           Real inv_mass_1,
                                           const Vector3r &p2,
                                           Real inv_mass_2,
                                           const Vector3r &p3,
                                           Real inv_mass_3,
                                           const Matrix3r &inv_rest_mat,
                                           const Vector3r &stretch_stiffness,  // xx, yy, zz
                                           const Vector3r &shear_stiffness,    // xy, xz, yz
                                           bool normalize_stretch,             // use false as default
                                           bool normalize_shear,               // use false as default
                                           Vector3r &corr0,
                                           Vector3r &corr1,
                                           Vector3r &corr2,
                                           Vector3r &corr3);

    //MARK: -------------- FEM Based PBD  -----------------------------------------------------
private:
    static void ComputeGradCGreen(Real rest_volume, const Matrix3r &inv_rest_mat, const Matrix3r &sigma, Vector3r *J);

    static void ComputeGreenStrainAndPiolaStress(const Vector3r &x1,
                                                 const Vector3r &x2,
                                                 const Vector3r &x3,
                                                 const Vector3r &x4,
                                                 const Matrix3r &inv_rest_mat,
                                                 Real rest_volume,
                                                 Real mu,
                                                 Real lambda,
                                                 Matrix3r &epsilon,
                                                 Matrix3r &sigma,
                                                 Real &energy);

    static void ComputeGreenStrainAndPiolaStressInversion(const Vector3r &x1,
                                                          const Vector3r &x2,
                                                          const Vector3r &x3,
                                                          const Vector3r &x4,
                                                          const Matrix3r &inv_rest_mat,
                                                          Real rest_volume,
                                                          Real mu,
                                                          Real lambda,
                                                          Matrix3r &epsilon,
                                                          Matrix3r &sigma,
                                                          Real &energy);

public:
    /** Initialize rest configuration infos which are required by the solver step.
     * Recomputation is only necessary when rest shape changes.
     */
    static bool InitFemTriangleConstraint(
            const Vector3r &p0, const Vector3r &p1, const Vector3r &p2, Real &area, Matrix2r &inv_rest_mat);

    static bool SolveFemTriangleConstraint(const Vector3r &p0,
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
                                           Vector3r &corr2);

    /** Initialize rest configuration infos which are required by the solver step.
     * Recomputation is only necessary when rest shape changes.
     */
    static bool InitFemTetraConstraint(const Vector3r &p0,
                                       const Vector3r &p1,
                                       const Vector3r &p2,
                                       const Vector3r &p3,
                                       Real &volume,
                                       Matrix3r &inv_rest_mat);

    static bool SolveFemTetraConstraint(const Vector3r &p0,
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
                                        Vector3r &corr3);

    /** Initialize contact between a particle and a tetrahedron and return
     * info which is required by the solver step.
     *
     * @param inv_mass_0 inverse mass of particle which collides with tet
     * @param x0 particle position
     * @param v0 particle velocity
     * @param inv_mass inverse masses of tet particles
     * @param x positions of tet particles
     * @param v velocities of tet particles
     * @param bary barycentric coordinates of contact point in tet
     * @param normal contact normal in body 1
     * @param constraint_info Stores the local and global position of the contact points and
     * the contact normal. \n
     * The joint info contains the following columns:\n
     * 0:	contact normal in body 1 (global)\n
     * 1:	contact tangent (global)\n
     * 0,2:   1.0 / normal^T * K * normal\n
     * 1,2:  maximal impulse in tangent direction\n
     */
    static bool InitParticleTetContactConstraint(
            Real inv_mass_0,         // inverse mass is zero if particle is static
            const Vector3r &x0,      // particle which collides with tet
            const Vector3r &v0,      // velocity of particle
            const Real inv_mass[],   // inverse masses of tet particles
            const Vector3r x[],      // positions of tet particles
            const Vector3r v[],      // velocities of tet particles
            const Vector3r &bary,    // barycentric coordinates of contact point in tet
            const Vector3r &normal,  // contact normal in body 1
            Eigen::Matrix<Real, 3, 3, Eigen::DontAlign> &constraint_info);

    /** Perform a solver step for a contact constraint between a particle and a tetrahedron.
     * A contact constraint handles collisions and resting contacts between the bodies.
     * The contact info must be generated in each time step.
     *
     * @param inv_mass_0 inverse mass of particle which collides with tet
     * @param x0 particle position
     * @param inv_mass inverse masses of tet particles
     * @param x positions of tet particles
     * @param bary barycentric coordinates of contact point in tet
     * @param constraint_info information which is required by the solver. This
     * information must be generated in the beginning by calling InitRigidBodyContactConstraint().
     * @param corr0 position correction of particle
     * @param corr position corrections of tet particles
     */
    static bool SolveParticleTetContactConstraint(
            Real inv_mass_0,        // inverse mass is zero if particle is static
            const Vector3r &x0,     // particle which collides with tet
            const Real inv_mass[],  // inverse masses of tet particles
            const Vector3r x[],     // positions of tet particles
            const Vector3r &bary,   // barycentric coordinates of contact point in tet
            Eigen::Matrix<Real, 3, 3, Eigen::DontAlign> &constraint_info,  // precomputed contact info
            Real &lambda,
            Vector3r &corr0,
            Vector3r corr[]);

    /** Perform a solver step for a contact constraint between a particle and a tetrahedron.
     * A contact constraint handles collisions and resting contacts between the bodies.
     * The contact info must be generated in each time step.
     *
     * @param inv_mass_0 inverse mass of particle which collides with tet
     * @param x0 particle position
     * @param v0 particle velocity
     * @param inv_mass inverse masses of tet particles
     * @param x positions of tet particles
     * @param v velocities of tet particles
     * @param bary barycentric coordinates of contact point in tet
     * @param friction_coeff friction coefficient
     * @param constraint_info information which is required by the solver. This
     * information must be generated in the beginning by calling InitRigidBodyContactConstraint().
     * @param corr_v0 velocity correction of particle
     * @param corr_v velocity corrections of tet particles
     */
    static bool VelocitySolveParticleTetContactConstraint(
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
            Vector3r corr_v[]);
};
}  // namespace vox::force
