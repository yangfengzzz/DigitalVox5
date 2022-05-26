//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.visual/gui/menu_base.h"

namespace vox::visualization::gui {

struct DrawContext;
struct Theme;

/// The menu item action is handled by Window, rather than by registering a
/// a callback function with (non-existent) Menu::SetOnClicked(). This is
/// because on macOS the menubar is global over all application windows, so any
/// callback would need to go find the data object corresponding to the active
/// window.
class Menu : public MenuBase {
    friend class Application;

public:
    Menu();
    ~Menu() override;

    void AddItem(const char* name, ItemId item_id = NO_ITEM, vox::KeyCode key = vox::KeyCode::UNKNOWN) override;
    void AddMenu(const char* name, std::shared_ptr<MenuBase> submenu) override;
    void AddSeparator() override;

    void InsertItem(int index,
                    const char* name,
                    ItemId item_id = NO_ITEM,
                    vox::KeyCode key = vox::KeyCode::UNKNOWN) override;
    void InsertMenu(int index, const char* name, std::shared_ptr<MenuBase> submenu) override;
    void InsertSeparator(int index) override;

    [[nodiscard]] int GetNumberOfItems() const override;

    /// Searches the menu hierarchy down from this menu to find the item
    /// and returns true if the item is enabled.
    [[nodiscard]] bool IsEnabled(ItemId item_id) const override;
    /// Searches the menu hierarchy down from this menu to find the item
    /// and set it enabled according to \p enabled.
    void SetEnabled(ItemId item_id, bool enabled) override;

    [[nodiscard]] bool IsChecked(ItemId item_id) const override;
    void SetChecked(ItemId item_id, bool checked) override;

    [[nodiscard]] int CalcHeight(const Theme& theme) const override;

    /// Returns true if submenu visibility changed on last call to DrawMenuBar
    [[nodiscard]] bool CheckVisibilityChange() const override;

    ItemId DrawMenuBar(const DrawContext& context, bool is_enabled) override;
    ItemId Draw(const DrawContext& context, const char* name, bool is_enabled) override;

    void* GetNativePointer() override;  // nullptr if not using native menus

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

}  // namespace vox::visualization::gui
