//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <functional>

#include "gui/widget.h"

namespace vox::visualization::gui {

class Slider : public Widget {
public:
    enum Type { INT, DOUBLE };
    /// The only difference between INT and DOUBLE is that INT increments by
    /// 1.0 and coerces value to whole numbers.
    explicit Slider(Type type);
    ~Slider() override;

    /// Returns the value of the control as an integer
    [[nodiscard]] int GetIntValue() const;
    /// Returns the value of the control as a double.
    [[nodiscard]] double GetDoubleValue() const;
    /// Sets the value of the control. Will not call onValueChanged, but the
    /// value will be clamped to [min, max].
    void SetValue(double val);

    [[nodiscard]] double GetMinimumValue() const;
    [[nodiscard]] double GetMaximumValue() const;
    /// Sets the bounds for valid values of the widget. Values will be clamped
    /// to be within [minValue, maxValue].
    void SetLimits(double min_value, double max_value);

    [[nodiscard]] Size CalcPreferredSize(const LayoutContext& theme, const Constraints& constraints) const override;

    DrawResult Draw(const DrawContext& context) override;

    /// Sets a function to call when the value changes because of user action.
    void SetOnValueChanged(std::function<void(double)> on_value_changed);

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

}  // namespace vox::visualization::gui
