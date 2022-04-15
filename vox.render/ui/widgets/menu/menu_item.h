//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef DIGITALVOX_VOX_RENDER_UI_WIDGETS_MENU_MENU_ITEM_H_
#define DIGITALVOX_VOX_RENDER_UI_WIDGETS_MENU_MENU_ITEM_H_

#include "ui/widgets/data_widget.h"
#include "event.h"

namespace vox::ui {
/**
 * Widget that can be added to a menu list. It is clickable and can be checkable
 */
class MenuItem : public DataWidget<bool> {
public:
    /**
     * Constructor
     * @param p_name p_name
     * @param p_shortcut p_shortcut
     * @param p_checkable p_checkable
     * @param p_checked p_checked
     */
    explicit MenuItem(std::string p_name, std::string p_shortcut = "",
                      bool p_checkable = false, bool p_checked = false);
    
protected:
    void draw_impl() override;
    
public:
    std::string name_;
    std::string shortcut_;
    bool checkable_;
    bool checked_;
    Event<> clicked_event_;
    Event<bool> value_changed_event_;
    
private:
    bool selected_{};
};

}
#endif /* DIGITALVOX_VOX_RENDER_UI_WIDGETS_MENU_MENU_ITEM_H_ */
