//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <functional>
#include <string>

#include "vox.visual/gui/widget.h"

namespace vox::visualization::gui {

class RadioButton : public Widget {
public:
    /// VERT radio buttons will be layout vertically, each item takes a line.
    /// HORIZ radio buttons will be layout horizontally, all items will
    /// be in the same line.
    enum Type { VERT, HORIZ };

    explicit RadioButton(Type type);
    ~RadioButton() override;

    void SetItems(const std::vector<std::string>& items);
    [[nodiscard]] int GetSelectedIndex() const;
    [[nodiscard]] const char* GetSelectedValue() const;
    void SetSelectedIndex(int index);

    /// callback to be invoked while selected index is changed.
    /// A SetSelectedIndex will not trigger this callback.
    void SetOnSelectionChanged(std::function<void(int)> callback);

    [[nodiscard]] Size CalcPreferredSize(const LayoutContext& context, const Constraints& constraints) const override;

    DrawResult Draw(const DrawContext& context) override;

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

}  // namespace vox::visualization::gui
