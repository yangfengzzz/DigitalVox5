// ----------------------------------------------------------------------------
// -                        Open3D: www.open3d.org                            -
// ----------------------------------------------------------------------------
// The MIT License (MIT)
//
// Copyright (c) 2018-2021 www.open3d.org
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
// ----------------------------------------------------------------------------

#pragma once

#include <string>

#include "color.h"
#include "gui/gui.h"
#include "gui/widget.h"

namespace arc::visualization::gui {

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

}  // namespace arc::visualization::gui
