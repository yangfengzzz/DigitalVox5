//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

namespace vox::compute {
#define COLLISION_CAPSULES 1
#define MAX_NUM_COLLISION_CAPSULES 8
#define SIM_THREAD_GROUP_SIZE 64
#define MAX_NUM_BONES 512
#define MAX_HAIR_GROUP_RENDER 16

class NonCopyable {
public:
    NonCopyable() = default;
    ~NonCopyable() = default;

    NonCopyable(const NonCopyable&) = delete;
    void operator=(const NonCopyable&) = delete;
};

}  // namespace vox::compute