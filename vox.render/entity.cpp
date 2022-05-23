//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/entity.h"

#include <utility>

#include "vox.render/component.h"
#include "vox.render/components_manager.h"
#include "vox.render/scene.h"
#include "vox.render/script.h"
//#include "vox.render/serializer.h"
#include "vox.render/logging.h"

namespace vox {
Event<Entity *> Entity::destroyed_event;
Event<Entity *> Entity::created_event;
Event<Entity *, Entity *> Entity::attach_event;
Event<Entity *> Entity::dettach_event;

Entity *Entity::FindChildByName(Entity *root, const std::string &name) {
    const auto &children = root->children;
    for (const auto &child : children) {
        if (child->name == name) {
            return child.get();
        }
    }
    return nullptr;
}

void Entity::TraverseSetOwnerScene(Entity *entity, vox::Scene *scene) {
    entity->scene = scene;
    const auto &children = entity->children;
    for (size_t i = 0; i < entity->children.size(); i++) {
        TraverseSetOwnerScene(children[i].get(), scene);
    }
}

Entity::Entity(std::string name) : name(std::move(name)) {
    transform = AddComponent<Transform>();
    inverse_world_mat_flag = transform->RegisterWorldChangeFlag();

    created_event.Invoke(this);
}

Entity::~Entity() {
    if (parent) {
        LOGE("use RemoveChild instead!")
    }

    destroyed_event.Invoke(this);
    std::for_each(components.begin(), components.end(),
                  [&](std::unique_ptr<Component> &component) { component_removed_event.Invoke(component.get()); });
    std::for_each(scripts.begin(), scripts.end(),
                  [&](auto &behaviour) { behaviour_removed_event.Invoke(std::ref(behaviour)); });

    for (auto &i : children) {
        RemoveChild(i.get());
    }
    children.clear();

    for (auto &component : components) {
        RemoveComponent(component.get());
    }
    components.clear();
}

bool Entity::IsActive() const { return is_active; }

void Entity::SetIsActive(bool value) {
    if (value != is_active) {
        is_active = value;
        if (value) {
            if ((parent != nullptr && parent->is_active_in_hierarchy) || (is_root)) {
                ProcessActive();
            }
        } else {
            if (is_active_in_hierarchy) {
                ProcessInActive();
            }
        }
    }
}

bool Entity::IsActiveInHierarchy() const { return is_active_in_hierarchy; }

Entity *Entity::Parent() { return parent; }

const std::vector<std::unique_ptr<Entity>> &Entity::Children() const { return children; }

size_t Entity::ChildCount() { return children.size(); }

Scene *Entity::Scene() { return scene; }

void Entity::AddChild(std::unique_ptr<Entity> &&child) {
    if (child->parent != this) {
        child->RemoveFromParent();
        child->parent = this;
        attach_event.Invoke(child.get(), this);

        if (child->scene != scene) {
            Entity::TraverseSetOwnerScene(child.get(), scene);
        }

        if (is_active_in_hierarchy) {
            if (!child->is_active_in_hierarchy && child->is_active) {
                child->ProcessActive();
            }
        } else {
            if (child->is_active_in_hierarchy) {
                child->ProcessInActive();
            }
        }
        child->SetTransformDirty();
        children.emplace_back(std::move(child));
    }
}

std::unique_ptr<Entity> Entity::RemoveChild(Entity *child) {
    if (child->parent == this) {
        auto mem = child->RemoveFromParent();
        if (child->is_active_in_hierarchy) {
            child->ProcessInActive();
        }
        Entity::TraverseSetOwnerScene(child, nullptr);
        child->SetTransformDirty();
        return mem;
    } else {
        return nullptr;
    }
}

Entity *Entity::GetChild(int index) { return children[index].get(); }

Entity *Entity::FindByName(const std::string &name) {
    const auto kChild = Entity::FindChildByName(this, name);
    if (kChild) return kChild;
    for (const auto &child : children) {
        const auto kGrandson = child->FindByName(name);
        if (kGrandson) {
            return kGrandson;
        }
    }
    return nullptr;
}

Entity *Entity::CreateChild(const std::string &name) {
    auto child = std::make_unique<Entity>(name);
    auto child_ptr = child.get();
    child->layer = layer;
    AddChild(std::move(child));
    return child_ptr;
}

void Entity::ClearChildren() {
    for (auto &child : children) {
        child->parent = nullptr;
        if (child->is_active_in_hierarchy) {
            child->ProcessInActive();
        }
        Entity::TraverseSetOwnerScene(child.get(), nullptr);  // Must after child.process_in_active().
    }
    children.clear();
}

void Entity::RemoveComponent(Component *component) {
    component_removed_event.Invoke(component);

    // ComponentsDependencies._removeCheck(this, component.constructor as any);
    components.erase(std::remove_if(components.begin(), components.end(),
                                    [&](const std::unique_ptr<Component> &x) { return x.get() == component; }),
                     components.end());
}

std::unique_ptr<Entity> Entity::Clone() {
    auto clone_entity = std::make_unique<Entity>(name);

    clone_entity->is_active = is_active;
    clone_entity->transform->SetLocalMatrix(transform->LocalMatrix());

    for (size_t i = 0, len = children.size(); i < len; i++) {
        const auto &child = children[i];
        clone_entity->AddChild(child->Clone());
    }

    for (const auto &source_comp : components) {
        if (!(dynamic_cast<Transform *>(source_comp.get()))) {
            // TODO
        }
    }

    return clone_entity;
}

void Entity::AddScript(Script *script) {
    behaviour_added_event.Invoke(script);

    auto iter = std::find(scripts.begin(), scripts.end(), script);
    if (iter == scripts.end()) {
        scripts.push_back(script);
    } else {
        LOGE("Script already attached.")
    }
}

void Entity::RemoveScript(Script *script) {
    auto iter = std::find(scripts.begin(), scripts.end(), script);
    if (iter != scripts.end()) {
        behaviour_removed_event.Invoke(*iter);
        scripts.erase(iter);
    }
}

std::unique_ptr<Entity> Entity::RemoveFromParent() {
    std::unique_ptr<Entity> mem{nullptr};
    if (parent != nullptr) {
        auto &old_parent_children = parent->children;
        auto iter = std::find_if(old_parent_children.begin(), old_parent_children.end(),
                                 [&](const auto &child) { return child.get() == this; });
        mem = std::move(*iter);
        parent = nullptr;
        dettach_event.Invoke(this);
    }
    return mem;
}

void Entity::ProcessActive() {
    active_changed_components = ComponentsManager::GetSingleton().GetActiveChangedTempList();
    SetActiveInHierarchy(active_changed_components);
    SetActiveComponents(true);
}

void Entity::ProcessInActive() {
    active_changed_components = ComponentsManager::GetSingleton().GetActiveChangedTempList();
    SetInActiveInHierarchy(active_changed_components);
    SetActiveComponents(false);
}

void Entity::SetActiveComponents(bool is_active) {
    for (size_t i = 0, length = active_changed_components.size(); i < length; ++i) {
        active_changed_components[i]->SetActive(is_active);
    }
    ComponentsManager::GetSingleton().PutActiveChangedTempList(active_changed_components);
    active_changed_components.clear();
}

void Entity::SetActiveInHierarchy(std::vector<Component *> &active_changed_components) {
    is_active_in_hierarchy = true;
    for (auto &component : components) {
        active_changed_components.push_back(component.get());
    }
    for (const auto &child : children) {
        if (child->IsActive()) {
            child->SetActiveInHierarchy(active_changed_components);
        }
    }
}

void Entity::SetInActiveInHierarchy(std::vector<Component *> &active_changed_components) {
    is_active_in_hierarchy = false;
    for (auto &component : components) {
        active_changed_components.push_back(component.get());
    }
    for (auto &child : children) {
        if (child->IsActive()) {
            child->SetInActiveInHierarchy(active_changed_components);
        }
    }
}

void Entity::SetTransformDirty() {
    if (transform) {
        transform->ParentChange();
    } else {
        for (auto &i : children) {
            i->SetTransformDirty();
        }
    }
}

std::vector<Script *> Entity::Scripts() { return scripts; }

// MARK: - Reflection
void Entity::OnSerialize(nlohmann::json &data) {}

void Entity::OnDeserialize(const nlohmann::json &data) {}

}  // namespace vox
