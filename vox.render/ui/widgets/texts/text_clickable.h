//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef DIGITALVOX_VOX_RENDER_UI_WIDGETS_TEXTS_TEXT_CLICKABLE_H_
#define DIGITALVOX_VOX_RENDER_UI_WIDGETS_TEXTS_TEXT_CLICKABLE_H_

#include "text.h"
#include "event.h"

namespace vox::ui {
/**
 * Widget to display text on a panel that is also clickable
 */
class TextClickable : public Text {
public:
    /**
     * Constructor
     * @param p_content p_content
     */
    explicit TextClickable(const std::string &p_content = "");
    
protected:
    void draw_impl() override;
    
public:
    Event<> clicked_event_;
    Event<> double_clicked_event_;
};

}
#endif /* DIGITALVOX_VOX_RENDER_UI_WIDGETS_TEXTS_TEXT_CLICKABLE_H_ */
