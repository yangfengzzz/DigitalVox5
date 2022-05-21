//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "entity_creation_menu.h"

#include <utility>
#include "editor_actions.h"

namespace vox::editor {
std::function<void()> combine(std::function<void()> p_a, const std::optional<std::function<void()>> &p_b) {
    if (p_b.has_value()) {
        return [=]() {
            p_a();
            p_b.value()();
        };
    }
    
    return p_a;
}

template<class T>
std::function<void()> entity_with_component_creation_handler(Entity *parent,
                                                             std::optional<std::function<void()>> on_item_clicked) {
    return combine(std::bind(&EditorActions::create_mono_component_entity<T>, EditorActions::GetSingletonPtr(),
                             true, parent), on_item_clicked);
}

std::function<void()> entity_with_model_component_creation_handler(Entity *parent, const std::string &model_name,
                                                                   const std::optional<std::function<void()>> &on_item_clicked) {
    return combine(std::bind(&EditorActions::create_entity_with_model, EditorActions::GetSingletonPtr(),
                             ":Models/" + model_name + ".fbx", true, parent, model_name), on_item_clicked);
}

void EntityCreationMenu::generate_entity_creation_menu(::vox::ui::MenuList &menu_list, Entity *p_parent,
                                                       const std::optional<std::function<void()>> &on_item_clicked) {
    menu_list.CreateWidget<MenuItem>("Create Empty").clicked_event_
    += combine(std::bind(&::vox::editor::EditorActions::create_empty_entity,
                         EditorActions::GetSingletonPtr(), true, p_parent, ""), on_item_clicked);
    
    auto &primitives = menu_list.CreateWidget<MenuList>("Primitives");
    auto &physicals = menu_list.CreateWidget<MenuList>("Physicals");
    auto &lights = menu_list.CreateWidget<MenuList>("Lights");
    auto &audio = menu_list.CreateWidget<MenuList>("Audio");
    auto &others = menu_list.CreateWidget<MenuList>("Others");

    primitives.CreateWidget<MenuItem>("Cube").clicked_event_ +=
    entity_with_model_component_creation_handler(p_parent, "Cube", on_item_clicked);
    primitives.CreateWidget<MenuItem>("Sphere").clicked_event_ +=
    entity_with_model_component_creation_handler(p_parent, "Sphere", on_item_clicked);
    primitives.CreateWidget<MenuItem>("Cone").clicked_event_ +=
    entity_with_model_component_creation_handler(p_parent, "Cone", on_item_clicked);
    primitives.CreateWidget<MenuItem>("Cylinder").clicked_event_ +=
    entity_with_model_component_creation_handler(p_parent, "Cylinder", on_item_clicked);
    primitives.CreateWidget<MenuItem>("Plane").clicked_event_ +=
    entity_with_model_component_creation_handler(p_parent, "Plane", on_item_clicked);
    primitives.CreateWidget<MenuItem>("Gear").clicked_event_ +=
    entity_with_model_component_creation_handler(p_parent, "Gear", on_item_clicked);
    primitives.CreateWidget<MenuItem>("Helix").clicked_event_ +=
    entity_with_model_component_creation_handler(p_parent, "Helix", on_item_clicked);
    primitives.CreateWidget<MenuItem>("Pipe").clicked_event_ +=
    entity_with_model_component_creation_handler(p_parent, "Pipe", on_item_clicked);
    primitives.CreateWidget<MenuItem>("Pyramid").clicked_event_ +=
    entity_with_model_component_creation_handler(p_parent, "Pyramid", on_item_clicked);
    primitives.CreateWidget<MenuItem>("Torus").clicked_event_ +=
    entity_with_model_component_creation_handler(p_parent, "Torus", on_item_clicked);
    
    (void)physicals;
    (void)lights;
    (void)audio;
    (void)others;
}

}
