//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "editor_actions.h"
#include "scene_manager.h"
#include "view/scene_view.h"
#include "ui/inspector.h"
#include "lua/script_interpreter.h"

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
    LOGI("Scene loaded from disk: {}", SceneManager::get_singleton().current_scene_source_path());
    panels_manager_.get_panel_as<ui::SceneView>("Scene View").focus();
}

bool EditorActions::is_current_scene_loaded_from_disk() const {
    return SceneManager::get_singleton().is_current_scene_loaded_from_disk();
}

void EditorActions::save_scene_changes() {
    if (is_current_scene_loaded_from_disk()) {
        save_current_scene_to(SceneManager::get_singleton().current_scene_source_path());
        LOGI("Current scene saved to: {}" + SceneManager::get_singleton().current_scene_source_path());
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

//MARK: - TOOLS
ui::PanelsManager &EditorActions::panels_manager() {
    return panels_manager_;
}

//MARK: - SETTINGS
void EditorActions::set_entity_spawn_at_origin(bool value) {
    
}

void EditorActions::set_entity_spawn_mode(EntitySpawnMode value) {
    
}

void EditorActions::reset_layout() {
    
}

void EditorActions::set_scene_view_camera_speed(int speed) {
    
}

int EditorActions::scene_view_camera_speed() {
    return 0;
}

void EditorActions::set_asset_view_camera_speed(int speed) {
    
}

int EditorActions::asset_view_camera_speed() {
    return 0;
}

void EditorActions::reset_scene_view_camera_position() {
    
}

void EditorActions::reset_asset_view_camera_position() {
    
}

//MARK: - GAME
EditorActions::EditorMode EditorActions::current_editor_mode() const {
    return editor_mode_;
}

void EditorActions::set_editor_mode(EditorMode new_editor_mode) {
    
}

void EditorActions::start_playing() {
    
}

void EditorActions::pause_game() {
    
}

void EditorActions::stop_playing() {
    
}

void EditorActions::next_frame() {
    
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

//MARK: - BUILDING
std::optional<std::string> EditorActions::select_build_folder() {
    return std::nullopt;
}

void EditorActions::build(bool auto_run, bool temp_folder) {
    
}

void EditorActions::build_at_location(const std::string &configuration, const std::string &build_path, bool auto_run) {
    
}

//MARK: - ACTION_SYSTEM
void EditorActions::delay_action(const std::function<void()> &action, uint32_t frames) {
    
}

void EditorActions::execute_delayed_actions() {
    
}

}
}
