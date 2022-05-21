//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifdef BUILD_CUDA_MODULE

#include <thread>
#include <vector>

#include "cuda_utils.h"
#include "tests.h"

namespace vox {
namespace tests {

TEST(CUDAUtils, InitState) {
    const int device_count = core::cuda::DeviceCount();
    const core::CUDAState& cuda_state = core::CUDAState::GetInstance();
    utility::LogInfo("Number of CUDA devices: {}", device_count);
    for (int i = 0; i < device_count; ++i) {
        for (int j = 0; j < device_count; ++j) {
            utility::LogInfo("P2PEnabled {}->{}: {}", i, j, cuda_state.IsP2PEnabled(i, j));
        }
    }
}

void CheckScopedStreamManually() {
    int current_device = core::cuda::GetDevice();

    ASSERT_EQ(core::cuda::GetStream(), core::cuda::GetDefaultStream());
    ASSERT_EQ(core::cuda::GetDevice(), current_device);

    cudaStream_t stream;
    OPEN3D_CUDA_CHECK(cudaStreamCreate(&stream));

    {
        core::CUDAScopedStream scoped_stream(stream);

        ASSERT_EQ(core::cuda::GetStream(), stream);
        ASSERT_NE(core::cuda::GetStream(), core::cuda::GetDefaultStream());
        ASSERT_EQ(core::cuda::GetDevice(), current_device);
    }

    OPEN3D_CUDA_CHECK(cudaStreamDestroy(stream));

    ASSERT_EQ(core::cuda::GetStream(), core::cuda::GetDefaultStream());
    ASSERT_EQ(core::cuda::GetDevice(), current_device);
}

void CheckScopedStreamAutomatically() {
    int current_device = core::cuda::GetDevice();

    ASSERT_EQ(core::cuda::GetStream(), core::cuda::GetDefaultStream());
    ASSERT_EQ(core::cuda::GetDevice(), current_device);

    {
        core::CUDAScopedStream scoped_stream(core::CUDAScopedStream::CreateNewStream);

        ASSERT_NE(core::cuda::GetStream(), core::cuda::GetDefaultStream());
        ASSERT_EQ(core::cuda::GetDevice(), current_device);
    }

    ASSERT_EQ(core::cuda::GetStream(), core::cuda::GetDefaultStream());
    ASSERT_EQ(core::cuda::GetDevice(), current_device);
}

void CheckScopedStreamMultiThreaded(const std::function<void()>& func) {
    std::vector<std::thread> threads;

    // NVCC does not like capturing const int by reference on Windows, use int.
    int kIterations = 100000;
    int kThreads = 8;
    for (int i = 0; i < kThreads; ++i) {
        threads.emplace_back([&kIterations, &func]() {
            utility::LogDebug("Starting thread with ID {}", std::this_thread::get_id());
            for (int i = 0; i < kIterations; ++i) {
                func();
            }
        });
    }

    for (auto& thread : threads) {
        if (thread.joinable()) {
            utility::LogDebug("Joining thread with ID {}", thread.get_id());
            thread.join();
        }
    }
}

TEST(CUDAUtils, ScopedStreamManually) { CheckScopedStreamManually(); }

TEST(CUDAUtils, ScopedStreamManuallyMultiThreaded) { CheckScopedStreamMultiThreaded(&CheckScopedStreamManually); }

TEST(CUDAUtils, ScopedStreamAutomatically) { CheckScopedStreamAutomatically(); }

TEST(CUDAUtils, ScopedStreamAutomaticallyMultiThreaded) {
    CheckScopedStreamMultiThreaded(&CheckScopedStreamAutomatically);
}

}  // namespace tests
}  // namespace vox

#endif
