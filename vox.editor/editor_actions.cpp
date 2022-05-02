//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "editor_actions.h"
#include "scene_manager.h"
#include "lua/script_interpreter.h"
#include "view/scene_view.h"
#include "view/asset_view.h"
#include "view/game_view.h"
#include "ui/inspector.h"
#include "ui/ui_manager.h"

namespace vox {
editor::EditorActions *editor::EditorActions::get_singleton_ptr() {
    return ms_singleton_;
}

editor::EditorActions &editor::EditorActions::get_singleton() {
    assert(ms_singleton_);
    return (*ms_singleton_);
}

namespace editor {
EditorActions::EditorActions(ui::PanelsManager &panels_manager) :
panels_manager_(panels_manager) {
    
}

//MARK: - SCENE
void EditorActions::load_empty_scene() {
    if (current_editor_mode() != EditorMode::EDIT)
        stop_playing();
    
    SceneManager::get_singleton().load_empty_lighted_scene();
    LOGI("New scene created")
}

void EditorActions::save_current_scene_to(const std::string &path) {
    SceneManager::get_singleton().store_current_scene_source_path(path);
    nlohmann::json root;
    SceneManager::get_singleton().current_scene()->on_serialize(root);
    
    nlohmann::json j = {
        {"root", root},
    };
    fs::write_json(j, path);
}

void EditorActions::load_scene_from_disk(const std::string &path, bool absolute) {
    if (current_editor_mode() != EditorMode::EDIT)
        stop_playing();
    
    SceneManager::get_singleton().load_scene(path, absolute);
    LOGI("Scene loaded from disk: {}", SceneManager::get_singleton().current_scene_source_path())
    panels_manager_.get_panel_as<ui::SceneView>("Scene View").focus();
}

bool EditorActions::is_current_scene_loaded_from_disk() const {
    return SceneManager::get_singleton().is_current_scene_loaded_from_disk();
}

void EditorActions::save_scene_changes() {
    if (is_current_scene_loaded_from_disk()) {
        save_current_scene_to(SceneManager::get_singleton().current_scene_source_path());
        LOGI("Current scene saved to: {}" + SceneManager::get_singleton().current_scene_source_path())
    } else {
        save_as();
    }
}

void EditorActions::save_as() {
    // todo
}

//MARK: - SCRIPTING
void EditorActions::refresh_scripts() {
    ScriptInterpreter::get_singleton().refresh_all();
    panels_manager_.get_panel_as<ui::Inspector>("Inspector").refresh();
    if (ScriptInterpreter::get_singleton().is_ok()) {
        LOGI("Scripts interpretation succeeded!")
    }
}

//MARK: - BUILDING
std::optional<std::string> EditorActions::select_build_folder() {
    // todo
    return std::nullopt;
}

void EditorActions::build(bool auto_run, bool temp_folder) {
    // todo
}

void EditorActions::build_at_location(const std::string &configuration, const std::string &build_path, bool auto_run) {
    // todo
}

//MARK: - ACTION_SYSTEM
void EditorActions::delay_action(const std::function<void()> &action, uint32_t frames) {
    delayed_actions_.emplace_back(frames + 1, action);
}

void EditorActions::execute_delayed_actions() {
    std::for_each(delayed_actions_.begin(), delayed_actions_.end(),
                  [](std::pair<uint32_t, std::function<void()>> &element) {
        --element.first;
        
        if (element.first == 0)
            element.second();
    });
    
    delayed_actions_.erase(std::remove_if(delayed_actions_.begin(), delayed_actions_.end(),
                                          [](std::pair<uint32_t, std::function<void()>> &element) {
        return element.first == 0;
    }), delayed_actions_.end());
}

//MARK: - TOOLS
ui::PanelsManager &EditorActions::panels_manager() {
    return panels_manager_;
}

//MARK: - SETTINGS
void EditorActions::set_entity_spawn_at_origin(bool value) {
    if (value)
        entity_spawn_mode_ = EntitySpawnMode::ORIGIN;
    else
        entity_spawn_mode_ = EntitySpawnMode::FRONT;
}

void EditorActions::set_entity_spawn_mode(EntitySpawnMode value) {
    entity_spawn_mode_ = value;
}

void EditorActions::reset_layout() {
    delay_action([]() {
        UiManager::get_singleton().reset_layout("Config\\layout.ini");
    });
}

void EditorActions::set_scene_view_camera_speed(int speed) {
    auto orbit_control = panels_manager_.get_panel_as<ui::SceneView>("Scene View").camera_control();
    orbit_control->rotate_speed_ = speed;
    orbit_control->zoom_speed_ = speed;
}

int EditorActions::scene_view_camera_speed() {
    return panels_manager_.get_panel_as<ui::SceneView>("Scene View").camera_control()->rotate_speed_;
}

void EditorActions::set_asset_view_camera_speed(int speed) {
    auto orbit_control = panels_manager_.get_panel_as<ui::AssetView>("Asset View").camera_control();
    orbit_control->rotate_speed_ = speed;
    orbit_control->zoom_speed_ = speed;
}

int EditorActions::asset_view_camera_speed() {
    return panels_manager_.get_panel_as<ui::AssetView>("Asset View").camera_control()->rotate_speed_;
}

void EditorActions::reset_scene_view_camera_position() {
    auto orbit_control = panels_manager_.get_panel_as<ui::SceneView>("Scene View").camera_control();
    orbit_control->entity()->transform_->set_position({-10.0f, 4.0f, 10.0f});
}

void EditorActions::reset_asset_view_camera_position() {
    auto orbit_control = panels_manager_.get_panel_as<ui::AssetView>("Asset View").camera_control();
    orbit_control->entity()->transform_->set_position({-10.0f, 4.0f, 10.0f});
}

//MARK: - GAME
EditorActions::EditorMode EditorActions::current_editor_mode() const {
    return editor_mode_;
}

void EditorActions::set_editor_mode(EditorMode new_editor_mode) {
    editor_mode_ = new_editor_mode;
    editor_mode_changed_event_.invoke(editor_mode_);
}

void EditorActions::start_playing() {
    if (editor_mode_ == EditorMode::EDIT) {
        ScriptInterpreter::get_singleton().refresh_all();
        panels_manager_.get_panel_as<ui::Inspector>("Inspector").refresh();
        
        if (ScriptInterpreter::get_singleton().is_ok()) {
            play_event_.invoke();
            scene_backup_.clear();
            
            nlohmann::json root;
            SceneManager::get_singleton().current_scene()->on_serialize(root);
            scene_backup_.insert(scene_backup_.begin(), {"root", root});
            
            panels_manager_.get_panel_as<ui::GameView>("Game View").focus();
            SceneManager::get_singleton().current_scene()->play();
            set_editor_mode(EditorMode::PLAY);
        }
    } else {
        // m_context.audioEngine->Unsuspend();
        set_editor_mode(EditorMode::PLAY);
    }
}

void EditorActions::pause_game() {
    set_editor_mode(EditorMode::PAUSE);
}

void EditorActions::stop_playing() {
    if (editor_mode_ != EditorMode::EDIT) {
        // ImGui::GetIO().DisableMouseUpdate = false;
        // m_context.window->SetCursorMode(OvWindowing::Cursor::ECursorMode::NORMAL);
        set_editor_mode(EditorMode::EDIT);
        bool loaded_from_disk = SceneManager::get_singleton().is_current_scene_loaded_from_disk();
        std::string scene_source_path = SceneManager::get_singleton().current_scene_source_path();
        
        std::string focused_actor_id;
        
        if (auto target_actor = panels_manager_.get_panel_as<ui::Inspector>("Inspector").target_entity())
            focused_actor_id = target_actor->name_;
        
        SceneManager::get_singleton().load_scene_from_memory(scene_backup_);
        if (loaded_from_disk) {
            // To bo able to save or reload the scene whereas the scene is loaded from memory (Supposed to have no path)
            SceneManager::get_singleton().store_current_scene_source_path(scene_source_path);
        }
        scene_backup_.clear();
        panels_manager_.get_panel_as<ui::SceneView>("Scene View").focus();
        if (auto actor_instance = SceneManager::get_singleton().current_scene()->find_entity_by_name(focused_actor_id)) {
            panels_manager_.get_panel_as<ui::Inspector>("Inspector").focus_entity(actor_instance);
        }
    }
}

void EditorActions::next_frame() {
    if (editor_mode_ == EditorMode::PLAY || editor_mode_ == EditorMode::PAUSE)
        set_editor_mode(EditorMode::FRAME_BY_FRAME);
}

//MARK: - Entity_CREATION_DESTRUCTION
Vector3F EditorActions::calculate_entity_spawn_point(float distance_to_camera) {
    return Vector3F();
}

Entity *EditorActions::create_empty_entity(bool focus_on_creation, Entity *parent, const std::string &name) {
    return nullptr;
}

Entity *EditorActions::create_entity_with_model(const std::string &path, bool focus_on_creation,
                                                Entity *parent, const std::string &name) {
    return nullptr;
}

bool EditorActions::destroy_entity(Entity *entity) {
    return false;
}

void EditorActions::duplicate_entity(Entity *to_duplicate, Entity *forced_parent, bool focus) {
    
}

//MARK: - ENTITY_MANIPULATION
void EditorActions::select_entity(Entity *target) {
    
}

void EditorActions::unselect_entity() {
    
}

bool EditorActions::is_any_entity_selected() const {
    return false;
}

Entity *EditorActions::get_selected_entity() const {
    return nullptr;
}

void EditorActions::move_to_target(Entity *target) {
    
}

//MARK: - RESOURCE_MANAGEMENT
void EditorActions::compile_shaders() {
    
}

void EditorActions::save_materials() {
    
}

bool EditorActions::import_asset(const std::string &initial_destination_directory) {
    return false;
}

bool EditorActions::import_asset_at_location(const std::string &destination) {
    return false;
}

std::string EditorActions::get_real_path(const std::string &path) {
    return "";
}

std::string EditorActions::get_resource_path(const std::string &path, bool is_from_engine) {
    return "";
}

std::string EditorActions::get_script_path(const std::string &path) {
    return "";
}

void EditorActions::propagate_folder_rename(const std::string &previous_name, const std::string &new_name) {
    
}

void EditorActions::propagate_folder_destruction(const std::string &folder_path) {
    
}

void EditorActions::propagate_script_rename(const std::string &previous_name, const std::string &new_name) {
    
}

void EditorActions::propagate_file_rename(const std::string &previous_name, const std::string &new_name) {
    
}

void EditorActions::propagate_file_rename_through_saved_files_of_type(const std::string &previous_name, const std::string &new_name,
                                                                      fs::FileType file_type) {
    
}

}
}
