// ----------------------------------------------------------------------------
// -                        Open3D: www.open3d.org                            -
// ----------------------------------------------------------------------------
// The MIT License (MIT)
//
// Copyright (c) 2018-2021 www.open3d.org
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
// ----------------------------------------------------------------------------

#pragma once

#include <functional>

#include "gui/widget.h"

namespace arc::visualization::gui {

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

}  // namespace arc::visualization::gui
