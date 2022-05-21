//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "color.h"
#include "text.h"

namespace vox::ui {
/**
 * Widget to display text on a panel that can be colored
 */
class TextColored : public Text {
public:
    explicit TextColored(const std::string &content = "", const Color &color = Color(1.0f, 1.0f, 1.0f, 1.0f));

public:
    Color color_;

protected:
    void draw_impl() override;
};

}  // namespace vox::ui
