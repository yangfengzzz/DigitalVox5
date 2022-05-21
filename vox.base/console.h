//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <Eigen/Core>
#include <string>
#include <vector>

namespace vox::utility {

std::string GetProgramOptionAsString(int argc,
                                     char **argv,
                                     const std::string &option,
                                     const std::string &default_value = "");

int GetProgramOptionAsInt(int argc, char **argv, const std::string &option, int default_value = 0);

double GetProgramOptionAsDouble(int argc, char **argv, const std::string &option, double default_value = 0.0);

Eigen::VectorXd GetProgramOptionAsEigenVectorXd(int argc,
                                                char **argv,
                                                const std::string &option,
                                                const Eigen::VectorXd &default_value = Eigen::VectorXd::Zero(0));

bool ProgramOptionExists(int argc, char **argv, const std::string &option);

bool ProgramOptionExistsAny(int argc, char **argv, const std::vector<std::string> &options);

}  // namespace vox::utility
