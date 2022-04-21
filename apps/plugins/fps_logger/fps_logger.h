//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "platform/plugins/plugin_base.h"
namespace plugins {
using FpsLoggerTags = vox::PluginBase<vox::tags::Passive>;

/**
 * @brief FPS Logger
 *
 * Control when FPS should be logged. Declutters the log output by removing FPS logs when not enabled
 *
 * Usage: vulkan_sample sample afbc --log-fps
 *
 */
class FpsLogger : public FpsLoggerTags {
public:
    FpsLogger();
    
    virtual ~FpsLogger() = default;
    
    virtual bool is_active(const vox::CommandParser &parser) override;
    
    virtual void init(const vox::CommandParser &parser) override;
    
    void on_update(float delta_time) override;
    
    vox::FlagCommand fps_flag = {vox::FlagType::FLAG_ONLY, "log-fps", "", "Log FPS"};
    
private:
    vox::Timer timer;
    
    size_t frame_count{0};
    
    size_t last_frame_count{0};
};
}        // namespace plugins
