//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <functional>

#include "vox.visual/gui/widget.h"

namespace vox::visualization::gui {

class ColorEdit : public Widget {
public:
    ColorEdit();
    ~ColorEdit() override;

    void SetValue(const vox::Color& color);
    void SetValue(float r, float g, float b);

    [[nodiscard]] const vox::Color& GetValue() const;

    [[nodiscard]] Size CalcPreferredSize(const LayoutContext& context, const Constraints& constraints) const override;

    DrawResult Draw(const DrawContext& context) override;

    /// Specifies a callback function which will be called when the value
    /// changes as a result of user action.
    void SetOnValueChanged(std::function<void(const vox::Color&)> on_value_changed);

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

}  // namespace vox::visualization::gui
