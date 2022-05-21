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

#include <memory>

#include "platform/input_events.h"

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
