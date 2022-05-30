//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.force/common.h"

// ------------------------------------------------------------------------------------
namespace vox::force {
class MathFunctions {
private:
    static void JacobiRotate(Matrix3r &A, Matrix3r &R, int p, int q);

public:
    static Real InfNorm(const Matrix3r &A);
    static Real OneNorm(const Matrix3r &A);

    static void EigenDecomposition(const Matrix3r &A, Matrix3r &eigen_vecs, Vector3r &eigen_vals);

    static void PolarDecomposition(const Matrix3r &A, Matrix3r &R, Matrix3r &U, Matrix3r &D);

    static void PolarDecompositionStable(const Matrix3r &M, Real tolerance, Matrix3r &R);

    static void SvdWithInversionHandling(const Matrix3r &A, Vector3r &sigma, Matrix3r &U, Matrix3r &VT);

    static Real CotTheta(const Vector3r &v, const Vector3r &w);

    /** Computes the cross product matrix of a vector.
     * @param  v		input vector
     * @param  v_hat	resulting cross product matrix
     */
    static void CrossProductMatrix(const Vector3r &v, Matrix3r &v_hat);

    /** Implementation of the paper: \n
     * Matthias Muller, Jan Bender, Nuttapong Chentanez and Miles Macklin,
     * "A Robust Method to Extract the Rotational Part of Deformations",
     * ACM SIGGRAPH Motion in Games, 2016
     */
    static void ExtractRotation(const Matrix3r &A, Quaternionr &q, unsigned int max_iter);
};
}  // namespace vox::force