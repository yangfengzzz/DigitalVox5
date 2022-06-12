//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.compute/common.h"

namespace vox::compute {
// Implementation of "Direct Position-Based Solver for Stiff Rods" paper
// (https://animation.rwth-aachen.de/publication/0557/)
//
class RodSegment {
public:
    virtual bool IsDynamic() = 0;
    virtual Real Mass() = 0;
    virtual const Vector3r &InertiaTensor() = 0;
    virtual const Vector3r &Position() = 0;
    virtual const Quaternionr &Rotation() = 0;
};

class RodConstraint {
public:
    using Vector6r = Eigen::Matrix<Real, 6, 1, Eigen::DontAlign>;
    virtual unsigned int SegmentIndex(unsigned int i) = 0;
    virtual Eigen::Matrix<Real, 3, 4, Eigen::DontAlign> &GetConstraintInfo() = 0;
    virtual Real GetAverageSegmentLength() = 0;
    virtual Vector3r &GetRestDarbouxVector() = 0;
    virtual Vector3r &GetStiffnessCoefficientK() = 0;
    virtual Vector3r &GetStretchCompliance() = 0;
    virtual Vector3r &GetBendingAndTorsionCompliance() = 0;
};
}  // namespace vox::compute
