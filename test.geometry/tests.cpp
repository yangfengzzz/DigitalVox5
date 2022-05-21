//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "tests.h"

namespace vox::tests {

void NotImplemented() {
    std::cout << "\033[0;32m"
              << "[          ] "
              << "\033[0;0m";
    std::cout << "\033[0;31m"
              << "Not implemented."
              << "\033[0;0m" << std::endl;

    GTEST_NONFATAL_FAILURE_("Not implemented");
}

}  // namespace vox::tests
