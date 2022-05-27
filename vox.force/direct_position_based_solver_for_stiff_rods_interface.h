//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.force/common.h"

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
class RodSegment {
public:
    virtual bool isDynamic() = 0;
    virtual Real Mass() = 0;
    virtual const Vector3r &InertiaTensor() = 0;
    virtual const Vector3r &Position() = 0;
    virtual const Quaternionr &Rotation() = 0;
};

class RodConstraint {
public:
    using Vector6r = Eigen::Matrix<Real, 6, 1, Eigen::DontAlign>;
    virtual unsigned int segmentIndex(unsigned int i) = 0;
    virtual Eigen::Matrix<Real, 3, 4, Eigen::DontAlign> &getConstraintInfo() = 0;
    virtual Real getAverageSegmentLength() = 0;
    virtual Vector3r &getRestDarbouxVector() = 0;
    virtual Vector3r &getStiffnessCoefficientK() = 0;
    virtual Vector3r &getStretchCompliance() = 0;
    virtual Vector3r &getBendingAndTorsionCompliance() = 0;
};
}  // namespace vox::force
