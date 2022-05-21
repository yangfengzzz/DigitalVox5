//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "device.h"
#include "tests.h"

namespace vox {
namespace tests {

TEST(Device, DefaultConstructor) {
    core::Device ctx;
    EXPECT_EQ(ctx.GetType(), core::Device::DeviceType::CPU);
    EXPECT_EQ(ctx.GetID(), 0);
}

TEST(Device, CPUMustBeID0) {
    EXPECT_EQ(core::Device(core::Device::DeviceType::CPU, 0).GetID(), 0);
    EXPECT_THROW(core::Device(core::Device::DeviceType::CPU, 1), std::runtime_error);
}

TEST(Device, SpecifiedConstructor) {
    core::Device ctx(core::Device::DeviceType::CUDA, 1);
    EXPECT_EQ(ctx.GetType(), core::Device::DeviceType::CUDA);
    EXPECT_EQ(ctx.GetID(), 1);
}

TEST(Device, StringConstructor) {
    core::Device ctx("CUDA:1");
    EXPECT_EQ(ctx.GetType(), core::Device::DeviceType::CUDA);
    EXPECT_EQ(ctx.GetID(), 1);
}

TEST(Device, StringConstructorLower) {
    core::Device ctx("cuda:1");
    EXPECT_EQ(ctx.GetType(), core::Device::DeviceType::CUDA);
    EXPECT_EQ(ctx.GetID(), 1);
}

}  // namespace tests
}  // namespace vox
