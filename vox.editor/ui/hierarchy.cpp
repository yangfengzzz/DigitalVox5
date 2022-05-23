//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.editor/ui/hierarchy.h"

#include "vox.editor/editor_actions.h"
#include "vox.editor/entity_creation_menu.h"
#include "vox.render/ui/plugins/contextual_menu.h"
#include "vox.render/ui/plugins/drag_drop_source.h"
#include "vox.render/ui/plugins/drag_drop_target.h"
#include "vox.render/ui/widgets/buttons/button_simple.h"
#include "vox.render/ui/widgets/input_fields/input_text.h"
#include "vox.render/ui/widgets/selection/check_box.h"
#include "vox.render/ui/widgets/visual/separator.h"

namespace vox::editor::ui {
namespace {
class HierarchyContextualMenu : public ContextualMenu {
public:
    HierarchyContextualMenu(Entity *target, TreeNode &tree_node, bool panel_menu = false)
        : target_(target), tree_node_(tree_node) {
        if (target_) {
            auto &focus_button = CreateWidget<MenuItem>("Focus");
            focus_button.clicked_event_ += [this] { EditorActions::GetSingleton().MoveToTarget(target_); };

            auto &duplicate_button = CreateWidget<MenuItem>("Duplicate");
            duplicate_button.clicked_event_ += [this] {
                EditorActions::GetSingleton().DelayAction(
                        std::bind(&EditorActions::DuplicateEntity, EditorActions::GetSingletonPtr(), target_, nullptr,
                                  true),
                        0);
            };

            auto &delete_button = CreateWidget<MenuItem>("Delete");
            delete_button.clicked_event_ += [this] { EditorActions::GetSingleton().DestroyEntity(target_); };
        }

        auto &create_entity = CreateWidget<MenuList>("Create...");
        EntityCreationMenu::GenerateEntityCreationMenu(create_entity, target_, std::bind(&TreeNode::Open, &tree_node_));
    }

    void Execute() override {
        if (!widgets_.empty()) ContextualMenu::Execute();
    }

private:
    Entity *target_;
    TreeNode &tree_node_;
};

void expand_tree_node(TreeNode &to_expand, const TreeNode *root) {
    to_expand.Open();

    if (&to_expand != root && to_expand.HasParent()) {
        expand_tree_node(*static_cast<TreeNode *>(to_expand.Parent()), root);
    }
}

std::vector<TreeNode *> nodesToCollapse;
std::vector<TreeNode *> founds;

void ExpandTreeNodeAndEnable(TreeNode &to_expand, const TreeNode *root) {
    if (!to_expand.IsOpened()) {
        to_expand.Open();
        nodesToCollapse.push_back(&to_expand);
    }

    to_expand.enabled_ = true;

    if (&to_expand != root && to_expand.HasParent()) {
        ExpandTreeNodeAndEnable(*static_cast<TreeNode *>(to_expand.Parent()), root);
    }
}

}  // namespace

// MARK: - Hierarchy
Hierarchy::Hierarchy(const std::string &title, bool opened, const PanelWindowSettings &window_settings)
    : PanelWindow(title, opened, window_settings) {
    auto &search_bar = CreateWidget<InputText>();
    search_bar.content_changed_event_ += [this](const std::string &origin) {
        founds.clear();
        auto content = origin;
        std::transform(content.begin(), content.end(), content.begin(), ::tolower);

        for (auto &[entity, item] : widget_entity_link_) {
            if (!content.empty()) {
                auto item_name = item->name_;
                std::transform(item_name.begin(), item_name.end(), item_name.begin(), ::tolower);

                if (item_name.find(content) != std::string::npos) {
                    founds.push_back(item);
                }

                item->enabled_ = false;
            } else {
                item->enabled_ = true;
            }
        }

        for (auto node : founds) {
            node->enabled_ = true;

            if (node->HasParent()) {
                ExpandTreeNodeAndEnable(*static_cast<TreeNode *>(node->Parent()), scene_root_);
            }
        }

        if (content.empty()) {
            for (auto node : nodesToCollapse) {
                node->Close();
            }

            nodesToCollapse.clear();
        }
    };

    scene_root_ = &CreateWidget<TreeNode>("Root", true);
    static_cast<TreeNode *>(scene_root_)->Open();
    scene_root_->AddPlugin<DDTarget<std::pair<Entity *, TreeNode *>>>("Entity").data_received_event_ +=
            [this](std::pair<Entity *, TreeNode *> element) {
                if (element.second->HasParent()) element.second->Parent()->UnconsiderWidget(*element.second);

                scene_root_->ConsiderWidget(*element.second);

                auto parent = element.first->Parent();
                if (parent) {
                    parent->RemoveChild(element.first);
                }
            };
    scene_root_->AddPlugin<HierarchyContextualMenu>(nullptr, *scene_root_);

    EditorActions::GetSingleton().entity_unselected_event_ += std::bind(&Hierarchy::UnselectEntitiesWidgets, this);
    //    EDITOR_CONTEXT(sceneManager).SceneUnloadEvent += std::bind(&Hierarchy::clear, this);
    Entity::created_event += std::bind(&Hierarchy::AddEntityByInstance, this, std::placeholders::_1);
    Entity::destroyed_event += std::bind(&Hierarchy::DeleteEntityByInstance, this, std::placeholders::_1);
    EditorActions::GetSingleton().entity_selected_event_ +=
            std::bind(&Hierarchy::SelectEntityByInstance, this, std::placeholders::_1);
    Entity::attach_event += std::bind(&Hierarchy::AttachEntityToParent, this, std::placeholders::_1);
    Entity::dettach_event += std::bind(&Hierarchy::DetachFromParent, this, std::placeholders::_1);
}

void Hierarchy::Clear() {
    EditorActions::GetSingleton().UnselectEntity();

    scene_root_->RemoveAllWidgets();
    widget_entity_link_.clear();
}

void Hierarchy::UnselectEntitiesWidgets() {
    for (auto &widget : widget_entity_link_) widget.second->selected_ = false;
}

void Hierarchy::SelectEntityByInstance(Entity *entity) {
    auto result = std::find_if(widget_entity_link_.begin(), widget_entity_link_.end(),
                               [entity](const std::pair<Entity *, TreeNode *> &link) { return link.first == entity; });

    if (result != widget_entity_link_.end())
        if (result->second) SelectEntityByWidget(*result->second);
}

void Hierarchy::SelectEntityByWidget(TreeNode &widget) {
    UnselectEntitiesWidgets();

    widget.selected_ = true;

    if (widget.HasParent()) {
        expand_tree_node(*static_cast<TreeNode *>(widget.Parent()), scene_root_);
    }
}

void Hierarchy::AttachEntityToParent(Entity *entity) {
    auto entity_widget = widget_entity_link_.find(entity);

    if (entity_widget != widget_entity_link_.end()) {
        auto widget = entity_widget->second;

        if (widget->Parent()) widget->Parent()->UnconsiderWidget(*widget);

        if (entity->Parent()) {
            auto parent_widget = std::find_if(
                    widget_entity_link_.begin(), widget_entity_link_.end(),
                    [&](std::pair<Entity *, TreeNode *> widget) { return widget.first == entity->Parent(); });
            parent_widget->second->leaf_ = false;
            parent_widget->second->ConsiderWidget(*widget);
        }
    }
}

void Hierarchy::DetachFromParent(Entity *entity) {
    if (auto entity_widget = widget_entity_link_.find(entity); entity_widget != widget_entity_link_.end()) {
        if (entity->Parent() && entity->Parent()->Children().size() == 1) {
            auto parent_widget = std::find_if(
                    widget_entity_link_.begin(), widget_entity_link_.end(),
                    [&](std::pair<Entity *, TreeNode *> widget) { return widget.first == entity->Parent(); });

            if (parent_widget != widget_entity_link_.end()) {
                parent_widget->second->leaf_ = true;
            }
        }

        auto widget = entity_widget->second;

        if (widget->Parent()) widget->Parent()->UnconsiderWidget(*widget);

        scene_root_->ConsiderWidget(*widget);
    }
}

void Hierarchy::DeleteEntityByInstance(Entity *entity) {
    if (auto result = widget_entity_link_.find(entity); result != widget_entity_link_.end()) {
        if (result->second) {
            result->second->Destroy();
        }

        widget_entity_link_.erase(result);
    }
}

void Hierarchy::AddEntityByInstance(Entity *entity) {
    auto &text_selectable = scene_root_->CreateWidget<TreeNode>(entity->name, true);
    text_selectable.leaf_ = true;
    text_selectable.AddPlugin<HierarchyContextualMenu>(entity, text_selectable);
    text_selectable.AddPlugin<DDSource<std::pair<Entity *, TreeNode *>>>("Entity", "Attach to...",
                                                                         std::make_pair(entity, &text_selectable));
    text_selectable.AddPlugin<DDTarget<std::pair<Entity *, TreeNode *>>>("Entity").data_received_event_ +=
            [entity, &text_selectable](std::pair<Entity *, TreeNode *> element) {
                if (element.second->Parent()) element.second->Parent()->UnconsiderWidget(*element.second);

                text_selectable.ConsiderWidget(*element.second);

                entity->AddChild(element.first->Parent()->RemoveChild(element.first));
            };
    auto &dispatcher = text_selectable.AddPlugin<DataDispatcher<std::string>>();

    Entity *target_ptr = entity;
    dispatcher.RegisterGatherer([target_ptr] { return target_ptr->name; });

    widget_entity_link_[target_ptr] = &text_selectable;

    text_selectable.clicked_event_ += std::bind(&EditorActions::SelectEntity, EditorActions::GetSingletonPtr(), entity);
    text_selectable.double_clicked_event_ +=
            std::bind(&EditorActions::MoveToTarget, EditorActions::GetSingletonPtr(), entity);
}

}  // namespace vox::editor::ui
