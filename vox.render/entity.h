//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <memory>
#include <string>
#include <typeindex>
#include <unordered_map>
#include <vector>

#include "vox.render/event.h"
#include "vox.render/layer.h"
#include "vox.render/transform.h"
#include "vox.render/vobject.h"

namespace vox {
class Component;

/// @brief A leaf of the tree structure which can have children and a single parent.
struct Entity final : public VObject {
public:
    /* Some events that are triggered when an action occur on the actor instance */
    Event<Component *> component_added_event;
    Event<Component *> component_removed_event;
    Event<Script *> behaviour_added_event;
    Event<Script *> behaviour_removed_event;

    /* Some events that are triggered when an action occur on any actor */
    static Event<Entity *> destroyed_event;
    static Event<Entity *> created_event;
    static Event<Entity *, Entity *> attach_event;
    static Event<Entity *> dettach_event;

public:
    /** The name of entity. */
    std::string name;
    /** The layer the entity belongs to. */
    int layer = Layer::LAYER_0;
    /** Transform component. */
    Transform *transform;

    /**
     * Create a entity.
     */
    explicit Entity(std::string name = "");

    ~Entity() override;

    /**
     * Whether to activate locally.
     */
    [[nodiscard]] bool IsActive() const;

    void SetIsActive(bool value);

    /**
     * Whether it is active in the hierarchy.
     */
    [[nodiscard]] bool IsActiveInHierarchy() const;

    /**
     * The parent entity.
     */
    Entity *Parent();

    /**
     * The children entities
     */
    [[nodiscard]] const std::vector<std::unique_ptr<Entity>> &Children() const;

    /**
     * Number of the children entities
     */
    size_t ChildCount();

    /**
     * The scene the entity belongs to.
     */
    Scene *Scene();

    /**
     * Add component based on the component type.
     * @returns    The component which has been added.
     */
    template <typename T, typename... Args>
    T *AddComponent(Args &&...args) {
        // ComponentsDependencies._addCheck(this, type);
        auto component = std::make_unique<T>(this, args...);
        T *component_ptr = component.get();
        components.emplace_back(std::move(component));
        component_added_event.Invoke(component_ptr);
        if (is_active_in_hierarchy) {
            component_ptr->SetActive(true);
        }
        return component_ptr;
    }

    /**
     * Get component which match the type.
     * @returns    The first component which match type.
     */
    template <typename T>
    T *GetComponent() {
        for (auto &_component : components) {
            T *component = dynamic_cast<T *>(_component.get());
            if (component) {
                return component;
            }
        }
        return nullptr;
    }

    /**
     * Get components which match the type.
     * @returns    The components which match type.
     */
    template <typename T>
    std::vector<T *> GetComponents() {
        std::vector<T *> results;
        for (auto &_component : components) {
            T *component = dynamic_cast<T *>(_component.get());
            if (component) {
                results.push_back(component);
            }
        }
        return results;
    }

    /**
     * Get the components which match the type of the entity and it's children.
     * @returns    The components collection which match the type.
     */
    template <typename T>
    std::vector<T *> GetComponentsIncludeChildren() {
        std::vector<T *> results;
        GetComponentsInChildren<T>(results);
        return results;
    }

    /**
     * Add child entity.
     * @param child - The child entity which want to be added.
     */
    void AddChild(std::unique_ptr<Entity> &&child);

    /**
     * Remove child entity.
     * @param child - The child entity which want to be removed.
     */
    std::unique_ptr<Entity> RemoveChild(Entity *child);

    /**
     * Find child entity by index.
     * @param index - The index of the child entity.
     * @returns    The component which be found.
     */
    Entity *GetChild(int index);

    /**
     * Find child entity by name.
     * @param name - The name of the entity which want to be found.
     * @returns The component which be found.
     */
    Entity *FindByName(const std::string &name);

    /**
     * Find the entity by path.
     * @param path - The path fo the entity eg: /entity.
     * @returns The component which be found.
     */
    Entity *FindByPath(const std::string &path);

    /**
     * Create child entity.
     * @param name - The child entity's name.
     * @returns The child entity.
     */
    Entity *CreateChild(const std::string &name = "");

    /**
     * Clear children entities.
     */
    void ClearChildren();

    void RemoveComponent(Component *component);

    /**fre
     * Clone
     * @returns Cloned entity.
     */
    std::unique_ptr<Entity> Clone();

public:
    /**
     * Called when the serialization is asked
     */
    void OnSerialize(nlohmann::json &data) override;

    /**
     * Called when the deserialization is asked
     */
    void OnDeserialize(const nlohmann::json &data) override;

public:
    std::vector<Script *> Scripts();

private:
    friend class Component;

    friend class Transform;

    friend class Script;

    friend class Scene;

    void AddScript(Script *script);

    void RemoveScript(Script *script);

    std::unique_ptr<Entity> RemoveFromParent();

    void ProcessActive();

    void ProcessInActive();

    template <typename T>
    void GetComponentsInChildren(std::vector<T *> &results) {
        for (auto &_component : components) {
            T *component = dynamic_cast<T *>(_component.get());
            if (component) {
                results.push_back(component);
            }
        }
        for (auto &i : children) {
            i->GetComponentsInChildren(results);
        }
    }

    void SetActiveComponents(bool is_active);

    void SetActiveInHierarchy(std::vector<Component *> &active_changed_components);

    void SetInActiveInHierarchy(std::vector<Component *> &active_changed_components);

    void SetTransformDirty();

    static Entity *FindChildByName(Entity *root, const std::string &name);

    static void TraverseSetOwnerScene(Entity *entity, vox::Scene *scene);

    bool is_active_in_hierarchy = false;
    std::vector<std::unique_ptr<Component>> components{};
    std::vector<Script *> scripts{};
    std::vector<std::unique_ptr<Entity>> children{};
    vox::Scene *scene = nullptr;
    bool is_root = false;
    bool is_active = true;

    Entity *parent = nullptr;
    std::vector<Component *> active_changed_components{};

    std::unique_ptr<UpdateFlag> inverse_world_mat_flag = nullptr;
};

}  // namespace vox
