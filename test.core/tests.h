//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <gtest/gtest.h>

#include <Eigen/Core>
#include <sstream>
#include <string>
#include <vector>

//#include "open3d/Macro.h"
//#include "open3d/data/Dataset.h"
#include "compare.h"
#include "print.h"
#include "rand.h"
#include "raw.h"
#include "sort.h"

namespace vox::tests {

// Eigen Zero()
const Eigen::Vector2d Zero2d = Eigen::Vector2d::Zero();
const Eigen::Vector3d Zero3d = Eigen::Vector3d::Zero();
const Eigen::Matrix<double, 6, 1> Zero6d = Eigen::Matrix<double, 6, 1>::Zero();
const Eigen::Vector2i Zero2i = Eigen::Vector2i::Zero();

// Mechanism for reporting unit tests for which there is no implementation yet.
void NotImplemented();

}  // namespace vox::tests
