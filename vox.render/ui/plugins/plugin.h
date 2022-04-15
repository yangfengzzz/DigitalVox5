//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef DIGITALVOX_VOX_RENDER_UI_PLUGINS_PLUGIN_H_
#define DIGITALVOX_VOX_RENDER_UI_PLUGINS_PLUGIN_H_

namespace vox::ui {
/**
 * Interface to any plugin of UI.
 * A plugin is basically a behaviour that you can plug to a widget
 */
class Plugin {
public:
    virtual ~Plugin() = default;
    
    /**
     * Execute the plugin behaviour
     */
    virtual void execute() = 0;
    
    /* Feel free to store any data you want here */
    void *user_data_ = nullptr;
};

}
#endif /* DIGITALVOX_VOX_RENDER_UI_PLUGINS_PLUGIN_H_ */
