//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <functional>

#include "vox.visual/gui/widget.h"

namespace vox::visualization::gui {

class NumberEdit : public Widget {
    using Super = Widget;

public:
    enum Type { INT, DOUBLE };
    explicit NumberEdit(Type type);
    ~NumberEdit() override;

    [[nodiscard]] int GetIntValue() const;
    [[nodiscard]] double GetDoubleValue() const;
    void SetValue(double val);

    [[nodiscard]] double GetMinimumValue() const;
    [[nodiscard]] double GetMaximumValue() const;
    void SetLimits(double min_value, double max_value);

    int GetDecimalPrecision();
    void SetDecimalPrecision(int num_digits);

    void SetPreferredWidth(int width);

    void SetOnValueChanged(std::function<void(double)> on_changed);

    [[nodiscard]] Size CalcPreferredSize(const LayoutContext& context, const Constraints& constraints) const override;
    Widget::DrawResult Draw(const DrawContext& context) override;

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

}  // namespace vox::visualization::gui
