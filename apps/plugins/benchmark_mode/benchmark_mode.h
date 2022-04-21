//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "platform/plugins/plugin_base.h"

namespace plugins {
class BenchmarkMode;

using BenchmarkModeTags = vox::PluginBase<BenchmarkMode, vox::tags::Passive>;

/**
 * @brief Benchmark Mode
 *
 * When enabled frame time statistics of a samples run will be printed to the console when an application closes. The simulation frame time (delta time) is also locked to 60FPS so that statistics can be compared more accurately across different devices.
 *
 * Usage: vulkan_samples sample afbc --benchmark
 *
 */
class BenchmarkMode : public BenchmarkModeTags {
public:
    BenchmarkMode();
    
    virtual ~BenchmarkMode() = default;
    
    virtual bool is_active(const vox::CommandParser &parser) override;
    
    virtual void init(const vox::CommandParser &parser) override;
    
    virtual void on_update(float delta_time) override;
    
    virtual void on_app_start(const std::string &app_info) override;
    
    virtual void on_app_close(const std::string &app_info) override;
    
    vox::FlagCommand benchmark_flag = {vox::FlagType::FLAG_ONLY, "benchmark", "", "Enable benchmark mode"};
    
private:
    uint32_t total_frames{0};
    
    float elapsed_time{0.0f};
};
}        // namespace plugins
