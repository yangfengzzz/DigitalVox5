//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "text.h"
#include "event.h"

namespace vox::ui {
/**
 * Widget to display text on a panel that is also clickable
 */
class TextClickable : public Text {
public:
    explicit TextClickable(const std::string &content = "");
    
protected:
    void draw_impl() override;
    
public:
    Event<> clicked_event_;
    Event<> double_clicked_event_;
};

}
