//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "benchmark_mode.h"

#include "platform/platform.h"

namespace plugins {
BenchmarkMode::BenchmarkMode() :
BenchmarkModeTags("Benchmark Mode",
                  "Log frame averages after running an app.",
                  {vox::Hook::ON_UPDATE, vox::Hook::ON_APP_START, vox::Hook::ON_APP_CLOSE},
                  {&benchmark_flag}) {
}

bool BenchmarkMode::is_active(const vox::CommandParser &parser) {
    return parser.contains(&benchmark_flag);
}

void BenchmarkMode::init(const vox::CommandParser &parser) {
    // Whilst in benchmark mode fix the fps so that separate runs are consistently simulated
    // This will effect the graph outputs of framerate
    platform_->force_simulation_fps(60.0f);
}

void BenchmarkMode::on_update(float delta_time) {
    elapsed_time += delta_time;
    total_frames++;
}

void BenchmarkMode::on_app_start(const std::string &app_id) {
    elapsed_time = 0;
    total_frames = 0;
    LOGI("Starting Benchmark for {}", app_id);
}

void BenchmarkMode::on_app_close(const std::string &app_id) {
    LOGI("Benchmark for {} completed in {} seconds (ran {} frames, averaged {} fps)",
         app_id,
         elapsed_time,
         total_frames,
         total_frames / elapsed_time);
}
}        // namespace plugins
