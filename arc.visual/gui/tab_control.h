//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <functional>

#include "gui/widget.h"

namespace vox::visualization::gui {

class TabControl : public Widget {
    using Super = Widget;

public:
    TabControl();
    ~TabControl() override;

    void AddTab(const char* name, std::shared_ptr<Widget> panel);

    [[nodiscard]] Size CalcPreferredSize(const LayoutContext& context, const Constraints& constraints) const override;
    void Layout(const LayoutContext& context) override;

    DrawResult Draw(const DrawContext& context) override;

    void SetOnSelectedTabChanged(std::function<void(int)> on_changed);
    void SetSelectedTabIndex(int index);
    int GetSelectedTabIndex();

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

}  // namespace vox::visualization::gui
