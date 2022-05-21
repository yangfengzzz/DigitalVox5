//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <functional>

#include "gui/widget.h"

namespace vox::visualization::gui {

class Combobox : public Widget {
public:
    Combobox();
    explicit Combobox(const std::vector<const char*>& items);
    ~Combobox() override;

    void ClearItems();
    /// Adds an item to the combobox. Its index is the order in which it is
    /// added, so the first item's index is 0, the second is 1, etc.
    /// Returns the index of the new item.
    int AddItem(const char* name);

    /// Changes the item's text. \p index must be valid, otherwise
    /// nothing will happen.
    void ChangeItem(int index, const char* name);
    /// If an item exists with \p orig_name, it will be changed to
    /// \p new_name.
    void ChangeItem(const char* orig_name, const char* new_name);

    /// Removes the first item matching the given text.
    void RemoveItem(const char* name);
    /// Removes the item at \p index.
    void RemoveItem(int index);

    [[nodiscard]] int GetNumberOfItems() const;

    /// Returns the text of the item at \p index. \p index must be
    /// valid.
    [[nodiscard]] const char* GetItem(int index) const;

    [[nodiscard]] int GetSelectedIndex() const;
    /// Returns the text of the selected value, or "" if nothing is selected
    [[nodiscard]] const char* GetSelectedValue() const;
    /// Sets the selected item by index. Does not call the onValueChanged
    /// callback.
    void SetSelectedIndex(int index);
    /// Sets the selected item by value. Does nothing if \p value is not an
    /// item, but will return false. Does not call the onValueChanged callback
    bool SetSelectedValue(const char* value);

    [[nodiscard]] Size CalcPreferredSize(const LayoutContext& context, const Constraints& constraints) const override;

    DrawResult Draw(const DrawContext& context) override;

    /// Specifies a callback function which will be called when the value
    /// changes as a result of user action.
    void SetOnValueChanged(std::function<void(const char*, int)> on_value_changed);

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

}  // namespace vox::visualization::gui
