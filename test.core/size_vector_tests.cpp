//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "tests.h"
#include "vox.core/size_vector.h"

namespace vox::tests {

TEST(DynamicSizeVector, Constructor) {
    core::DynamicSizeVector dsv{std::nullopt, 3};
    EXPECT_FALSE(dsv[0].has_value());
    EXPECT_EQ(dsv[1].value(), 3);
}

TEST(DynamicSizeVector, IsCompatible) {
    EXPECT_TRUE(core::SizeVector({}).IsCompatible({}));
    EXPECT_FALSE(core::SizeVector({}).IsCompatible({std::nullopt}));
    EXPECT_TRUE(core::SizeVector({10, 3}).IsCompatible({std::nullopt, 3}));
    EXPECT_FALSE(core::SizeVector({10, 3}).IsCompatible({std::nullopt, 5}));
}

}  // namespace vox::tests
