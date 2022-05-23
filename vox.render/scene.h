//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "vox.render/background.h"
#include "vox.render/core/device.h"
#include "vox.render/lighting/ambient_light.h"
#include "vox.render/platform/input_events.h"
#include "vox.render/scene_forward.h"
#include "vox.render/shader/shader_data.h"
#include "vox.render/vobject.h"

namespace vox {
/// @brief A collection of entities organized in a tree structure.
///		   It can contain more than one root entity.
struct Scene final : public VObject {
public:
    /** Scene name. */
    std::string name;

    /** The background of the scene. */
    Background background = Background();

    /** Scene-related shader data. */
    ShaderData shader_data;

    /**
     * Create scene.
     * @param device - Device
     */
    explicit Scene(Device &device);

    ~Scene() override;

    Device &Device();

    /**
     * Ambient light.
     */
    [[nodiscard]] const std::shared_ptr<AmbientLight> &AmbientLight() const;

    void SetAmbientLight(const std::shared_ptr<vox::AmbientLight> &light);

    /**
     * Count of root entities.
     */
    size_t RootEntitiesCount();

    /**
     * Root entity collection.
     */
    [[nodiscard]] const std::vector<std::unique_ptr<Entity>> &RootEntities() const;

    /**
     * Play the scene
     */
    void Play();

    /**
     * Returns true if the scene is playing
     */
    [[nodiscard]] bool IsPlaying() const;

    /**
     * Create root entity.
     * @param name - Entity name
     * @returns Entity
     */
    Entity *CreateRootEntity(const std::string &name = "");

    /**
     * Append an entity.
     * @param entity - The root entity to add
     */
    void AddRootEntity(std::unique_ptr<Entity> &&entity);

    /**
     * Remove an entity.
     * @param entity - The root entity to remove
     */
    void RemoveRootEntity(Entity *entity);

    /**
     * Get root entity from index.
     * @param index - Index
     * @returns Entity
     */
    Entity *GetRootEntity(size_t index = 0);

    /**
     * Find entity globally by name.
     * @param name - Entity name
     * @returns Entity
     */
    Entity *FindEntityByName(const std::string &name);

    void AttachRenderCamera(Camera *camera);

    void DetachRenderCamera(Camera *camera);

public:
    void UpdateShaderData();

public:
    /**
     * Called when the serialization is asked
     */
    void OnSerialize(nlohmann::json &data) override;

    /**
     * Called when the deserialization is asked
     */
    void OnDeserialize(const nlohmann::json &data) override;

private:
    friend class SceneManager;

    void ProcessActive(bool active);

    void RemoveEntity(Entity *old_entity);

    std::vector<Camera *> active_cameras{};

    bool is_active_in_engine = false;

    std::vector<std::unique_ptr<Entity>> root_entities;
    std::shared_ptr<vox::AmbientLight> ambient_light;

    vox::Device &device;
};

}  // namespace vox
