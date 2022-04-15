//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef DIGITALVOX_VOX_RENDER_UI_WIDGETS_MENU_MENU_LIST_H_
#define DIGITALVOX_VOX_RENDER_UI_WIDGETS_MENU_MENU_LIST_H_

#include "ui/widgets/layout/group.h"
#include "event.h"

namespace vox::ui {
/**
 * Widget that behave like a group with a menu display
 */
class MenuList : public Group {
public:
    /**
     * Constructor
     * @param p_name p_name
     * @param p_locked p_locked
     */
    explicit MenuList(std::string p_name, bool p_locked = false);
    
protected:
    void draw_impl() override;
    
public:
    std::string name_;
    bool locked_;
    Event<> clicked_event_;
    
private:
    bool opened_{};
};

}
#endif /* DIGITALVOX_VOX_RENDER_UI_WIDGETS_MENU_MENU_LIST_H_ */
