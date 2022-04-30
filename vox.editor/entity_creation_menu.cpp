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
    return combine(std::bind(&EditorActions::create_mono_component_entity<T>, EditorActions::get_singleton_ptr(),
                             true, parent), on_item_clicked);
}

std::function<void()> entity_with_model_component_creation_handler(Entity *parent, const std::string &model_name,
                                                                   const std::optional<std::function<void()>> &on_item_clicked) {
    return combine(std::bind(&EditorActions::create_entity_with_model, EditorActions::get_singleton_ptr(),
                             ":Models/" + model_name + ".fbx", true, parent, model_name), on_item_clicked);
}

void EntityCreationMenu::generate_entity_creation_menu(::vox::ui::MenuList &menu_list, Entity *p_parent,
                                                       const std::optional<std::function<void()>> &on_item_clicked) {
    menu_list.create_widget<MenuItem>("Create Empty").clicked_event_
    += combine(std::bind(&::vox::editor::EditorActions::create_empty_entity,
                         EditorActions::get_singleton_ptr(), true, p_parent, ""), on_item_clicked);
    
    auto &primitives = menu_list.create_widget<MenuList>("Primitives");
    auto &physicals = menu_list.create_widget<MenuList>("Physicals");
    auto &lights = menu_list.create_widget<MenuList>("Lights");
    auto &audio = menu_list.create_widget<MenuList>("Audio");
    auto &others = menu_list.create_widget<MenuList>("Others");
    
    primitives.create_widget<MenuItem>("Cube").clicked_event_ +=
    entity_with_model_component_creation_handler(p_parent, "Cube", on_item_clicked);
    primitives.create_widget<MenuItem>("Sphere").clicked_event_ +=
    entity_with_model_component_creation_handler(p_parent, "Sphere", on_item_clicked);
    primitives.create_widget<MenuItem>("Cone").clicked_event_ +=
    entity_with_model_component_creation_handler(p_parent, "Cone", on_item_clicked);
    primitives.create_widget<MenuItem>("Cylinder").clicked_event_ +=
    entity_with_model_component_creation_handler(p_parent, "Cylinder", on_item_clicked);
    primitives.create_widget<MenuItem>("Plane").clicked_event_ +=
    entity_with_model_component_creation_handler(p_parent, "Plane", on_item_clicked);
    primitives.create_widget<MenuItem>("Gear").clicked_event_ +=
    entity_with_model_component_creation_handler(p_parent, "Gear", on_item_clicked);
    primitives.create_widget<MenuItem>("Helix").clicked_event_ +=
    entity_with_model_component_creation_handler(p_parent, "Helix", on_item_clicked);
    primitives.create_widget<MenuItem>("Pipe").clicked_event_ +=
    entity_with_model_component_creation_handler(p_parent, "Pipe", on_item_clicked);
    primitives.create_widget<MenuItem>("Pyramid").clicked_event_ +=
    entity_with_model_component_creation_handler(p_parent, "Pyramid", on_item_clicked);
    primitives.create_widget<MenuItem>("Torus").clicked_event_ +=
    entity_with_model_component_creation_handler(p_parent, "Torus", on_item_clicked);
    
    (void)physicals;
    (void)lights;
    (void)audio;
    (void)others;
}

}
