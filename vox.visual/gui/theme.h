//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <string>

#include "vox.math/color.h"
#include "vox.visual/gui/gui.h"
#include "vox.visual/gui/widget.h"

namespace vox::visualization::gui {

struct Theme {
    vox::Color background_color;

    std::string font_path;
    std::string font_bold_path;
    std::string font_italic_path;
    std::string font_bold_italic_path;
    std::string font_mono_path;
    int font_size;
    int default_margin;
    int default_layout_spacing;
    vox::Color text_color;

    int border_width;
    int border_radius;
    vox::Color border_color;

    vox::Color menubar_border_color;

    vox::Color button_color;
    vox::Color button_hover_color;
    vox::Color button_active_color;
    vox::Color button_on_color;
    vox::Color button_on_hover_color;
    vox::Color button_on_active_color;
    vox::Color button_on_text_color;

    vox::Color checkbox_background_off_color;
    vox::Color checkbox_background_on_color;
    vox::Color checkbox_background_hover_off_color;
    vox::Color checkbox_background_hover_on_color;
    vox::Color checkbox_check_color;

    vox::Color radiobtn_background_off_color;
    vox::Color radiobtn_background_on_color;
    vox::Color radiobtn_background_hover_off_color;
    vox::Color radiobtn_background_hover_on_color;

    vox::Color toggle_background_off_color;
    vox::Color toggle_background_on_color;
    vox::Color toggle_background_hover_off_color;
    vox::Color toggle_background_hover_on_color;
    vox::Color toggle_thumb_color;

    vox::Color combobox_background_color;
    vox::Color combobox_hover_color;
    vox::Color combobox_arrow_background_color;

    vox::Color slider_grab_color;

    vox::Color text_edit_background_color;

    vox::Color list_background_color;
    vox::Color list_hover_color;
    vox::Color list_selected_color;

    vox::Color tree_background_color;
    vox::Color tree_selected_color;

    vox::Color tab_inactive_color;
    vox::Color tab_hover_color;
    vox::Color tab_active_color;

    int dialog_border_width;
    int dialog_border_radius;
};

}  // namespace vox::visualization::gui
