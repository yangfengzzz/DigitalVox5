//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <memory>

#include "vox.render/platform/input_events.h"

namespace vox::visualization::gui {

struct DrawContext;
struct Theme;

/// The menu item action is handled by Window, rather than by registering a
/// a callback function with (non-existent) Menu::SetOnClicked(). This is
/// because on macOS the menubar is global over all application windows, so any
/// callback would need to go find the data object corresponding to the active
/// window.
class MenuBase {
public:
    using ItemId = int;
    static constexpr ItemId NO_ITEM = -1;

    MenuBase() = default;
    virtual ~MenuBase() = default;

    virtual void AddItem(const char* name, ItemId item_id = NO_ITEM, vox::KeyCode key = vox::KeyCode::UNKNOWN) = 0;
    virtual void AddMenu(const char* name, std::shared_ptr<MenuBase> submenu) = 0;
    virtual void AddSeparator() = 0;

    virtual void InsertItem(int index,
                            const char* name,
                            ItemId item_id = NO_ITEM,
                            vox::KeyCode key = vox::KeyCode::UNKNOWN) = 0;
    virtual void InsertMenu(int index, const char* name, std::shared_ptr<MenuBase> submenu) = 0;
    virtual void InsertSeparator(int index) = 0;

    [[nodiscard]] virtual int GetNumberOfItems() const = 0;

    /// Searches the menu hierarchy down from this menu to find the item
    /// and returns true if the item is enabled.
    [[nodiscard]] virtual bool IsEnabled(ItemId item_id) const = 0;
    /// Searches the menu hierarchy down from this menu to find the item
    /// and set it enabled according to \p enabled.
    virtual void SetEnabled(ItemId item_id, bool enabled) = 0;

    [[nodiscard]] virtual bool IsChecked(ItemId item_id) const = 0;
    virtual void SetChecked(ItemId item_id, bool checked) = 0;

    [[nodiscard]] virtual int CalcHeight(const Theme& theme) const = 0;

    /// Returns true if submenu visibility changed on last call to DrawMenuBar
    [[nodiscard]] virtual bool CheckVisibilityChange() const = 0;

    virtual ItemId DrawMenuBar(const DrawContext& context, bool is_enabled) = 0;
    virtual ItemId Draw(const DrawContext& context, const char* name, bool is_enabled) = 0;

    virtual void* GetNativePointer() = 0;  // nullptr if not using native menus
};

}  // namespace vox::visualization::gui
