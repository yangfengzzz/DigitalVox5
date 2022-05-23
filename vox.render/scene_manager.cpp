//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/scene_manager.h"

#include <utility>

#include "vox.render/camera.h"
#include "vox.render/entity.h"
#include "vox.render/lighting/direct_light.h"

namespace vox {
SceneManager *SceneManager::GetSingletonPtr() { return ms_singleton; }

SceneManager &SceneManager::GetSingleton() {
    assert(ms_singleton);
    return (*ms_singleton);
}

SceneManager::SceneManager(Device &device, std::string p_scene_root_folder)
    : device_(device), scene_root_folder_(std::move(p_scene_root_folder)) {
    LoadEmptyScene();
}

SceneManager::~SceneManager() { UnloadCurrentScene(); }

void SceneManager::Update() {
    if (delayed_load_call_) {
        delayed_load_call_();
        delayed_load_call_ = nullptr;
    }
}

void SceneManager::LoadAndPlayDelayed(const std::string &p_path, bool p_absolute) {
    delayed_load_call_ = [this, p_path, p_absolute] {
        std::string previous_source_path = CurrentSceneSourcePath();
        LoadScene(p_path, p_absolute);
        StoreCurrentSceneSourcePath(previous_source_path);
        CurrentScene()->Play();
    };
}

void SceneManager::LoadEmptyScene() {
    UnloadCurrentScene();

    current_scene_ = std::make_unique<Scene>(device_);
    current_scene_->ProcessActive(false);

    scene_load_event_.Invoke();
}

void SceneManager::LoadEmptyLightedScene() {
    LoadEmptyScene();

    auto root_entity = current_scene_->CreateRootEntity();
    auto camera_entity = root_entity->CreateChild("MainCamera");
    camera_entity->transform->SetPosition(10, 10, 10);
    camera_entity->transform->LookAt(Point3F(0, 0, 0));
    camera_entity->AddComponent<Camera>();

    // init directional light
    auto light = root_entity->CreateChild("light");
    light->transform->SetPosition(0, 3, 0);
    light->AddComponent<DirectLight>();
}

bool SceneManager::LoadScene(const std::string &p_path, bool p_absolute) {
    //    std::string completePath = (p_absolute ? "" : _sceneRootFolder) + p_path;
    //
    //    tinyxml2::XMLDocument doc;
    //    doc.LoadFile(completePath.c_str());
    //
    //    if (load_scene_from_memory(doc)) {
    //        store_current_scene_source_path(completePath);
    //        return true;
    //    }

    return false;
}

bool SceneManager::LoadSceneFromMemory(const nlohmann::json &data) {
    //    if (!p_doc.Error()) {
    //        tinyxml2::XMLNode *root = p_doc.FirstChild();
    //        if (root) {
    //            tinyxml2::XMLNode *sceneNode = root->FirstChildElement("scene");
    //            if (sceneNode) {
    //                load_empty_scene();
    //                _currentScene->on_deserialize(p_doc, sceneNode);
    //                return true;
    //            }
    //        }
    //    }

    return false;
}

void SceneManager::UnloadCurrentScene() {
    if (current_scene_) {
        current_scene_.reset();
        current_scene_ = nullptr;
        scene_unload_event_.Invoke();
    }

    ForgetCurrentSceneSourcePath();
}

bool SceneManager::HasCurrentScene() const { return current_scene_ != nullptr; }

Scene *SceneManager::CurrentScene() { return current_scene_.get(); }

std::string SceneManager::CurrentSceneSourcePath() const { return current_scene_source_path_; }

bool SceneManager::IsCurrentSceneLoadedFromDisk() const { return current_scene_loaded_from_path_; }

void SceneManager::StoreCurrentSceneSourcePath(const std::string &p_path) {
    current_scene_source_path_ = p_path;
    current_scene_loaded_from_path_ = true;
    current_scene_source_path_changed_event_.Invoke(current_scene_source_path_);
}

void SceneManager::ForgetCurrentSceneSourcePath() {
    current_scene_source_path_ = "";
    current_scene_loaded_from_path_ = false;
    current_scene_source_path_changed_event_.Invoke(current_scene_source_path_);
}

}  // namespace vox
