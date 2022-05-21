//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "gui/menu.h"

#include <set>

#include "gui/menu_macos.h"

namespace vox::visualization::gui {

struct Menu::Impl {
    std::shared_ptr<MenuBase> menu;
    std::set<std::shared_ptr<MenuBase>> submenus;  // to keep shared_ptr alive
};

Menu::Menu() : impl_(new Menu::Impl()) {
#ifdef __APPLE__
    impl_->menu = std::make_shared<MenuMacOS>();
#else
    impl_->menu = std::make_shared<MenuImgui>();
#endif
}

Menu::~Menu() = default;

void Menu::AddItem(const char* name, ItemId item_id /*= NO_ITEM*/, vox::KeyCode key /*= KEY_NONE*/) {
    impl_->menu->AddItem(name, item_id, key);
}

void Menu::AddMenu(const char* name, std::shared_ptr<MenuBase> submenu) {
    auto menu_submenu = std::dynamic_pointer_cast<Menu>(submenu);
    if (menu_submenu) {
        impl_->menu->AddMenu(name, menu_submenu->impl_->menu);
        impl_->submenus.insert(submenu);
    } else {
        impl_->menu->AddMenu(name, submenu);
    }
}

void Menu::AddSeparator() { impl_->menu->AddSeparator(); }

void Menu::InsertItem(int index, const char* name, ItemId item_id /*= NO_ITEM*/, vox::KeyCode key /*= KEY_NONE*/) {
    impl_->menu->InsertItem(index, name, item_id, key);
}

void Menu::InsertMenu(int index, const char* name, std::shared_ptr<MenuBase> submenu) {
    auto menu_submenu = std::dynamic_pointer_cast<Menu>(submenu);
    if (menu_submenu) {
        impl_->menu->InsertMenu(index, name, menu_submenu->impl_->menu);
        impl_->submenus.insert(submenu);
    } else {
        impl_->menu->InsertMenu(index, name, submenu);
    }
}

void Menu::InsertSeparator(int index) { impl_->menu->InsertSeparator(index); }

int Menu::GetNumberOfItems() const { return impl_->menu->GetNumberOfItems(); }

bool Menu::IsEnabled(ItemId item_id) const { return impl_->menu->IsEnabled(item_id); }

void Menu::SetEnabled(ItemId item_id, bool enabled) { impl_->menu->SetEnabled(item_id, enabled); }

bool Menu::IsChecked(ItemId item_id) const { return impl_->menu->IsChecked(item_id); }

void Menu::SetChecked(ItemId item_id, bool checked) { impl_->menu->SetChecked(item_id, checked); }

int Menu::CalcHeight(const Theme& theme) const { return impl_->menu->CalcHeight(theme); }

bool Menu::CheckVisibilityChange() const { return impl_->menu->CheckVisibilityChange(); }

MenuBase::ItemId Menu::DrawMenuBar(const DrawContext& context, bool is_enabled) {
    return impl_->menu->DrawMenuBar(context, is_enabled);
}

MenuBase::ItemId Menu::Draw(const DrawContext& context, const char* name, bool is_enabled) {
    return impl_->menu->Draw(context, name, is_enabled);
}

void* Menu::GetNativePointer() { return impl_->menu->GetNativePointer(); }

}  // namespace vox::visualization::gui
