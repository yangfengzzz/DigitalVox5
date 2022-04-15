//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef DIGITALVOX_VOX_RENDER_UI_WIDGETS_LAYOUT_GROUP_COLLAPSABLE_H_
#define DIGITALVOX_VOX_RENDER_UI_WIDGETS_LAYOUT_GROUP_COLLAPSABLE_H_

#include "group.h"
#include "event.h"

namespace vox::ui {
/**
 * Widget that can contains other widgets and is collapsable
 */
class GroupCollapsable : public Group {
public:
    /**
     * Constructor
     */
    explicit GroupCollapsable(std::string p_name = "");
    
protected:
    void draw_impl() override;
    
public:
    std::string name_;
    bool closable_ = false;
    bool opened_ = true;
    Event<> close_event_;
    Event<> open_event_;
};

}
#endif /* DIGITALVOX_VOX_RENDER_UI_WIDGETS_LAYOUT_GROUP_COLLAPSABLE_H_ */
