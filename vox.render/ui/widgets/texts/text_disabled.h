//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "text.h"

namespace vox::ui {
/**
 * Widget to display a disabled text on a panel
 */
class TextDisabled : public Text {
public:
    explicit TextDisabled(const std::string &content = "");

protected:
    void draw_impl() override;
};

}  // namespace vox::ui
