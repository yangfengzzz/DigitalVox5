//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <functional>

#include "vox.visual/gui/widget.h"

namespace vox::visualization::gui {

class Checkbox;
class ColorEdit;
class Label;
class NumberEdit;

/// The only difference between just putting in a Checkbox with
/// TreeView::AddItem() is that with a Checkbox, clicking on the
/// text will toggle on/off, whereas with this you must click on
/// the checkbox; clicking on the text will open/close the children
/// (if any, and if CanSelectItemsWithChildren is false).
class CheckableTextTreeCell : public Widget {
public:
    CheckableTextTreeCell(const char* text, bool is_checked, std::function<void(bool)> on_toggled);
    ~CheckableTextTreeCell() override;

    std::shared_ptr<Checkbox> GetCheckbox();
    std::shared_ptr<Label> GetLabel();

    [[nodiscard]] Size CalcPreferredSize(const LayoutContext& context, const Constraints& constraints) const override;
    void Layout(const LayoutContext& context) override;

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

class LUTTreeCell : public Widget {
public:
    LUTTreeCell(const char* text,
                bool is_checked,
                const vox::Color& color,
                std::function<void(bool)> on_enabled,
                std::function<void(const vox::Color&)> on_color_changed);
    ~LUTTreeCell() override;

    std::shared_ptr<Checkbox> GetCheckbox();
    std::shared_ptr<Label> GetLabel();
    std::shared_ptr<ColorEdit> GetColorEdit();

    [[nodiscard]] Size CalcPreferredSize(const LayoutContext& context, const Constraints& constraints) const override;
    void Layout(const LayoutContext& context) override;

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

class ColormapTreeCell : public Widget {
public:
    ColormapTreeCell(double value,
                     const vox::Color& color,
                     std::function<void(double)> on_value_changed,
                     std::function<void(const vox::Color&)> on_color_changed);
    ~ColormapTreeCell() override;

    std::shared_ptr<NumberEdit> GetNumberEdit();
    std::shared_ptr<ColorEdit> GetColorEdit();

    [[nodiscard]] Size CalcPreferredSize(const LayoutContext& context, const Constraints& constraints) const override;
    void Layout(const LayoutContext& context) override;

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

class TreeView : public Widget {
    using Super = Widget;

public:
    using ItemId = int;

    TreeView();
    ~TreeView() override;

    /// Returns the ID of the root item, that is,
    /// AddItem(GetRootItem(), ...) will be a top-level item.
    [[nodiscard]] ItemId GetRootItem() const;
    /// Adds an item to the tree.
    ItemId AddItem(ItemId parent_id, std::shared_ptr<Widget> item);
    /// Adds a text item to the tree
    ItemId AddTextItem(ItemId parent_id, const char* text);
    /// Removes an item an all its children (if any) from the tree
    void RemoveItem(ItemId item_id);
    /// Clears all the items
    void Clear();
    /// Returns item, or nullptr if item_id cannot be found.
    [[nodiscard]] std::shared_ptr<Widget> GetItem(ItemId item_id) const;
    [[nodiscard]] std::vector<ItemId> GetItemChildren(ItemId parent_id) const;

    [[nodiscard]] bool GetCanSelectItemsWithChildren() const;
    /// If true, enables selecting items that have children.
    /// Items can be toggled open/closed with the triangles or by
    /// double-clicking. Default is false.
    void SetCanSelectItemsWithChildren(bool can_select);

    /// Returns the currently selected item id in the tree.
    [[nodiscard]] ItemId GetSelectedItemId() const;
    /// Selects the indicated item of the list. Does not call onValueChanged.
    void SetSelectedItemId(ItemId item_id);

    [[nodiscard]] Size CalcPreferredSize(const LayoutContext& context, const Constraints& constraints) const override;
    [[nodiscard]] Size CalcMinimumSize(const LayoutContext& context) const override;

    void Layout(const LayoutContext& context) override;

    DrawResult Draw(const DrawContext& context) override;

    /// Calls onSelectionChanged(const char *sel_text, ItemId sel_item_id)
    /// when the list selection changes because of user action.
    void SetOnSelectionChanged(std::function<void(ItemId)> on_selection_changed);

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

}  // namespace vox::visualization::gui