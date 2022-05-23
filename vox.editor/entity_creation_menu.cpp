//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.editor/entity_creation_menu.h"

#include <utility>

#include "vox.editor/editor_actions.h"

namespace vox::editor {
std::function<void()> Combine(std::function<void()> p_a, const std::optional<std::function<void()>> &p_b) {
    if (p_b.has_value()) {
        return [=]() {
            p_a();
            p_b.value()();
        };
    }

    return p_a;
}

template <class T>
std::function<void()> EntityWithComponentCreationHandler(Entity *parent,
                                                         std::optional<std::function<void()>> on_item_clicked) {
    return Combine(
            std::bind(&EditorActions::CreateMonoComponentEntity<T>, EditorActions::GetSingletonPtr(), true, parent),
            on_item_clicked);
}

std::function<void()> EntityWithModelComponentCreationHandler(
        Entity *parent, const std::string &model_name, const std::optional<std::function<void()>> &on_item_clicked) {
    return Combine(std::bind(&EditorActions::CreateEntityWithModel, EditorActions::GetSingletonPtr(),
                             ":Models/" + model_name + ".fbx", true, parent, model_name),
                   on_item_clicked);
}

void EntityCreationMenu::GenerateEntityCreationMenu(::vox::ui::MenuList &menu_list,
                                                    Entity *parent,
                                                    const std::optional<std::function<void()>> &on_item_clicked) {
    menu_list.CreateWidget<MenuItem>("Create Empty").clicked_event_ +=
            Combine(std::bind(&::vox::editor::EditorActions::CreateEmptyEntity, EditorActions::GetSingletonPtr(), true,
                              parent, ""),
                    on_item_clicked);

    auto &primitives = menu_list.CreateWidget<MenuList>("Primitives");
    auto &physicals = menu_list.CreateWidget<MenuList>("Physicals");
    auto &lights = menu_list.CreateWidget<MenuList>("Lights");
    auto &audio = menu_list.CreateWidget<MenuList>("Audio");
    auto &others = menu_list.CreateWidget<MenuList>("Others");

    primitives.CreateWidget<MenuItem>("Cube").clicked_event_ +=
            EntityWithModelComponentCreationHandler(parent, "Cube", on_item_clicked);
    primitives.CreateWidget<MenuItem>("Sphere").clicked_event_ +=
            EntityWithModelComponentCreationHandler(parent, "Sphere", on_item_clicked);
    primitives.CreateWidget<MenuItem>("Cone").clicked_event_ +=
            EntityWithModelComponentCreationHandler(parent, "Cone", on_item_clicked);
    primitives.CreateWidget<MenuItem>("Cylinder").clicked_event_ +=
            EntityWithModelComponentCreationHandler(parent, "Cylinder", on_item_clicked);
    primitives.CreateWidget<MenuItem>("Plane").clicked_event_ +=
            EntityWithModelComponentCreationHandler(parent, "Plane", on_item_clicked);
    primitives.CreateWidget<MenuItem>("Gear").clicked_event_ +=
            EntityWithModelComponentCreationHandler(parent, "Gear", on_item_clicked);
    primitives.CreateWidget<MenuItem>("Helix").clicked_event_ +=
            EntityWithModelComponentCreationHandler(parent, "Helix", on_item_clicked);
    primitives.CreateWidget<MenuItem>("Pipe").clicked_event_ +=
            EntityWithModelComponentCreationHandler(parent, "Pipe", on_item_clicked);
    primitives.CreateWidget<MenuItem>("Pyramid").clicked_event_ +=
            EntityWithModelComponentCreationHandler(parent, "Pyramid", on_item_clicked);
    primitives.CreateWidget<MenuItem>("Torus").clicked_event_ +=
            EntityWithModelComponentCreationHandler(parent, "Torus", on_item_clicked);

    (void)physicals;
    (void)lights;
    (void)audio;
    (void)others;
}

}  // namespace vox::editor
