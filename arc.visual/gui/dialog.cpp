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

#include "gui/dialog.h"

#include <string>

namespace arc::visualization::gui {

struct Dialog::Impl {
    std::string title;
    Window *parent = nullptr;
};

Dialog::Dialog(const char *title) : impl_(new Dialog::Impl()) {}

Dialog::~Dialog() = default;

Size Dialog::CalcPreferredSize(const LayoutContext &context, const Constraints &constraints) const {
    if (GetChildren().size() == 1) {
        auto child = GetChildren()[0];
        return child->CalcPreferredSize(context, constraints);
    } else {
        return Super::CalcPreferredSize(context, constraints);
    }
}

void Dialog::Layout(const LayoutContext &context) {
    if (GetChildren().size() == 1) {
        auto child = GetChildren()[0];
        child->SetFrame(GetFrame());
        child->Layout(context);
    } else {
        Super::Layout(context);
    }
}

void Dialog::OnWillShow() {}

Widget::DrawResult Dialog::Draw(const DrawContext &context) { return Super::Draw(context); }

}  // namespace arc::visualization::gui
