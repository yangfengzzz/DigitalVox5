//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "ui/widgets/data_widget.h"
#include "event.h"
#include "color.h"

namespace vox::ui {
/**
 * Widget that can open a color picker on click
 */
class ColorEdit : public DataWidget<Color> {
public:
    explicit ColorEdit(bool enable_alpha = false, const Color &default_color = {});
    
protected:
    void draw_impl() override;
    
public:
    bool enable_alpha_;
    Color color_;
    Event<Color &> color_changed_event_;
};

}
