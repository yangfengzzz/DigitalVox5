//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "button.h"
#include "vector2.h"
#include "color.h"

namespace vox::ui {
/**
 * Button widget of a single color (Color palette element)
 */
class ButtonColored : public Button {
public:
    explicit ButtonColored(std::string label = "", const Color &color = {},
                           const Vector2F &size = Vector2F(0.f, 0.f), bool enable_alpha = true);
    
protected:
    void draw_impl() override;
    
public:
    std::string label_;
    Color color_;
    Vector2F size_;
    bool enableAlpha_;
};

}
