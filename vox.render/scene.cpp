//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "scene.h"
#include "vector2.h"
#include "vector3.h"
#include "vector4.h"
#include "color.h"
#include "matrix4x4.h"

#include <queue>
#include "logging.h"
#include "entity.h"
#include "camera.h"

namespace vox {
Scene::Scene(Device &device) :
device_(device) {
    //shaderData(device) {
    //    setAmbientLight(std::make_shared<AmbientLight>());
}

Scene::~Scene() {
    root_entities_.clear();
}

Device &Scene::device() {
    return device_;
}

//const std::shared_ptr<AmbientLight> &Scene::ambientLight() const {
//    return _ambientLight;
//}
//
//void Scene::setAmbientLight(const std::shared_ptr<AmbientLight> &light) {
//    if (!light) {
//        LOG(ERROR) << "The scene must have one ambient light\n";
//        return;
//    }
//
//    auto lastAmbientLight = _ambientLight;
//    if (lastAmbientLight != light) {
//        light->setScene(this);
//        _ambientLight = light;
//    }
//}

size_t Scene::root_entities_count() {
    return root_entities_.size();
}

const std::vector<std::unique_ptr<Entity>> &Scene::root_entities() const {
    return root_entities_;
}

void Scene::play() {
    process_active(true);
}

bool Scene::is_playing() const {
    return is_active_in_engine_;
}

Entity *Scene::create_root_entity(const std::string &name) {
    auto entity = std::make_unique<Entity>(name);
    auto entity_ptr = entity.get();
    add_root_entity(std::move(entity));
    return entity_ptr;
}

void Scene::add_root_entity(std::unique_ptr<Entity> &&entity) {
    const auto kIsRoot = entity->is_root_;
    
    // let entity become root
    if (!kIsRoot) {
        entity->is_root_ = true;
        entity->remove_from_parent();
    }
    
    // add or remove from scene's root_entities
    Entity *entity_ptr = entity.get();
    const auto kOldScene = entity->scene_;
    if (kOldScene != this) {
        if (kOldScene && kIsRoot) {
            kOldScene->remove_entity(entity_ptr);
        }
        Entity::traverse_set_owner_scene(entity_ptr, this);
        root_entities_.emplace_back(std::move(entity));
    } else if (!kIsRoot) {
        root_entities_.emplace_back(std::move(entity));
    }
    
    // process entity active/inActive
    if (is_active_in_engine_) {
        if (!entity_ptr->is_active_in_hierarchy_ && entity_ptr->is_active_) {
            entity_ptr->process_active();
        }
    } else {
        if (entity_ptr->is_active_in_hierarchy_) {
            entity_ptr->process_in_active();
        }
    }
}

void Scene::remove_root_entity(Entity *entity) {
    if (entity->is_root_ && entity->scene_ == this) {
        remove_entity(entity);
        if (is_active_in_engine_) {
            entity->process_in_active();
        }
        Entity::traverse_set_owner_scene(entity, nullptr);
    }
}

Entity *Scene::get_root_entity(size_t index) {
    return root_entities_[index].get();
}

Entity *Scene::find_entity_by_name(const std::string &name) {
    const auto &children = root_entities_;
    for (const auto &child : children) {
        if (child->name_ == name) {
            return child.get();
        }
    }
    
    for (const auto &child : children) {
        const auto kEntity = child->find_by_name(name);
        if (kEntity) {
            return kEntity;
        }
    }
    return nullptr;
}

void Scene::attachRenderCamera(Camera *camera) {
    auto iter = std::find(active_cameras_.begin(), active_cameras_.end(), camera);
    if (iter == active_cameras_.end()) {
        active_cameras_.push_back(camera);
    } else {
        LOGI("Camera already attached.");
    }
}

void Scene::detachRenderCamera(Camera *camera) {
    auto iter = std::find(active_cameras_.begin(), active_cameras_.end(), camera);
    if (iter != active_cameras_.end()) {
        active_cameras_.erase(iter);
    }
}

void Scene::process_active(bool active) {
    is_active_in_engine_ = active;
    const auto &root_entities = root_entities_;
    for (const auto &entity : root_entities) {
        if (entity->is_active_) {
            active ? entity->process_active() : entity->process_in_active();
        }
    }
}

void Scene::remove_entity(Entity *entity) {
    auto &old_root_entities = root_entities_;
    old_root_entities.erase(std::remove_if(old_root_entities.begin(),
                                           old_root_entities.end(), [entity](auto &old_entity) {
        return old_entity.get() == entity;
    }), old_root_entities.end());
}

//MARK: - Update Loop
void Scene::update_shader_data() {
    // union scene and camera macro.
    for (auto &camera: active_cameras_) {
        camera->update();
    }
}

//MARK: - Reflection
void Scene::on_serialize(nlohmann::json &data) {
    
}

void Scene::on_deserialize(const nlohmann::json &data) {
    
}

}        // namespace vox
