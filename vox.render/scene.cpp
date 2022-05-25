//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/scene.h"

#include <queue>

#include "vox.base/logging.h"
#include "vox.render/camera.h"
#include "vox.render/entity.h"

namespace vox {
Scene::Scene(vox::Device &device) : device(device), shader_data(device) {
    SetAmbientLight(std::make_shared<vox::AmbientLight>());
}

Scene::~Scene() { root_entities.clear(); }

Device &Scene::Device() { return device; }

const std::shared_ptr<AmbientLight> &Scene::AmbientLight() const { return ambient_light; }

void Scene::SetAmbientLight(const std::shared_ptr<vox::AmbientLight> &light) {
    if (!light) {
        LOGE("The scene must have one ambient light")
        return;
    }

    auto last_ambient_light = ambient_light;
    if (last_ambient_light != light) {
        light->SetScene(this);
        ambient_light = light;
    }
}

size_t Scene::RootEntitiesCount() { return root_entities.size(); }

const std::vector<std::unique_ptr<Entity>> &Scene::RootEntities() const { return root_entities; }

void Scene::Play() { ProcessActive(true); }

bool Scene::IsPlaying() const { return is_active_in_engine; }

Entity *Scene::CreateRootEntity(const std::string &name) {
    auto entity = std::make_unique<Entity>(name);
    auto entity_ptr = entity.get();
    AddRootEntity(std::move(entity));
    return entity_ptr;
}

void Scene::AddRootEntity(std::unique_ptr<Entity> &&entity) {
    const auto kIsRoot = entity->is_root;

    // let entity become root
    if (!kIsRoot) {
        entity->is_root = true;
        entity->RemoveFromParent();
    }

    // add or remove from scene's root_entities
    Entity *entity_ptr = entity.get();
    const auto kOldScene = entity->scene;
    if (kOldScene != this) {
        if (kOldScene && kIsRoot) {
            kOldScene->RemoveEntity(entity_ptr);
        }
        Entity::TraverseSetOwnerScene(entity_ptr, this);
        root_entities.emplace_back(std::move(entity));
    } else if (!kIsRoot) {
        root_entities.emplace_back(std::move(entity));
    }

    // process entity active/inActive
    if (is_active_in_engine) {
        if (!entity_ptr->is_active_in_hierarchy && entity_ptr->is_active) {
            entity_ptr->ProcessActive();
        }
    } else {
        if (entity_ptr->is_active_in_hierarchy) {
            entity_ptr->ProcessInActive();
        }
    }
}

void Scene::RemoveRootEntity(Entity *entity) {
    if (entity->is_root && entity->scene == this) {
        RemoveEntity(entity);
        if (is_active_in_engine) {
            entity->ProcessInActive();
        }
        Entity::TraverseSetOwnerScene(entity, nullptr);
    }
}

Entity *Scene::GetRootEntity(size_t index) { return root_entities[index].get(); }

Entity *Scene::FindEntityByName(const std::string &name) {
    const auto &children = root_entities;
    for (const auto &child : children) {
        if (child->name == name) {
            return child.get();
        }
    }

    for (const auto &child : children) {
        const auto kEntity = child->FindByName(name);
        if (kEntity) {
            return kEntity;
        }
    }
    return nullptr;
}

void Scene::AttachRenderCamera(Camera *camera) {
    auto iter = std::find(active_cameras.begin(), active_cameras.end(), camera);
    if (iter == active_cameras.end()) {
        active_cameras.push_back(camera);
    } else {
        LOGI("Camera already attached.")
    }
}

void Scene::DetachRenderCamera(Camera *camera) {
    auto iter = std::find(active_cameras.begin(), active_cameras.end(), camera);
    if (iter != active_cameras.end()) {
        active_cameras.erase(iter);
    }
}

void Scene::ProcessActive(bool active) {
    is_active_in_engine = active;
    for (const auto &entity : root_entities) {
        if (entity->is_active) {
            active ? entity->ProcessActive() : entity->ProcessInActive();
        }
    }
}

void Scene::RemoveEntity(Entity *entity) {
    auto &old_root_entities = root_entities;
    old_root_entities.erase(std::remove_if(old_root_entities.begin(), old_root_entities.end(),
                                           [entity](auto &old_entity) { return old_entity.get() == entity; }),
                            old_root_entities.end());
}

// MARK: - Update Loop
void Scene::UpdateShaderData() {
    // union scene and camera macro.
    for (auto &camera : active_cameras) {
        camera->Update();
    }
}

// MARK: - Reflection
void Scene::OnSerialize(nlohmann::json &data) {}

void Scene::OnDeserialize(const nlohmann::json &data) {}

}  // namespace vox
