//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.editor/ui/inspector.h"

#include "vox.editor/editor_actions.h"
#include "vox.render/behaviour.h"
#include "vox.render/entity.h"
#include "vox.render/ui/gui_drawer.h"
#include "vox.render/ui/plugins/drag_drop_target.h"
#include "vox.render/ui/widgets/buttons/button_simple.h"
#include "vox.render/ui/widgets/drags/drag_float.h"
#include "vox.render/ui/widgets/drags/drag_multiple_floats.h"
#include "vox.render/ui/widgets/input_fields/input_float.h"
#include "vox.render/ui/widgets/layout/columns.h"
#include "vox.render/ui/widgets/layout/spacing.h"
#include "vox.render/ui/widgets/selection/color_edit.h"
#include "vox.render/ui/widgets/texts/text.h"
#include "vox.render/ui/widgets/visual/image.h"

namespace vox::editor::ui {
Inspector::Inspector(const std::string &title, bool opened, const PanelWindowSettings &window_settings)
    : PanelWindow(title, opened, window_settings) {
    inspector_header_ = &CreateWidget<Group>();
    inspector_header_->enabled_ = false;
    entity_info_ = &CreateWidget<Group>();

    auto &header_columns = inspector_header_->CreateWidget<Columns<2>>();

    /* name_ field */
    auto name_gatherer = [this] { return target_entity_ ? target_entity_->name : "%undef%"; };
    auto name_provider = [this](const std::string &new_name) {
        if (target_entity_) target_entity_->name = new_name;
    };
    GuiDrawer::DrawString(header_columns, "Name", name_gatherer, name_provider);

    /* Active field */
    auto active_gatherer = [this] { return target_entity_ != nullptr && target_entity_->IsActive(); };
    auto active_provider = [this](bool active) {
        if (target_entity_) target_entity_->SetIsActive(active);
    };
    GuiDrawer::DrawBoolean(header_columns, "Active", active_gatherer, active_provider);
}

Inspector::~Inspector() {
    Entity::destroyed_event -= destroyed_listener_;
    UnFocus();
}

void Inspector::FocusEntity(Entity *target) {
    if (target_entity_) UnFocus();

    entity_info_->RemoveAllWidgets();

    target_entity_ = target;

    component_added_listener_ = target_entity_->component_added_event +=
            [this](auto useless) { EditorActions::GetSingleton().DelayAction([this] { Refresh(); }); };
    behaviour_added_listener_ = target_entity_->behaviour_added_event +=
            [this](auto useless) { EditorActions::GetSingleton().DelayAction([this] { Refresh(); }); };
    component_removed_listener_ = target_entity_->component_removed_event +=
            [this](auto useless) { EditorActions::GetSingleton().DelayAction([this] { Refresh(); }); };
    behaviour_removed_listener_ = target_entity_->behaviour_removed_event +=
            [this](auto useless) { EditorActions::GetSingleton().DelayAction([this] { Refresh(); }); };

    inspector_header_->enabled_ = true;

    CreateEntityInspector(target);

    // Force component and script selectors to trigger their ChangedEvent to update button states
    component_selector_widget_->value_changed_event_.Invoke(component_selector_widget_->current_choice_);
    script_selector_widget_->content_changed_event_.Invoke(script_selector_widget_->content_);

    EditorActions::GetSingleton().entity_selected_event_.Invoke(target_entity_);
}

void Inspector::UnFocus() {
    if (target_entity_) {
        target_entity_->component_added_event -= component_added_listener_;
        target_entity_->component_removed_event -= component_removed_listener_;
        target_entity_->behaviour_added_event -= behaviour_added_listener_;
        target_entity_->behaviour_removed_event -= behaviour_removed_listener_;
    }

    SoftUnFocus();
}

void Inspector::SoftUnFocus() {
    if (target_entity_) {
        EditorActions::GetSingleton().entity_unselected_event_.Invoke(target_entity_);
        inspector_header_->enabled_ = false;
        target_entity_ = nullptr;
        entity_info_->RemoveAllWidgets();
    }
}

Entity *Inspector::TargetEntity() const { return target_entity_; }

void Inspector::CreateEntityInspector(Entity *target) {
    std::map<std::string, Component *> components;

    //    for (auto &component: target->_components)
    //        if (component->name() != "Transform")
    //            components[component->name()] = component.get();

    auto transform = target->GetComponent<Transform>();
    if (transform) DrawComponent(transform);

    for (auto &[name, instance] : components) DrawComponent(instance);

    //    auto &behaviours = target.GetBehaviours();
    //
    //    for (auto&[name, behaviour]: behaviours)
    //        DrawBehaviour(behaviour);
}

void Inspector::DrawComponent(Component *component) {
    if (auto inspector_item = dynamic_cast<InspectorItem *>(component); inspector_item) {
        auto &header = entity_info_->CreateWidget<GroupCollapsable>(component->name());
        header.closable_ = !dynamic_cast<Transform *>(component);
        header.close_event_ += [this, &header, &component] {
            //            if (component->entity()->_removeComponent(component))
            //                _componentSelectorWidget->value_changed_event_.Invoke(_componentSelectorWidget->currentChoice);
        };
        auto &columns = header.CreateWidget<Columns<2>>();
        columns.widths_[0] = 200;
        inspector_item->OnInspector(columns);
    }
}

void Inspector::DrawBehaviour(Behaviour *behaviour) {
    if (auto inspector_item = dynamic_cast<InspectorItem *>(behaviour); inspector_item) {
        auto &header = entity_info_->CreateWidget<GroupCollapsable>(behaviour->name());
        header.closable_ = true;
        header.close_event_ += [this, &header, &behaviour] {
            //            behaviour->entity()->removeBehaviour(behaviour);
        };

        auto &columns = header.CreateWidget<Columns<2>>();
        columns.widths_[0] = 200;
        inspector_item->OnInspector(columns);
    }
}

void Inspector::Refresh() {
    if (target_entity_) {
        entity_info_->RemoveAllWidgets();
        CreateEntityInspector(target_entity_);
    }
}

}  // namespace vox::editor::ui
