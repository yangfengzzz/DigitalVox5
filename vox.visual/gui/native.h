//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <string>
#include <vector>

#include "vox.visual/gui/file_dialog.h"

struct GLFWwindow;

namespace vox::visualization::gui {

void* GetNativeDrawable(GLFWwindow* glfw_window);
// Note that Windows cannot post an expose event so it must draw immediately.
// Therefore this function cannot be called while drawing.
void PostNativeExposeEvent(GLFWwindow* glfw_window);
void ShowNativeAlert(const char* message);

#ifdef __APPLE__
void MacTransformIntoApp();
void SetNativeMenubar(void* menubar);
#endif  // __APPLE__

#if defined(__APPLE__) || defined(_WIN32)
void ShowNativeFileDialog(FileDialog::Mode mode,
                          const std::string& path,
                          const std::vector<std::pair<std::string, std::string>>& filters,
                          std::function<void(const char*)> on_ok,
                          std::function<void()> on_cancel);
#endif  // __APPLE__ || _WIN32

}  // namespace vox::visualization::gui
