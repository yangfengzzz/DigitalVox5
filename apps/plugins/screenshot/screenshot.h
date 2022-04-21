//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "platform/filesystem.h"
#include "platform/plugins/plugin_base.h"

namespace plugins {
class Screenshot;

using ScreenshotTags = vox::PluginBase<Screenshot, vox::tags::Passive>;

/**
 * @brief Screenshot
 *
 * Capture a screen shot of the last rendered image at a given frame. The output can also be named
 *
 * Usage: vulkan_sample sample afbc --screenshot 1 --screenshot-output afbc-screenshot
 *
 */
class Screenshot : public ScreenshotTags {
public:
    Screenshot();
    
    virtual ~Screenshot() = default;
    
    virtual bool is_active(const vox::CommandParser &parser) override;
    
    virtual void init(const vox::CommandParser &parser) override;
    
    virtual void on_update(float delta_time) override;
    
    virtual void on_app_start(const std::string &app_info) override;
    
    virtual void on_post_draw(vox::RenderContext &context) override;
    
    vox::FlagCommand screenshot_flag = {vox::FlagType::ONE_VALUE, "screenshot", "", "Take a screenshot at a given frame"};
    vox::FlagCommand screenshot_output_flag = {vox::FlagType::ONE_VALUE, "screenshot-output", "", "Declare an output name for the image"};
    
private:
    uint32_t current_frame = 0;
    uint32_t frame_number;
    std::string current_app_name;
    
    bool output_path_set = false;
    std::string output_path;
};
}        // namespace plugins
