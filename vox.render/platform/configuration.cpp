//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.


#include "configuration.h"

namespace vox {
BoolSetting::BoolSetting(bool &handle, bool value) :
handle_{handle},
value_{value} {
}

void BoolSetting::set() {
    handle_ = value_;
}

std::type_index BoolSetting::get_type() {
    return typeid(BoolSetting);
}

IntSetting::IntSetting(int &handle, int value) :
handle_{handle},
value_{value} {
}

void IntSetting::set() {
    handle_ = value_;
}

std::type_index IntSetting::get_type() {
    return typeid(IntSetting);
}

void EmptySetting::set() {
}

std::type_index EmptySetting::get_type() {
    return typeid(EmptySetting);
}

void Configuration::set() {
    for (const auto &pair: current_configuration_->second) {
        for (auto setting: pair.second) {
            setting->set();
        }
    }
}

bool Configuration::next() {
    if (configs_.empty()) {
        return false;
    }
    
    current_configuration_++;
    
    if (current_configuration_ == configs_.end()) {
        return false;
    }
    
    return true;
}

void Configuration::reset() {
    current_configuration_ = configs_.begin();
}

void Configuration::insert_setting(uint32_t config_index, std::unique_ptr<Setting> setting) {
    settings_.push_back(std::move(setting));
    configs_[config_index][settings_.back()->get_type()].push_back(settings_.back().get());
}

}        // namespace vox
