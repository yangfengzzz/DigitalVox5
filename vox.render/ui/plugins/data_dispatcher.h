//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <functional>

#include "ui/plugins/plugin.h"

namespace vox::ui {
#define TRY_GATHER(type, output) \
    if (auto plugin = get_plugin<DataDispatcher<type>>(); plugin) output = plugin->gather();
#define TRY_PROVIDE(type, output) \
    if (auto plugin = get_plugin<DataDispatcher<type>>(); plugin) plugin->provide(output);
#define TRY_NOTIFY_CHANGE(type) \
    if (auto plugin = get_plugin<DataDispatcher<type>>(); plugin) plugin->notify_change();

/**
 * Plugin that allow automatic data update of any DataWidget
 */
template <typename T>
class DataDispatcher : public Plugin {
public:
    /**
     * Register a reference
     */
    void register_reference(T &reference) { data_pointer_ = &reference; }

    /**
     * Register a provider (Which function should be called when the widget data is modified)
     */
    void register_provider(std::function<void(T)> provider) { provider_ = provider; }

    /**
     * Register a gather (Which function should be called when the widget data needs to be updated)
     */
    void register_gatherer(std::function<T(void)> gatherer) { gatherer_ = gatherer; }

    /**
     * Provide data to the dispatcher
     */
    void provide(T data) {
        if (value_changed_) {
            if (data_pointer_)
                *data_pointer_ = data;
            else
                provider_(data);

            value_changed_ = false;
        }
    }

    /**
     * Notify that a change occurred
     */
    void notify_change() { value_changed_ = true; }

    /**
     * Returns the data from the dispatcher
     */
    T gather() { return data_pointer_ ? *data_pointer_ : gatherer_(); }

    /**
     * Execute the data dispatcher behaviour (No effect)
     */
    void execute() override {}

private:
    bool value_changed_ = false;
    T *data_pointer_ = nullptr;
    std::function<void(T)> provider_;
    std::function<T(void)> gatherer_;
};

}  // namespace vox::ui
