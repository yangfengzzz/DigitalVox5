//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.visual/gui/dialog.h"

#include <string>

namespace vox::visualization::gui {

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

}  // namespace vox::visualization::gui
