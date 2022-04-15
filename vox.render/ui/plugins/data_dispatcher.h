//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef DIGITALVOX_VOX_RENDER_UI_PLUGINS_DATA_DISPATCHER_H_
#define DIGITALVOX_VOX_RENDER_UI_PLUGINS_DATA_DISPATCHER_H_

#include <functional>

#include "ui/plugins/plugin.h"

namespace vox::ui {
#define TRY_GATHER(type, output)   if (auto plugin = get_plugin<DataDispatcher<type>>(); plugin) output = plugin->gather();
#define TRY_PROVIDE(type, output)  if (auto plugin = get_plugin<DataDispatcher<type>>(); plugin) plugin->provide(output);
#define TRY_NOTIFY_CHANGE(type)    if (auto plugin = get_plugin<DataDispatcher<type>>(); plugin) plugin->notify_change();

/**
 * Plugin that allow automatic data update of any DataWidget
 */
template<typename T>
class DataDispatcher : public Plugin {
public:
    /**
     * Register a reference
     * @param p_reference p_reference
     */
    void register_reference(T &p_reference) {
        data_pointer_ = &p_reference;
    }
    
    /**
     * Register a provider (Which function should be called when the widget data is modified)
     * @param p_provider p_provider
     */
    void register_provider(std::function<void(T)> p_provider) {
        provider_ = p_provider;
    }
    
    /**
     * Register a gather (Which function should be called when the widget data needs to be updated)
     * @param p_gatherer p_gatherer
     */
    void register_gatherer(std::function<T(void)> p_gatherer) {
        gatherer_ = p_gatherer;
    }
    
    /**
     * Provide data to the dispatcher
     * @param p_data p_data
     */
    void provide(T p_data) {
        if (value_changed_) {
            if (data_pointer_)
                *data_pointer_ = p_data;
            else
                provider_(p_data);
            
            value_changed_ = false;
        }
    }
    
    /**
     * Notify that a change occurred
     */
    void notify_change() {
        value_changed_ = true;
    }
    
    /**
     * Returns the data from the dispatcher
     */
    T gather() {
        return data_pointer_ ? *data_pointer_ : gatherer_();
    }
    
    /**
     * Execute the data dispatcher behaviour (No effect)
     */
    void execute() override {
    }
    
private:
    bool value_changed_ = false;
    T *data_pointer_ = nullptr;
    std::function<void(T)> provider_;
    std::function<T(void)> gatherer_;
};

}
#endif /* DIGITALVOX_VOX_RENDER_UI_PLUGINS_DATA_DISPATCHER_H_ */
