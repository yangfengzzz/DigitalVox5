//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef DIGITALVOX_VOX_RENDER_UI_PLUGINS_CONTEXTUAL_MENU_H_
#define DIGITALVOX_VOX_RENDER_UI_PLUGINS_CONTEXTUAL_MENU_H_

#include "ui/plugins/plugin.h"
#include "ui/widgets/widget_container.h"
#include "ui/widgets/menu/menu_list.h"
#include "ui/widgets/menu/menu_item.h"

namespace vox::ui {
/**
 * A simple plugin that will show a contextual menu on right click
 * You can add widgets to a contextual menu
 */
class ContextualMenu : public Plugin, public WidgetContainer {
public:
    /**
     * Execute the plugin
     */
    void execute() override;
    
    /**
     * Force close the contextual menu
     */
    static void close();
};

}
#endif /* DIGITALVOX_VOX_RENDER_UI_PLUGINS_CONTEXTUAL_MENU_H_ */