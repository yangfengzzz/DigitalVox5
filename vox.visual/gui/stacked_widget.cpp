//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.visual/gui/stacked_widget.h"

#include <algorithm>  // for std::max, std::min

namespace vox {
namespace visualization {
namespace gui {

struct StackedWidget::Impl {
    int selected_index_ = 0;
};

StackedWidget::StackedWidget() : impl_(new StackedWidget::Impl()) {}

StackedWidget::~StackedWidget() {}

void StackedWidget::SetSelectedIndex(int index) { impl_->selected_index_ = index; }

int StackedWidget::GetSelectedIndex() const { return impl_->selected_index_; }

Size StackedWidget::CalcPreferredSize(const LayoutContext& context, const Constraints& constraints) const {
    Size size(0, 0);
    for (auto child : GetChildren()) {
        auto sz = child->CalcPreferredSize(context, constraints);
        size.width = std::max(size.width, sz.width);
        size.height = std::max(size.height, sz.height);
    }
    return size;
}

void StackedWidget::Layout(const LayoutContext& context) {
    auto& frame = GetFrame();
    for (auto child : GetChildren()) {
        child->SetFrame(frame);
    }

    Super::Layout(context);
}

Widget::DrawResult StackedWidget::Draw(const DrawContext& context) {
    // Don't Super, because Widget::Draw will draw all the children,
    // and we only want to draw the selected child.
    if (impl_->selected_index_ >= 0 && impl_->selected_index_ < int(GetChildren().size())) {
        return GetChildren()[impl_->selected_index_]->Draw(context);
    }
    return DrawResult::NONE;
}

}  // namespace gui
}  // namespace visualization
}  // namespace vox
