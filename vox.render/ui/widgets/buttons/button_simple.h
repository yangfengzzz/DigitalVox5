//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "button.h"
#include "color.h"
#include "vector2.h"

namespace vox::ui {
/**
 * Simple button widget
 */
class ButtonSimple : public Button {
public:
    explicit ButtonSimple(std::string label = "", const Vector2F &size = Vector2F(0.f, 0.f), bool disabled = false);

protected:
    void draw_impl() override;

public:
    std::string label_;
    Vector2F size_;
    bool disabled_ = false;

    Color idle_background_color_;
    Color hovered_background_color_;
    Color clicked_background_color_;

    Color text_color_;
};

}  // namespace vox::ui
