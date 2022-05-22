//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "tests.h"
#include "vox.base/helper.h"

#ifdef BUILD_ISPC_MODULE
#include "Helper_ispc.h"
#endif

namespace vox::tests {

TEST(Helper, JoinStrings) {
    std::vector<std::string> strings;

    strings = {"a", "b", "c"};
    EXPECT_EQ(utility::JoinStrings(strings), "a, b, c");
    EXPECT_EQ(utility::JoinStrings(strings, "-"), "a-b-c");

    strings = {};
    EXPECT_EQ(utility::JoinStrings(strings), "");
    EXPECT_EQ(utility::JoinStrings(strings, "-"), "");
}

TEST(Helper, StringStartsWith) {
    EXPECT_TRUE(utility::StringStartsWith("abc", "a"));
    EXPECT_TRUE(utility::StringStartsWith("abc", "ab"));
    EXPECT_TRUE(utility::StringStartsWith("abc", "abc"));
    EXPECT_FALSE(utility::StringStartsWith("abc", "abcd"));
    EXPECT_TRUE(utility::StringStartsWith("abc", ""));
    EXPECT_FALSE(utility::StringStartsWith("", "a"));
}

TEST(Helper, StringEndsWith) {
    EXPECT_TRUE(utility::StringEndsWith("abc", "c"));
    EXPECT_TRUE(utility::StringEndsWith("abc", "bc"));
    EXPECT_TRUE(utility::StringEndsWith("abc", "abc"));
    EXPECT_FALSE(utility::StringEndsWith("abc", "abcd"));
    EXPECT_TRUE(utility::StringEndsWith("abc", ""));
    EXPECT_FALSE(utility::StringEndsWith("", "c"));
}

TEST(Helper, UniformRandIntGeneratorWithFixedSeed) {
    std::array<int, 1024> values{};
    utility::UniformRandIntGenerator rand_generator(0, 9, 42);
    for (auto it = values.begin(); it != values.end(); ++it) *it = rand_generator();

    for (int i = 0; i < 10; i++) {
        std::array<int, 1024> new_values{};
        utility::UniformRandIntGenerator new_rand_generator(0, 9, 42);
        for (auto it = new_values.begin(); it != new_values.end(); ++it) *it = new_rand_generator();
        EXPECT_TRUE(values == new_values);
    }
}

TEST(Helper, UniformRandIntGeneratorWithRandomSeed) {
    std::array<int, 1024> values{};
    utility::UniformRandIntGenerator rand_generator(0, 9);
    for (auto it = values.begin(); it != values.end(); ++it) *it = rand_generator();

    for (int i = 0; i < 10; i++) {
        std::array<int, 1024> new_values{};
        utility::UniformRandIntGenerator new_rand_generator(0, 9);
        for (auto it = new_values.begin(); it != new_values.end(); ++it) *it = new_rand_generator();
        EXPECT_FALSE(values == new_values);
    }
}

TEST(Helper, CHAR_BIT_constant) {
#ifdef BUILD_ISPC_MODULE
    int32_t value;
    ispc::GetCharBit(&value);

    EXPECT_EQ(value, CHAR_BIT);
#endif
}

TEST(Helper, ENSURE_EXPORTED) {
#ifdef BUILD_ISPC_MODULE
    ispc::NotAutomaticallyExportedStruct s;
    s.i = 1;
    s.b = 255;

    EXPECT_EQ(s.i, 1);
    EXPECT_EQ(s.b, 255);
#endif
}

}  // namespace vox::tests
