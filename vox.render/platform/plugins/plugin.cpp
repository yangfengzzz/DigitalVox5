//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "plugin.h"
#include "platform/platform.h"

namespace vox {
std::vector<Plugin *> associate_plugins(const std::vector<Plugin *> &plugins) {
    for (auto *plugin: plugins) {
        for (auto *comparison_plugin: plugins) {
            bool full_control = comparison_plugin->has_tags<tags::FullControl>();
            bool stopping = comparison_plugin->has_tags<tags::Stopping>();
            bool controlling = full_control || stopping;
            
            bool entrypoint = comparison_plugin->has_tags<tags::Entrypoint>();
            
            if (plugin->has_tag<tags::FullControl>() && (controlling || entrypoint)) {
                plugin->excludes(comparison_plugin);
                continue;
            }
            
            if (plugin->has_tag<tags::Stopping>() && stopping) {
                plugin->excludes(comparison_plugin);
                continue;
            }
            
            if (plugin->has_tag<tags::Entrypoint>() && entrypoint) {
                plugin->excludes(comparison_plugin);
                continue;
            }
            
            plugin->includes(comparison_plugin);
        }
    }
    
    return plugins;
}

bool Plugin::activate_plugin(Platform *p, const CommandParser &parser, bool force_activation) {
    platform_ = p;
    
    bool active = is_active(parser);
    
    // Plugin activated
    if (force_activation || active) {
        init(parser);
    }
    
    return active;
}

const std::string &Plugin::get_name() const {
    return name_;
}

const std::string &Plugin::get_description() const {
    return description_;
}

void Plugin::excludes(Plugin *plugin) {
    exclusions_.push_back(plugin);
}

const std::vector<Plugin *> &Plugin::get_exclusions() const {
    return exclusions_;
}

void Plugin::includes(Plugin *plugin) {
    inclusions_.push_back(plugin);
}

const std::vector<Plugin *> &Plugin::get_inclusions() const {
    return inclusions_;
}

}        // namespace vox
