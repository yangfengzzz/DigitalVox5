//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "entity.h"

#include <utility>
#include "scene.h"

#include "component.h"
//#include "components_manager.h"
//#include "script.h"
//#include "serializer.h"
#include "logging.h"

namespace vox {
Event<Entity *> Entity::destroyed_event_;
Event<Entity *> Entity::created_event_;
Event<Entity *, Entity *> Entity::attach_event_;
Event<Entity *> Entity::dettach_event_;

Entity *Entity::find_child_by_name(Entity *root, const std::string &name) {
    const auto &children = root->children_;
    for (const auto &child : children) {
        if (child->name_ == name) {
            return child.get();
        }
    }
    return nullptr;
}

void Entity::traverse_set_owner_scene(Entity *entity, Scene *scene) {
    entity->scene_ = scene;
    const auto &children = entity->children_;
    for (size_t i = 0; i < entity->children_.size(); i++) {
        traverse_set_owner_scene(children[i].get(), scene);
    }
}

Entity::Entity(std::string name) : name_(std::move(name)) {
    transform_ = add_component<Transform>();
    inverse_world_mat_flag_ = transform_->register_world_change_flag();
    
    created_event_.invoke(this);
}

Entity::~Entity() {
    if (parent_) {
        LOGE("use remove_child instead!")
    }
    
    destroyed_event_.invoke(this);
    std::for_each(components_.begin(), components_.end(), [&](std::unique_ptr<Component> &component) {
        component_removed_event_.invoke(component.get());
    });
    std::for_each(scripts_.begin(), scripts_.end(), [&](auto &behaviour) {
        behaviour_removed_event_.invoke(std::ref(behaviour));
    });
    
    for (auto &i : children_) {
        remove_child(i.get());
    }
    children_.clear();
    
    for (auto &component : components_) {
        remove_component(component.get());
    }
    components_.clear();
}

bool Entity::is_active() const {
    return is_active_;
}

void Entity::set_is_active(bool value) {
    if (value != is_active_) {
        is_active_ = value;
        if (value) {
            const auto &parent = parent_;
            if ((parent != nullptr && parent->is_active_in_hierarchy_)
                || (is_root_)) {
                process_active();
            }
        } else {
            if (is_active_in_hierarchy_) {
                process_in_active();
            }
        }
    }
}

bool Entity::is_active_in_hierarchy() const {
    return is_active_in_hierarchy_;
}

Entity *Entity::parent() {
    return parent_;
}

const std::vector<std::unique_ptr<Entity>> &Entity::children() const {
    return children_;
}

size_t Entity::child_count() {
    return children_.size();
}

Scene *Entity::scene() {
    return scene_;
}

void Entity::add_child(std::unique_ptr<Entity> &&child) {
    if (child->parent_ != this) {
        child->remove_from_parent();
        child->parent_ = this;
        attach_event_.invoke(child.get(), this);
        
        if (child->scene_ != scene_) {
            Entity::traverse_set_owner_scene(child.get(), scene_);
        }
        
        if (is_active_in_hierarchy_) {
            if (!child->is_active_in_hierarchy_ && child->is_active_) {
                child->process_active();
            }
        } else {
            if (child->is_active_in_hierarchy_) {
                child->process_in_active();
            }
        }
        child->set_transform_dirty();
        children_.emplace_back(std::move(child));
    }
}

std::unique_ptr<Entity> Entity::remove_child(Entity *child) {
    if (child->parent_ == this) {
        auto mem = child->remove_from_parent();
        if (child->is_active_in_hierarchy_) {
            child->process_in_active();
        }
        Entity::traverse_set_owner_scene(child, nullptr);
        child->set_transform_dirty();
        return mem;
    } else {
        return nullptr;
    }
}

Entity *Entity::get_child(int index) {
    return children_[index].get();
}

Entity *Entity::find_by_name(const std::string &name) {
    const auto &children = children_;
    const auto kChild = Entity::find_child_by_name(this, name);
    if (kChild) return kChild;
    for (const auto &child : children) {
        const auto kGrandson = child->find_by_name(name);
        if (kGrandson) {
            return kGrandson;
        }
    }
    return nullptr;
}

Entity *Entity::create_child(const std::string &name) {
    auto child = std::make_unique<Entity>(name);
    auto child_ptr = child.get();
    child->layer_ = layer_;
    add_child(std::move(child));
    return child_ptr;
}

void Entity::clear_children() {
    auto &children = children_;
    for (auto &child : children) {
        child->parent_ = nullptr;
        if (child->is_active_in_hierarchy_) {
            child->process_in_active();
        }
        Entity::traverse_set_owner_scene(child.get(), nullptr); // Must after child.process_in_active().
    }
    children.clear();
}

void Entity::remove_component(Component *component) {
    component_removed_event_.invoke(component);
    
    // ComponentsDependencies._removeCheck(this, component.constructor as any);
    components_.erase(std::remove_if(components_.begin(),
                                     components_.end(), [&](const std::unique_ptr<Component> &x) {
        return x.get() == component;
    }), components_.end());
}

std::unique_ptr<Entity> Entity::clone() {
    auto clone_entity = std::make_unique<Entity>(name_);
    
    clone_entity->is_active_ = is_active_;
    clone_entity->transform_->set_local_matrix(transform_->local_matrix());
    
    const auto &children = children_;
    for (size_t i = 0, len = children_.size(); i < len; i++) {
        const auto &child = children[i];
        clone_entity->add_child(child->clone());
    }
    
    const auto &components = components_;
    for (const auto &source_comp : components) {
        if (!(dynamic_cast<Transform *>(source_comp.get()))) {
            // TODO
        }
    }
    
    return clone_entity;
}

void Entity::add_script(Script *script) {
    behaviour_added_event_.invoke(script);
    
    auto iter = std::find(scripts_.begin(), scripts_.end(), script);
    if (iter == scripts_.end()) {
        scripts_.push_back(script);
    } else {
        LOGE("Script already attached.")
    }
}

void Entity::remove_script(Script *script) {
    auto iter = std::find(scripts_.begin(), scripts_.end(), script);
    if (iter != scripts_.end()) {
        behaviour_removed_event_.invoke(*iter);
        scripts_.erase(iter);
    }
}

std::unique_ptr<Entity> Entity::remove_from_parent() {
    std::unique_ptr<Entity> mem{nullptr};
    if (parent_ != nullptr) {
        auto &old_parent_children = parent_->children_;
        auto iter = std::find_if(old_parent_children.begin(), old_parent_children.end(),
                                 [&](const auto &child) {
            return child.get() == this;
        });
        mem = std::move(*iter);
        parent_ = nullptr;
        dettach_event_.invoke(this);
    }
    return mem;
}

//void Entity::process_active() {
//    _activeChangedComponents = ComponentsManager::get_singleton().get_active_changed_temp_list();
//    set_active_in_hierarchy(_activeChangedComponents);
//    set_active_components(true);
//}
//
//void Entity::process_in_active() {
//    _activeChangedComponents = ComponentsManager::get_singleton().get_active_changed_temp_list();
//    set_in_active_in_hierarchy(_activeChangedComponents);
//    set_active_components(false);
//}
//
//void Entity::set_active_components(bool is_active) {
//    auto &activeChangedComponents = _activeChangedComponents;
//    for (size_t i = 0, length = activeChangedComponents.size(); i < length; ++i) {
//        activeChangedComponents[i]->set_active(is_active);
//    }
//    ComponentsManager::get_singleton().put_active_changed_temp_list(activeChangedComponents);
//    _activeChangedComponents.clear();
//}

void Entity::set_active_in_hierarchy(std::vector<Component *> &active_changed_components) {
    is_active_in_hierarchy_ = true;
    auto &components = components_;
    for (auto &component : components) {
        active_changed_components.push_back(component.get());
    }
    const auto &children = children_;
    for (const auto &child : children) {
        if (child->is_active()) {
            child->set_active_in_hierarchy(active_changed_components);
        }
    }
}

void Entity::set_in_active_in_hierarchy(std::vector<Component *> &active_changed_components) {
    is_active_in_hierarchy_ = false;
    auto &components = components_;
    for (auto &component : components) {
        active_changed_components.push_back(component.get());
    }
    auto &children = children_;
    for (auto &child : children) {
        if (child->is_active()) {
            child->set_in_active_in_hierarchy(active_changed_components);
        }
    }
}

void Entity::set_transform_dirty() {
    if (transform_) {
        transform_->parent_change();
    } else {
        for (auto &i : children_) {
            i->set_transform_dirty();
        }
    }
}

std::vector<Script *> Entity::scripts() {
    return scripts_;
}

//MARK: - Reflection
//void Entity::on_serialize(tinyxml2::XMLDocument &p_doc, tinyxml2::XMLNode *p_actorsRoot) {
//    tinyxml2::XMLNode *actorNode = p_doc.NewElement("actor");
//    p_actorsRoot->InsertEndChild(actorNode);
//
//    Serializer::serializeString(p_doc, actorNode, "name", name);
//
//    tinyxml2::XMLNode *componentsNode = p_doc.NewElement("components");
//    actorNode->InsertEndChild(componentsNode);
//    for (auto &component: _components) {
//        /* Current component root */
//        tinyxml2::XMLNode *componentNode = p_doc.NewElement("component");
//        componentsNode->InsertEndChild(componentNode);
//
//        /* Component type */
//        Serializer::serializeString(p_doc, componentNode, "type", typeid(component).name());
//
//        /* Data node (Will be passed to the component) */
//        tinyxml2::XMLElement *data = p_doc.NewElement("data");
//        componentNode->InsertEndChild(data);
//
//        /* Data serialization of the component */
//        component->on_serialize(p_doc, data);
//    }
//}
//
//void Entity::on_deserialize(tinyxml2::XMLDocument &p_doc, tinyxml2::XMLNode *p_actorsRoot) {
//    Serializer::deserializeString(p_doc, p_actorsRoot, "name", name);
//
//    {
//        tinyxml2::XMLNode *componentsRoot = p_actorsRoot->FirstChildElement("components");
//        if (componentsRoot) {
//            tinyxml2::XMLElement *currentComponent = componentsRoot->FirstChildElement("component");
//
//            while (currentComponent) {
//                std::string componentType = currentComponent->FirstChildElement("type")->GetText();
//                currentComponent = currentComponent->NextSiblingElement("component");
//            }
//        }
//    }
//
//    {
//        tinyxml2::XMLNode *behavioursRoot = p_actorsRoot->FirstChildElement("behaviours");
//
//        if (behavioursRoot) {
//            tinyxml2::XMLElement *currentBehaviour = behavioursRoot->FirstChildElement("behaviour");
//
//            while (currentBehaviour) {
//                std::string behaviourType = currentBehaviour->FirstChildElement("type")->GetText();
//            }
//        }
//    }
//}

}        // namespace vox
