//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "core_test.h"
#include "tests.h"
#include "vox.core/blob.h"
#include "vox.core/device.h"
#include "vox.core/memory_manager.h"

namespace vox::tests {

class BlobPermuteDevices : public PermuteDevices {};
INSTANTIATE_TEST_SUITE_P(Blob, BlobPermuteDevices, testing::ValuesIn(PermuteDevices::TestCases()));

TEST_P(BlobPermuteDevices, BlobConstructor) {
    core::Device device = GetParam();

    core::Blob b(10, core::Device(device));
}

TEST_P(BlobPermuteDevices, BlobConstructorWithExternalMemory) {
    core::Device device = GetParam();

    void* data_ptr = core::MemoryManager::Malloc(8, device);
    bool deleter_called = false;

    auto deleter = [&device, &deleter_called, data_ptr](void* dummy) -> void {
        core::MemoryManager::Free(data_ptr, device);
        deleter_called = true;
    };

    {
        core::Blob b(device, data_ptr, deleter);
        EXPECT_EQ(b.GetDataPtr(), data_ptr);
        EXPECT_FALSE(deleter_called);
    }
    EXPECT_TRUE(deleter_called);
}

}  // namespace vox::tests
