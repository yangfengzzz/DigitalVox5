//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "inspector.h"
#include "entity.h"
#include "behaviour.h"

#include "ui/widgets/texts/text.h"
#include "ui/widgets/buttons/button_simple.h"
#include "ui/widgets/drags/drag_multiple_floats.h"
#include "ui/widgets/drags/drag_float.h"
#include "ui/widgets/layout/spacing.h"
#include "ui/widgets/layout/columns.h"
#include "ui/widgets/visual/image.h"
#include "ui/widgets/input_fields/input_float.h"
#include "ui/widgets/selection/color_edit.h"
#include "ui/plugins/drag_drop_target.h"
#include "ui/gui_drawer.h"
#include "editor_actions.h"

namespace vox::editor::ui {
Inspector::Inspector(const std::string &title,
                     bool opened,
                     const PanelWindowSettings &window_settings) :
PanelWindow(title, opened, window_settings) {
    inspector_header_ = &create_widget<Group>();
    inspector_header_->enabled_ = false;
    entity_info_ = &create_widget<Group>();
    
    auto &header_columns = inspector_header_->create_widget<Columns<2>>();
    
    /* name_ field */
    auto name_gatherer = [this] {
        return target_entity_ ? target_entity_->name_ : "%undef%";
    };
    auto name_provider = [this](const std::string &new_name) {
        if (target_entity_) target_entity_->name_ = new_name;
    };
    GuiDrawer::draw_string(header_columns, "Name", name_gatherer, name_provider);
    
    /* Active field */
    auto active_gatherer = [this] {
        return target_entity_ != nullptr && target_entity_->is_active();
    };
    auto active_provider = [this](bool active) {
        if (target_entity_) target_entity_->set_is_active(active);
    };
    GuiDrawer::draw_boolean(header_columns, "Active", active_gatherer, active_provider);
    
}

Inspector::~Inspector() {
    Entity::destroyed_event_ -= destroyed_listener_;
    un_focus();
}

void Inspector::focus_entity(Entity *target) {
    if (target_entity_)
        un_focus();
    
    entity_info_->remove_all_widgets();
    
    target_entity_ = target;
    
    component_added_listener_ = target_entity_->component_added_event_ += [this](auto useless) {
        EditorActions::GetSingleton().delay_action([this] {
            refresh();
        });
    };
    behaviour_added_listener_ = target_entity_->behaviour_added_event_ += [this](auto useless) {
        EditorActions::GetSingleton().delay_action([this] {
            refresh();
        });
    };
    component_removed_listener_ = target_entity_->component_removed_event_ += [this](auto useless) {
        EditorActions::GetSingleton().delay_action([this] {
            refresh();
        });
    };
    behaviour_removed_listener_ = target_entity_->behaviour_removed_event_ += [this](auto useless) {
        EditorActions::GetSingleton().delay_action([this] {
            refresh();
        });
    };
    
    inspector_header_->enabled_ = true;
    
    create_entity_inspector(target);
    
    // Force component and script selectors to trigger their ChangedEvent to update button states
    component_selector_widget_->value_changed_event_.invoke(component_selector_widget_->current_choice_);
    script_selector_widget_->content_changed_event_.invoke(script_selector_widget_->content_);
    
    EditorActions::GetSingleton().entity_selected_event_.invoke(target_entity_);
}

void Inspector::un_focus() {
    if (target_entity_) {
        target_entity_->component_added_event_ -= component_added_listener_;
        target_entity_->component_removed_event_ -= component_removed_listener_;
        target_entity_->behaviour_added_event_ -= behaviour_added_listener_;
        target_entity_->behaviour_removed_event_ -= behaviour_removed_listener_;
    }
    
    soft_un_focus();
}

void Inspector::soft_un_focus() {
    if (target_entity_) {
        EditorActions::GetSingleton().entity_unselected_event_.invoke(target_entity_);
        inspector_header_->enabled_ = false;
        target_entity_ = nullptr;
        entity_info_->remove_all_widgets();
    }
}

Entity *Inspector::target_entity() const {
    return target_entity_;
}

void Inspector::create_entity_inspector(Entity *target) {
    std::map<std::string, Component *> components;
    
    //    for (auto &component: target->_components)
    //        if (component->name() != "Transform")
    //            components[component->name()] = component.get();
    
    auto transform = target->get_component<Transform>();
    if (transform)
        draw_component(transform);
    
    for (auto &[name, instance] : components)
        draw_component(instance);
    
    //    auto &behaviours = target.GetBehaviours();
    //
    //    for (auto&[name, behaviour]: behaviours)
    //        DrawBehaviour(behaviour);
}

void Inspector::draw_component(Component *component) {
    if (auto inspector_item = dynamic_cast<InspectorItem *>(component); inspector_item) {
        auto &header = entity_info_->create_widget<GroupCollapsable>(component->name());
        header.closable_ = !dynamic_cast<Transform *>(component);
        header.close_event_ += [this, &header, &component] {
            //            if (component->entity()->_removeComponent(component))
            //                _componentSelectorWidget->value_changed_event_.invoke(_componentSelectorWidget->currentChoice);
        };
        auto &columns = header.create_widget<Columns<2>>();
        columns.widths_[0] = 200;
        inspector_item->on_inspector(columns);
    }
}

void Inspector::draw_behaviour(Behaviour *behaviour) {
    if (auto inspector_item = dynamic_cast<InspectorItem *>(behaviour); inspector_item) {
        auto &header = entity_info_->create_widget<GroupCollapsable>(behaviour->name());
        header.closable_ = true;
        header.close_event_ += [this, &header, &behaviour] {
            //            behaviour->entity()->removeBehaviour(behaviour);
        };
        
        auto &columns = header.create_widget<Columns<2>>();
        columns.widths_[0] = 200;
        inspector_item->on_inspector(columns);
    }
}

void Inspector::refresh() {
    if (target_entity_) {
        entity_info_->remove_all_widgets();
        create_entity_inspector(target_entity_);
    }
}

}
