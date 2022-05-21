//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <functional>

#include "gui/widget.h"

namespace vox::visualization::gui {

class Checkbox : public Widget {
public:
    explicit Checkbox(const char* name);
    ~Checkbox() override;

    [[nodiscard]] bool IsChecked() const;
    void SetChecked(bool checked);

    Size CalcPreferredSize(const LayoutContext& context, const Constraints& constraints) const override;

    DrawResult Draw(const DrawContext& context) override;

    /// Specifies a callback function which will be called when the box
    /// changes checked state as a result of user action.
    void SetOnChecked(std::function<void(bool)> on_checked);

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

}  // namespace vox::visualization::gui
