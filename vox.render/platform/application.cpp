//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "application.h"

#include "logging.h"
#include "platform/platform.h"

namespace vox {
Application::Application() :
name_{"Sample Name"} {
}

bool Application::prepare(Platform &platform) {
    auto &debug_info = get_debug_info();
    debug_info.insert<field::MinMax, float>("fps", fps_);
    debug_info.insert<field::MinMax, float>("frame_time", frame_time_);
    
    platform_ = &platform;
    
    return true;
}

void Application::finish() {
}

bool Application::resize(const uint32_t /*width*/, const uint32_t /*height*/) {
    return true;
}

void Application::input_event(const InputEvent &input_event) {
}

void Application::update(float delta_time) {
    fps_ = 1.0f / delta_time;
    frame_time_ = delta_time * 1000.0f;
}

const std::string &Application::get_name() const {
    return name_;
}

void Application::set_name(const std::string &name) {
    name_ = name;
}

DebugInfo &Application::get_debug_info() {
    return debug_info_;
}


}        // namespace vox
