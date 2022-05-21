//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once
// These are internal helper functions

#include <imgui.h>

#include <string>
#include <vector>

#include "color.h"
#include "gui/gui.h"

namespace vox::visualization::gui {
// These functions are here, because ImVec4 requires imgui.h, and can't be
// forward-declared because we need to know the size, since it is a return
// value. Since imgui.h is an implementation detail, we can't put this function
// in Color or it would pull in imgui.h pretty much everywhere that gui is used.
ImVec4 colorToImgui(const vox::Color& color);
uint32_t colorToImguiRGBA(const vox::Color& color);

std::string FindFontPath(std::string font, FontStyle style);

}  // namespace vox::visualization::gui
