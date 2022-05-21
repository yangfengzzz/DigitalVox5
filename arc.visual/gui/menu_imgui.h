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

#include "gui/menu_base.h"

namespace vox::visualization::gui {

class MenuImgui : public MenuBase {
public:
    MenuImgui();
    ~MenuImgui() override;

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

    [[nodiscard]] bool IsEnabled(ItemId item_id) const override;
    void SetEnabled(ItemId item_id, bool enabled) override;

    [[nodiscard]] bool IsChecked(ItemId item_id) const override;
    void SetChecked(ItemId item_id, bool checked) override;

    [[nodiscard]] int CalcHeight(const Theme& theme) const override;

    [[nodiscard]] bool CheckVisibilityChange() const override;

    ItemId DrawMenuBar(const DrawContext& context, bool is_enabled) override;
    ItemId Draw(const DrawContext& context, const char* name, bool is_enabled) override;

    void* GetNativePointer() override;

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

}  // namespace vox::visualization::gui
