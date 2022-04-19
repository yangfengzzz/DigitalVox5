//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "scene_manager.h"

#include <utility>

namespace vox {
SceneManager::SceneManager(Device &device, std::string p_scene_root_folder) :
device_(device),
scene_root_folder_(std::move(p_scene_root_folder)) {
    load_empty_scene();
}

SceneManager::~SceneManager() {
    unload_current_scene();
}

void SceneManager::update() {
    if (delayed_load_call_) {
        delayed_load_call_();
        delayed_load_call_ = nullptr;
    }
}

void SceneManager::load_and_play_delayed(const std::string &p_path, bool p_absolute) {
    delayed_load_call_ = [this, p_path, p_absolute] {
        std::string previous_source_path = current_scene_source_path();
        load_scene(p_path, p_absolute);
        store_current_scene_source_path(previous_source_path);
        current_scene()->play();
    };
}

void SceneManager::load_empty_scene() {
    unload_current_scene();
    
    current_scene_ = std::make_unique<Scene>(device_);
    current_scene_->process_active(false);
    
    scene_load_event_.invoke();
}

bool SceneManager::load_scene(const std::string &p_path, bool p_absolute) {
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

bool SceneManager::load_scene_from_memory(const nlohmann::json &data) {
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

void SceneManager::unload_current_scene() {
    if (current_scene_) {
        current_scene_.reset();
        current_scene_ = nullptr;
        scene_unload_event_.invoke();
    }
    
    forget_current_scene_source_path();
}

bool SceneManager::has_current_scene() const {
    return current_scene_ != nullptr;
}

Scene *SceneManager::current_scene() {
    return current_scene_.get();
}

std::string SceneManager::current_scene_source_path() const {
    return current_scene_source_path_;
}

bool SceneManager::is_current_scene_loaded_from_disk() const {
    return current_scene_loaded_from_path_;
}

void SceneManager::store_current_scene_source_path(const std::string &p_path) {
    current_scene_source_path_ = p_path;
    current_scene_loaded_from_path_ = true;
    current_scene_source_path_changed_event_.invoke(current_scene_source_path_);
}

void SceneManager::forget_current_scene_source_path() {
    current_scene_source_path_ = "";
    current_scene_loaded_from_path_ = false;
    current_scene_source_path_changed_event_.invoke(current_scene_source_path_);
}

}
