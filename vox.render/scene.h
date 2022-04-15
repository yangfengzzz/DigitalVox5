//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <algorithm>
#include <memory>
#include <string>
#include <vector>
#include <iostream>
#include <iomanip>
#include "core/device.h"

#include "scene_forward.h"
#include "vobject.h"
//#include "lighting/ambient_light.h"
//#include "shader/shader_data.h"
#include "background.h"
#include "platform/input_events.h"

namespace vox {
/// @brief A collection of entities organized in a tree structure.
///		   It can contain more than one root entity.
class Scene : public VObject {
public:
    /** Scene name. */
    std::string name_;
    
    /** The background of the scene. */
    Background background_ = Background();
    
    /** Scene-related shader data. */
    //    ShaderData shaderData;
    
    /**
     * Create scene.
     * @param device - Device
     */
    explicit Scene(Device &device);
    
    ~Scene() override;
    
    Device &device();
    
    /**
     * Ambient light.
     */
    //    const std::shared_ptr<AmbientLight> &ambientLight() const;
    
    //    void setAmbientLight(const std::shared_ptr<AmbientLight> &light);
    
    /**
     * Count of root entities.
     */
    size_t root_entities_count();
    
    /**
     * Root entity collection.
     */
    [[nodiscard]] const std::vector<std::unique_ptr<Entity>> &root_entities() const;
    
    /**
     * Play the scene
     */
    void play();
    
    /**
     * Returns true if the scene is playing
     */
    [[nodiscard]] bool is_playing() const;
    
    /**
     * Create root entity.
     * @param name - Entity name
     * @returns Entity
     */
    Entity *create_root_entity(const std::string &name = "");
    
    /**
     * Append an entity.
     * @param entity - The root entity to add
     */
    void add_root_entity(std::unique_ptr<Entity> &&entity);
    
    /**
     * Remove an entity.
     * @param entity - The root entity to remove
     */
    void remove_root_entity(Entity *entity);
    
    /**
     * Get root entity from index.
     * @param index - Index
     * @returns Entity
     */
    Entity *get_root_entity(size_t index = 0);
    
    /**
     * Find entity globally by name.
     * @param name - Entity name
     * @returns Entity
     */
    Entity *find_entity_by_name(const std::string &name);
    
    //    void attachRenderCamera(Camera *camera);
    //
    //    void detachRenderCamera(Camera *camera);
    
public:
    void update_shader_data();
    
public:
    /**
     * Called when the serialization is asked
     */
    void on_serialize(nlohmann::json &data) override;
    
    /**
     * Called when the deserialization is asked
     */
    void on_deserialize(const nlohmann::json &data) override;
    
private:
    friend class SceneManager;
    
    void process_active(bool active);
    
    void remove_entity(Entity *old_entity);
    
    std::vector<Camera *> active_cameras_{};
    
    bool is_active_in_engine_ = false;
    
    std::vector<std::unique_ptr<Entity>> root_entities_;
    //    std::shared_ptr<AmbientLight> _ambientLight;
    
    Device &device_;
};

}        // namespace vox
