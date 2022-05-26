//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <functional>

#include "vox.visual/gui/widget.h"

namespace vox::visualization::gui {

class ToggleSwitch : public Widget {
public:
    explicit ToggleSwitch(const char* title);
    ~ToggleSwitch() override;

    /// Returns the text of the toggle slider.
    [[nodiscard]] const char* GetText() const;
    /// Sets the text of the toggle slider.
    void SetText(const char* text);

    [[nodiscard]] bool GetIsOn() const;
    void SetOn(bool is_on);

    [[nodiscard]] Size CalcPreferredSize(const LayoutContext& context, const Constraints& constraints) const override;

    DrawResult Draw(const DrawContext& context) override;

    /// Sets a function that will be called when the switch is clicked on to
    /// change state. The boolean argument is true if the switch is now on
    /// and false otherwise.
    void SetOnClicked(std::function<void(bool)> on_clicked);

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

}  // namespace vox::visualization::gui
