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
#include "layer.h"
#include "transform.h"
#include "vobject.h"
#include "event.h"

namespace vox {
class Component;

/// @brief A leaf of the tree structure which can have children and a single parent.
class Entity final : public VObject {
public:
    /* Some events that are triggered when an action occur on the actor instance */
    Event<Component *> component_added_event_;
    Event<Component *> component_removed_event_;
    Event<Script *> behaviour_added_event_;
    Event<Script *> behaviour_removed_event_;
    
    /* Some events that are triggered when an action occur on any actor */
    static Event<Entity *> destroyed_event_;
    static Event<Entity *> created_event_;
    static Event<Entity *, Entity *> attach_event_;
    static Event<Entity *> dettach_event_;
    
public:
    /** The name of entity. */
    std::string name_;
    /** The layer the entity belongs to. */
    int layer_ = Layer::LAYER_0;
    /** Transform component. */
    Transform *transform_;
    
    /**
     * Create a entity.
     */
    explicit Entity(std::string name = "");
    
    ~Entity() override;
    
    /**
     * Whether to activate locally.
     */
    [[nodiscard]] bool is_active() const;
    
    void set_is_active(bool value);
    
    /**
     * Whether it is active in the hierarchy.
     */
    [[nodiscard]] bool is_active_in_hierarchy() const;
    
    /**
     * The parent entity.
     */
    Entity *parent();
    
    /**
     * The children entities
     */
    [[nodiscard]] const std::vector<std::unique_ptr<Entity>> &children() const;
    
    /**
     * Number of the children entities
     */
    size_t child_count();
    
    /**
     * The scene the entity belongs to.
     */
    Scene *scene();
    
    /**
     * Add component based on the component type.
     * @returns    The component which has been added.
     */
    template<typename T, typename ... Args>
    T *add_component(Args &&... args) {
        // ComponentsDependencies._addCheck(this, type);
        auto component = std::make_unique<T>(this, args...);
        T *component_ptr = component.get();
        components_.emplace_back(std::move(component));
        component_added_event_.invoke(component_ptr);
        if (is_active_in_hierarchy_) {
            component_ptr->set_active(true);
        }
        return component_ptr;
    }
    
    /**
     * Get component which match the type.
     * @returns    The first component which match type.
     */
    template<typename T>
    T *get_component() {
        for (auto &_component : components_) {
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
    template<typename T>
    std::vector<T *> get_components() {
        std::vector<T *> results;
        for (auto &_component : components_) {
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
    template<typename T>
    std::vector<T *> get_components_include_children() {
        std::vector<T *> results;
        get_components_in_children<T>(results);
        return results;
    }
    
    /**
     * Add child entity.
     * @param child - The child entity which want to be added.
     */
    void add_child(std::unique_ptr<Entity> &&child);
    
    /**
     * Remove child entity.
     * @param child - The child entity which want to be removed.
     */
    std::unique_ptr<Entity> remove_child(Entity *child);
    
    /**
     * Find child entity by index.
     * @param index - The index of the child entity.
     * @returns    The component which be found.
     */
    Entity *get_child(int index);
    
    /**
     * Find child entity by name.
     * @param name - The name of the entity which want to be found.
     * @returns The component which be found.
     */
    Entity *find_by_name(const std::string &name);
    
    /**
     * Find the entity by path.
     * @param path - The path fo the entity eg: /entity.
     * @returns The component which be found.
     */
    Entity *find_by_path(const std::string &path);
    
    /**
     * Create child entity.
     * @param name - The child entity's name.
     * @returns The child entity.
     */
    Entity *create_child(const std::string &name = "");
    
    /**
     * Clear children entities.
     */
    void clear_children();
    
    void remove_component(Component *component);
    
    /**fre
     * Clone
     * @returns Cloned entity.
     */
    std::unique_ptr<Entity> clone();
    
public:
    /**
     * Called when the serialization is asked
     */
    void on_serialize(nlohmann::json &data) override;
    
    /**
     * Called when the deserialization is asked
     */
    void on_deserialize(const nlohmann::json &data) override;
    
public:
    std::vector<Script *> scripts();
    
private:
    friend class Component;
    
    friend class Transform;
    
    friend class Script;
    
    friend class Scene;
    
    void add_script(Script *script);
    
    void remove_script(Script *script);
    
    std::unique_ptr<Entity> remove_from_parent();
    
    void process_active();
    
    void process_in_active();
    
    template<typename T>
    void get_components_in_children(std::vector<T *> &results) {
        for (auto &_component : components_) {
            T *component = dynamic_cast<T *>(_component.get());
            if (component) {
                results.push_back(component);
            }
        }
        for (auto &i : children_) {
            i->get_components_in_children(results);
        }
    }
    
    void set_active_components(bool is_active);
    
    void set_active_in_hierarchy(std::vector<Component *> &active_changed_components);
    
    void set_in_active_in_hierarchy(std::vector<Component *> &active_changed_components);
    
    void set_transform_dirty();
    
    static Entity *find_child_by_name(Entity *root, const std::string &name);
    
    static void traverse_set_owner_scene(Entity *entity, Scene *scene);
    
    bool is_active_in_hierarchy_ = false;
    std::vector<std::unique_ptr<Component>> components_{};
    std::vector<Script *> scripts_{};
    std::vector<std::unique_ptr<Entity>> children_{};
    Scene *scene_ = nullptr;
    bool is_root_ = false;
    bool is_active_ = true;
    
    Entity *parent_ = nullptr;
    std::vector<Component *> active_changed_components_{};
    
    std::unique_ptr<UpdateFlag> inverse_world_mat_flag_ = nullptr;
};

}        // namespace vox
