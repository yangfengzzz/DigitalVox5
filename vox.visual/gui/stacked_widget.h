//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.visual/gui/widget.h"

namespace vox::visualization::gui {

/// Stacks its children on top of each other, with only the selected child
/// showing. It is like a tab control without the tabs.
class StackedWidget : public Widget {
    using Super = Widget;

public:
    StackedWidget();
    ~StackedWidget() override;

    /// Sets the index of the child to draw.
    void SetSelectedIndex(int index);
    /// Returns the index of the selected child.
    [[nodiscard]] int GetSelectedIndex() const;

    [[nodiscard]] Size CalcPreferredSize(const LayoutContext& context, const Constraints& constraints) const override;

    void Layout(const LayoutContext& context) override;

    Widget::DrawResult Draw(const DrawContext& context) override;

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

}  // namespace vox::visualization::gui
