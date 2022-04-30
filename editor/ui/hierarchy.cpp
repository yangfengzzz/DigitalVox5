//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "hierarchy.h"

#include "ui/widgets/buttons/button_simple.h"
#include "ui/widgets/selection/check_box.h"
#include "ui/widgets/visual/separator.h"
#include "ui/widgets/input_fields/input_text.h"
#include "ui/plugins/drag_drop_target.h"
#include "ui/plugins/drag_drop_source.h"
#include "ui/plugins/contextual_menu.h"
#include "editor_actions.h"
#include "entity_creation_menu.h"

namespace vox::editor::ui {
namespace {
class HierarchyContextualMenu : public ContextualMenu {
public:
    HierarchyContextualMenu(Entity *target, TreeNode &tree_node, bool panel_menu = false) :
    target_(target),
    tree_node_(tree_node) {
        if (target_) {
            auto &focus_button = create_widget<MenuItem>("Focus");
            focus_button.clicked_event_ += [this] {
                EditorActions::get_singleton().move_to_target(target_);
            };
            
            auto &duplicate_button = create_widget<MenuItem>("Duplicate");
            duplicate_button.clicked_event_ += [this] {
                EditorActions::get_singleton().delay_action(std::bind(&EditorActions::duplicate_entity,
                                                                      EditorActions::get_singleton_ptr(), target_, nullptr, true), 0);
            };
            
            auto &delete_button = create_widget<MenuItem>("Delete");
            delete_button.clicked_event_ += [this] {
                EditorActions::get_singleton().destroy_entity(target_);
            };
        }
        
        auto &create_entity = create_widget<MenuList>("Create...");
        EntityCreationMenu::generate_entity_creation_menu(create_entity, target_, std::bind(&TreeNode::open, &tree_node_));
    }
    
    void execute() override {
        if (!widgets_.empty())
            ContextualMenu::execute();
    }
    
private:
    Entity *target_;
    TreeNode &tree_node_;
};

void expand_tree_node(TreeNode &to_expand, const TreeNode *root) {
    to_expand.open();
    
    if (&to_expand != root && to_expand.has_parent()) {
        expand_tree_node(*static_cast<TreeNode *>(to_expand.parent()), root);
    }
}

std::vector<TreeNode *> nodesToCollapse;
std::vector<TreeNode *> founds;

void expand_tree_node_and_enable(TreeNode &to_expand, const TreeNode *root) {
    if (!to_expand.is_opened()) {
        to_expand.open();
        nodesToCollapse.push_back(&to_expand);
    }
    
    to_expand.enabled_ = true;
    
    if (&to_expand != root && to_expand.has_parent()) {
        expand_tree_node_and_enable(*static_cast<TreeNode *>(to_expand.parent()), root);
    }
}

} // namespace

//MARK: - Hierarchy
Hierarchy::Hierarchy(const std::string &title,
                     bool opened,
                     const PanelWindowSettings &window_settings) :
PanelWindow(title, opened, window_settings) {
    auto &search_bar = create_widget<InputText>();
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
            
            if (node->has_parent()) {
                expand_tree_node_and_enable(*static_cast<TreeNode *>(node->parent()), scene_root_);
            }
        }
        
        if (content.empty()) {
            for (auto node : nodesToCollapse) {
                node->close();
            }
            
            nodesToCollapse.clear();
        }
    };
    
    scene_root_ = &create_widget<TreeNode>("Root", true);
    static_cast<TreeNode *>(scene_root_)->open();
    scene_root_->add_plugin<DDTarget<std::pair<Entity *, TreeNode *>>>("Entity").data_received_event_ +=
    [this](std::pair<Entity *, TreeNode *> element) {
        if (element.second->has_parent())
            element.second->parent()->unconsider_widget(*element.second);
        
        scene_root_->consider_widget(*element.second);
        
        auto parent = element.first->parent();
        if (parent) {
            parent->remove_child(element.first);
        }
    };
    scene_root_->add_plugin<HierarchyContextualMenu>(nullptr, *scene_root_);
    
    EditorActions::get_singleton().entity_unselected_event_ += std::bind(&Hierarchy::unselect_entities_widgets, this);
    //    EDITOR_CONTEXT(sceneManager).SceneUnloadEvent += std::bind(&Hierarchy::clear, this);
    Entity::created_event_ += std::bind(&Hierarchy::add_entity_by_instance, this, std::placeholders::_1);
    Entity::destroyed_event_ += std::bind(&Hierarchy::delete_entity_by_instance, this, std::placeholders::_1);
    EditorActions::get_singleton().entity_selected_event_ += std::bind(&Hierarchy::select_entity_by_instance, this, std::placeholders::_1);
    Entity::attach_event_ += std::bind(&Hierarchy::attach_entity_to_parent, this, std::placeholders::_1);
    Entity::dettach_event_ += std::bind(&Hierarchy::detach_from_parent, this, std::placeholders::_1);
}

void Hierarchy::clear() {
    EditorActions::get_singleton().unselect_entity();
    
    scene_root_->remove_all_widgets();
    widget_entity_link_.clear();
}

void Hierarchy::unselect_entities_widgets() {
    for (auto &widget : widget_entity_link_)
        widget.second->selected_ = false;
}

void Hierarchy::select_entity_by_instance(Entity *entity) {
    auto result = std::find_if(widget_entity_link_.begin(), widget_entity_link_.end(),
                               [entity](const std::pair<Entity *, TreeNode *> &link) {
        return link.first == entity;
    });
    
    if (result != widget_entity_link_.end())
        if (result->second)
            select_entity_by_widget(*result->second);
}

void Hierarchy::select_entity_by_widget(TreeNode &widget) {
    unselect_entities_widgets();
    
    widget.selected_ = true;
    
    if (widget.has_parent()) {
        expand_tree_node(*static_cast<TreeNode *>(widget.parent()), scene_root_);
    }
}

void Hierarchy::attach_entity_to_parent(Entity *entity) {
    auto entity_widget = widget_entity_link_.find(entity);
    
    if (entity_widget != widget_entity_link_.end()) {
        auto widget = entity_widget->second;
        
        if (widget->parent())
            widget->parent()->unconsider_widget(*widget);
        
        if (entity->parent()) {
            auto parent_widget =
            std::find_if(widget_entity_link_.begin(), widget_entity_link_.end(), [&](std::pair<Entity *, TreeNode *> widget) {
                return widget.first == entity->parent();
            });
            parent_widget->second->leaf_ = false;
            parent_widget->second->consider_widget(*widget);
        }
    }
}

void Hierarchy::detach_from_parent(Entity *entity) {
    if (auto entity_widget = widget_entity_link_.find(entity); entity_widget != widget_entity_link_.end()) {
        if (entity->parent() && entity->parent()->children().size() == 1) {
            auto parent_widget =
            std::find_if(widget_entity_link_.begin(), widget_entity_link_.end(), [&](std::pair<Entity *, TreeNode *> widget) {
                return widget.first == entity->parent();
            });
            
            if (parent_widget != widget_entity_link_.end()) {
                parent_widget->second->leaf_ = true;
            }
        }
        
        auto widget = entity_widget->second;
        
        if (widget->parent())
            widget->parent()->unconsider_widget(*widget);
        
        scene_root_->consider_widget(*widget);
    }
}

void Hierarchy::delete_entity_by_instance(Entity *entity) {
    if (auto result = widget_entity_link_.find(entity); result != widget_entity_link_.end()) {
        if (result->second) {
            result->second->destroy();
        }
        
        widget_entity_link_.erase(result);
    }
}

void Hierarchy::add_entity_by_instance(Entity *entity) {
    auto &text_selectable = scene_root_->create_widget<TreeNode>(entity->name_, true);
    text_selectable.leaf_ = true;
    text_selectable.add_plugin<HierarchyContextualMenu>(entity, text_selectable);
    text_selectable
        .add_plugin<DDSource<std::pair<Entity *, TreeNode *>>>("Entity", "Attach to...", std::make_pair(entity, &text_selectable));
    text_selectable.add_plugin<DDTarget<std::pair<Entity *, TreeNode *>>>("Entity").data_received_event_ +=
    [entity, &text_selectable](std::pair<Entity *, TreeNode *> element) {
        if (element.second->parent())
            element.second->parent()->unconsider_widget(*element.second);
        
        text_selectable.consider_widget(*element.second);
        
        entity->add_child(element.first);
    };
    auto &dispatcher = text_selectable.add_plugin<DataDispatcher<std::string>>();
    
    Entity *target_ptr = entity;
    dispatcher.register_gatherer([target_ptr] {
        return target_ptr->name_;
    });
    
    widget_entity_link_[target_ptr] = &text_selectable;
    
    text_selectable.clicked_event_ += std::bind(&EditorActions::select_entity, EditorActions::get_singleton_ptr(), entity);
    text_selectable.double_clicked_event_ += std::bind(&EditorActions::move_to_target, EditorActions::get_singleton_ptr(), entity);
}

}
