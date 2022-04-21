//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "platform/plugins/plugin_base.h"

namespace plugins {
using StopAfterTags = vox::PluginBase<vox::tags::Stopping>;

/**
 * @brief Stop After
 *
 * Stop the execution of the app after a specific frame.
 *
 * Usage: vulkan_sample sample afbc --stop-after-frame 100
 *
 * TODO: Add stop after duration
 *
 */
class StopAfter : public StopAfterTags {
public:
    StopAfter();
    
    virtual ~StopAfter() = default;
    
    virtual bool is_active(const vox::CommandParser &parser) override;
    
    virtual void init(const vox::CommandParser &parser) override;
    
    virtual void on_update(float delta_time) override;
    
    vox::FlagCommand
    stop_after_frame_flag = {vox::FlagType::ONE_VALUE, "stop-after-frame", "", "Stop the application after a certain number of frames"};
    
private:
    uint32_t remaining_frames{0};
};
}        // namespace plugins
