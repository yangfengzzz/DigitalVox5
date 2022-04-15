//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef DIGITALVOX_VOX_RENDER_UI_WIDGETS_TEXTS_TEXT_SELECTABLE_H_
#define DIGITALVOX_VOX_RENDER_UI_WIDGETS_TEXTS_TEXT_SELECTABLE_H_

#include "text.h"

namespace vox::ui {
/**
 * Simple widget to display a selectable text on a panel
 */
class TextSelectable : public Text {
public:
    /**
     * Constructor
     * @param p_content p_content
     * @param p_selected p_selected
     * @param p_disabled p_disabled
     */
    explicit TextSelectable(const std::string &p_content = "",
                            bool p_selected = false, bool p_disabled = false);
    
protected:
    void draw_impl() override;
    
public:
    bool selected_;
    bool disabled_;
    
    Event<bool> clicked_event_;
    Event<> selected_event_;
    Event<> unselected_event_;
};

}
#endif /* DIGITALVOX_VOX_RENDER_UI_WIDGETS_TEXTS_TEXT_SELECTABLE_H_ */
