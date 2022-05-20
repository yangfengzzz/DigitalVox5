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
#include <vector>

#include "file_dialog.h"

struct GLFWwindow;

namespace arc::visualization::gui {

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

}  // namespace arc::visualization::gui
