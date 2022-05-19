// ----------------------------------------------------------------------------
// -                        Open3D: www.open3d.org                            -
// ----------------------------------------------------------------------------
// The MIT License (MIT)
//
// Copyright (c) 2018-2021 www.open3d.org
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
// ----------------------------------------------------------------------------

#include "device.h"
#include "tests.h"

namespace arc {
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
}  // namespace arc
