//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "editor_actions.h"
#include "editor_application.h"
#include "scene_manager.h"
#include "lua/script_interpreter.h"
#include "view/scene_view.h"
#include "view/asset_view.h"
#include "view/game_view.h"
#include "ui/inspector.h"
#include "ui/ui_manager.h"

namespace vox {
editor::EditorActions *editor::EditorActions::GetSingletonPtr() {
    return ms_singleton;
}

editor::EditorActions &editor::EditorActions::GetSingleton() {
    assert(ms_singleton);
    return (*ms_singleton);
}

namespace editor {
EditorActions::EditorActions(EditorApplication &app) :
app_(app) {
}

//MARK: - SCENE
void EditorActions::load_empty_scene() {
    if (current_editor_mode() != EditorMode::EDIT)
        stop_playing();

    SceneManager::GetSingleton().LoadEmptyLightedScene();
    LOGI("New scene created")
}

void EditorActions::save_current_scene_to(const std::string &path) {
    SceneManager::GetSingleton().StoreCurrentSceneSourcePath(path);
    nlohmann::json root;
    SceneManager::GetSingleton().CurrentScene()->on_serialize(root);
    
    nlohmann::json j = {
        {"root", root},
    };
    fs::WriteJson(j, path);
}

void EditorActions::load_scene_from_disk(const std::string &path, bool absolute) {
    if (current_editor_mode() != EditorMode::EDIT)
        stop_playing();

    SceneManager::GetSingleton().LoadScene(path, absolute);
    LOGI("Scene loaded from disk: {}", SceneManager::GetSingleton().CurrentSceneSourcePath())
    app_.panels_manager_.get_panel_as<ui::SceneView>("Scene View").focus();
}

bool EditorActions::is_current_scene_loaded_from_disk() const {
    return SceneManager::GetSingleton().IsCurrentSceneLoadedFromDisk();
}

void EditorActions::save_scene_changes() {
    if (is_current_scene_loaded_from_disk()) {
        save_current_scene_to(SceneManager::GetSingleton().CurrentSceneSourcePath());
        LOGI("Current scene saved to: {}" + SceneManager::GetSingleton().CurrentSceneSourcePath())
    } else {
        save_as();
    }
}

void EditorActions::save_as() {
    // todo
}

//MARK: - SCRIPTING
void EditorActions::refresh_scripts() {
    ScriptInterpreter::GetSingleton().refresh_all();
    app_.panels_manager_.get_panel_as<ui::Inspector>("Inspector").refresh();
    if (ScriptInterpreter::GetSingleton().is_ok()) {
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
    return app_.panels_manager_;
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
        UiManager::GetSingleton().reset_layout("Config\\layout.ini");
    });
}

void EditorActions::set_scene_view_camera_speed(int speed) {
    auto orbit_control = app_.panels_manager_.get_panel_as<ui::SceneView>("Scene View").camera_control();
    orbit_control->rotate_speed_ = speed;
    orbit_control->zoom_speed_ = speed;
}

int EditorActions::scene_view_camera_speed() {
    return app_.panels_manager_.get_panel_as<ui::SceneView>("Scene View").camera_control()->rotate_speed_;
}

void EditorActions::set_asset_view_camera_speed(int speed) {
    auto orbit_control = app_.panels_manager_.get_panel_as<ui::AssetView>("Asset View").camera_control();
    orbit_control->rotate_speed_ = speed;
    orbit_control->zoom_speed_ = speed;
}

int EditorActions::asset_view_camera_speed() {
    return app_.panels_manager_.get_panel_as<ui::AssetView>("Asset View").camera_control()->rotate_speed_;
}

void EditorActions::reset_scene_view_camera_position() {
    auto orbit_control = app_.panels_manager_.get_panel_as<ui::SceneView>("Scene View").camera_control();
    orbit_control->entity()->transform->SetPosition({-10.0f, 4.0f, 10.0f});
}

void EditorActions::reset_asset_view_camera_position() {
    auto orbit_control = app_.panels_manager_.get_panel_as<ui::AssetView>("Asset View").camera_control();
    orbit_control->entity()->transform->SetPosition({-10.0f, 4.0f, 10.0f});
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
        ScriptInterpreter::GetSingleton().refresh_all();
        app_.panels_manager_.get_panel_as<ui::Inspector>("Inspector").refresh();
        
        if (ScriptInterpreter::GetSingleton().is_ok()) {
            play_event_.invoke();
            scene_backup_.clear();
            
            nlohmann::json root;
            SceneManager::GetSingleton().CurrentScene()->on_serialize(root);
            scene_backup_.insert(scene_backup_.begin(), {"root", root});
            
            app_.panels_manager_.get_panel_as<ui::GameView>("Game View").focus();
            SceneManager::GetSingleton().CurrentScene()->play();
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
        bool loaded_from_disk = SceneManager::GetSingleton().IsCurrentSceneLoadedFromDisk();
        std::string scene_source_path = SceneManager::GetSingleton().CurrentSceneSourcePath();
        
        std::string focused_actor_id;
        
        if (auto target_actor = app_.panels_manager_.get_panel_as<ui::Inspector>("Inspector").target_entity())
            focused_actor_id = target_actor->name_;

        SceneManager::GetSingleton().LoadSceneFromMemory(scene_backup_);
        if (loaded_from_disk) {
            // To bo able to save or reload the scene whereas the scene is loaded from memory (Supposed to have no path)
            SceneManager::GetSingleton().StoreCurrentSceneSourcePath(scene_source_path);
        }
        scene_backup_.clear();
        app_.panels_manager_.get_panel_as<ui::SceneView>("Scene View").focus();
        if (auto actor_instance = SceneManager::GetSingleton().CurrentScene()->FindEntityByName(focused_actor_id)) {
            app_.panels_manager_.get_panel_as<ui::Inspector>("Inspector").focus_entity(actor_instance);
        }
    }
}

void EditorActions::next_frame() {
    if (editor_mode_ == EditorMode::PLAY || editor_mode_ == EditorMode::PAUSE)
        set_editor_mode(EditorMode::FRAME_BY_FRAME);
}

//MARK: - Entity_CREATION_DESTRUCTION
Point3F EditorActions::calculate_entity_spawn_point(float distance_to_camera) {
    auto camera_entity = app_.panels_manager_.get_panel_as<ui::SceneView>("Scene View").camera_control()->entity();
    return camera_entity->transform->WorldPosition() + camera_entity->transform->WorldRotationQuaternion()
    *
                                                                camera_entity->transform->WorldForward() * distance_to_camera;
}

Entity *EditorActions::create_empty_entity(bool focus_on_creation, Entity *parent, const std::string &name) {
    const auto kCurrentScene = SceneManager::GetSingleton().CurrentScene();
    Entity* entity{nullptr};
    if (parent) {
        entity = parent->CreateChild(name);
    } else {
        entity = kCurrentScene->CreateRootEntity(name);
    }
    
    if (entity_spawn_mode_ == EntitySpawnMode::FRONT)
        entity->transform->SetWorldPosition(calculate_entity_spawn_point(10.0f));
    
    if (focus_on_creation)
        select_entity(entity);
    
    LOGI("Entity created")
    
    return entity;
}

Entity *EditorActions::create_entity_with_model(const std::string &path, bool focus_on_creation,
                                                Entity *parent, const std::string &name) {
    return nullptr;
}

bool EditorActions::destroy_entity(Entity *entity) {
    // entity.MarkAsDestroy();
    LOGI("Entity destroyed")
    return true;
}

void EditorActions::duplicate_entity(Entity *to_duplicate, Entity *forced_parent, bool focus) {
    
}

//MARK: - ENTITY_MANIPULATION
void EditorActions::select_entity(Entity *target) {
    app_.panels_manager_.get_panel_as<ui::Inspector>("Inspector").focus_entity(target);
}

void EditorActions::unselect_entity() {
    app_.panels_manager_.get_panel_as<ui::Inspector>("Inspector").un_focus();
}

bool EditorActions::is_any_entity_selected() const {
    return app_.panels_manager_.get_panel_as<ui::Inspector>("Inspector").target_entity();
}

Entity *EditorActions::get_selected_entity() const {
    return app_.panels_manager_.get_panel_as<ui::Inspector>("Inspector").target_entity();
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
    std::string result;

    // The path is an engine path
    if (path[0] == ':') {
        result = app_.engine_assets_path_ + std::string(path.data() + 1, path.data() + path.size());
    } else {
        // The path is a project path
        result = app_.project_assets_path_ + path;
    }

    return result;
}

std::string EditorActions::get_resource_path(const std::string &path, bool is_from_engine) {
    std::string result = path;

    if (replace(result, is_from_engine ? app_.engine_assets_path_ : app_.project_assets_path_, "")) {
        if (is_from_engine)
            result = ':' + result;
    }
    return result;
}

std::string EditorActions::get_script_path(const std::string &path) {
    std::string result = path;
    replace(result, app_.project_scripts_path_, "");
    replace(result, ".lua", "");
    return result;
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
