//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "gui/menu_base.h"

namespace vox::visualization::gui {

class MenuMacOS : public MenuBase {
public:
    MenuMacOS();
    ~MenuMacOS() override;

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
