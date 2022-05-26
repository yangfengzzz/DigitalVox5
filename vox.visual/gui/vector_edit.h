//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.visual/gui/widget.h"
#include "vox.math/vector3.h"

namespace vox::visualization::gui {

class VectorEdit : public Widget {
    using Super = Widget;

public:
    VectorEdit();
    ~VectorEdit() override;

    [[nodiscard]] vox::Vector3F GetValue() const;
    /// Sets the value of the widget. Does not call onValueChanged.
    void SetValue(const vox::Vector3F& val);

    /// Sets the function that is called whenever the value in the widget
    /// changes because of user behavior
    void SetOnValueChanged(std::function<void(const vox::Vector3F&)> on_changed);

    [[nodiscard]] Size CalcPreferredSize(const LayoutContext& context, const Constraints& constraints) const override;
    Widget::DrawResult Draw(const DrawContext& context) override;

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

}  // namespace vox::visualization::gui
