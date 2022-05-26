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

class ListView : public Widget {
    using Super = Widget;

public:
    ListView();
    ~ListView() override;

    void SetItems(const std::vector<std::string>& items);

    /// Returns the currently selected item in the list.
    [[nodiscard]] int GetSelectedIndex() const;
    /// Returns the value of the currently selected item in the list.
    [[nodiscard]] const char* GetSelectedValue() const;
    /// Selects the indicated row of the list. Does not call onValueChanged.
    void SetSelectedIndex(int index);
    /// Limit the max visible items shown to user.
    /// Set to negative number will make list extends vertically as much
    /// as possible, otherwise the list will at least show 3 items and
    /// at most show \ref num items.
    void SetMaxVisibleItems(int num);

    [[nodiscard]] Size CalcPreferredSize(const LayoutContext& context, const Constraints& constraints) const override;

    [[nodiscard]] Size CalcMinimumSize(const LayoutContext& context) const override;

    DrawResult Draw(const DrawContext& context) override;

    /// Calls onValueChanged(const char *selectedText, bool isDoubleClick)
    /// when the list selection changes because of user action.
    void SetOnValueChanged(std::function<void(const char*, bool)> on_value_changed);

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

}  // namespace vox::visualization::gui
