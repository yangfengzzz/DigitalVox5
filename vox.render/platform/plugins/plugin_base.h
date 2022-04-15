//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <spdlog/fmt/fmt.h>

#include <utility>

#include "tags.h"
#include "platform/parser.h"
#include "platform/platform.h"
#include "plugin.h"

namespace vox {
/**
 * @brief PluginBase is the base class that plugins inherit from. The class enforces the use of tags when creating new plugins.
 * 		  For method information see Plugin
 */
template<typename... TAGS>
class PluginBase : public Plugin, public Tag<TAGS...> {
public:
    PluginBase(std::string name, std::string description, std::vector<Hook> hooks = {},
               std::vector<Command *> commands = {});
    
    ~PluginBase() override = default;
    
    [[nodiscard]] const std::vector<Command *> &get_cli_commands() const override;
    
    [[nodiscard]] const std::vector<Hook> &get_hooks() const override;
    
    bool has_tag(TagID id) const override;
    
    // hooks that can be implemented by plugins
    void on_update(float delta_time) override {};
    
    void on_app_start(const std::string &app_id) override {};
    
    void on_app_close(const std::string &app_id) override {};
    
    void on_platform_close() override {};
    
    void on_post_draw(RenderContext &context) override {};
    
    void on_app_error(const std::string &app_id) override {};
    
private:
    Tag<TAGS...> *tags_ = reinterpret_cast<Tag<TAGS...> *>(this);
    
    std::vector<Hook> hooks_;
    std::vector<Command *> commands_;
};

template<typename... TAGS>
PluginBase<TAGS...>::PluginBase(const std::string name, const std::string description,
                                std::vector<Hook> hooks, std::vector<Command *> commands) :
Plugin(name, description), hooks_{std::move(hooks)}, commands_{std::move(commands)} {
}

template<typename... TAGS>
const std::vector<Command *> &PluginBase<TAGS...>::get_cli_commands() const {
    return commands_;
}

template<typename... TAGS>
bool PluginBase<TAGS...>::has_tag(TagID id) const {
    return tags_->has_tag(id);
}

template<typename... TAGS>
const std::vector<Hook> &PluginBase<TAGS...>::get_hooks() const {
    return hooks_;
}

}        // namespace vox
