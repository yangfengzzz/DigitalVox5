//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "gui/widget.h"

namespace vox::visualization::gui {

class Label : public Widget {
    using Super = Widget;

public:
    /// Copies text
    explicit Label(const char* text = nullptr);
    ~Label() override;

    [[nodiscard]] const char* GetText() const;
    /// Sets the text of the label (copies text)
    void SetText(const char* text);

    [[nodiscard]] vox::Color GetTextColor() const;
    void SetTextColor(const vox::Color& color);

    [[nodiscard]] FontId GetFontId() const;
    void SetFontId(FontId font_id);

    [[nodiscard]] Size CalcPreferredSize(const LayoutContext& context, const Constraints& constraints) const override;

    DrawResult Draw(const DrawContext& context) override;

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

}  // namespace vox::visualization::gui
