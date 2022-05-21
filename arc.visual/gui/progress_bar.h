//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "gui/widget.h"

namespace vox::visualization::gui {

class ProgressBar : public Widget {
public:
    ProgressBar();
    ~ProgressBar() override;

    /// ProgressBar values ranges from 0.0 (incomplete) to 1.0 (complete)
    void SetValue(float value);
    [[nodiscard]] float GetValue() const;

    [[nodiscard]] Size CalcPreferredSize(const LayoutContext& context, const Constraints& constraints) const override;

    Widget::DrawResult Draw(const DrawContext& context) override;

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

}  // namespace vox::visualization::gui
