//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.editor/editor_actions.h"

#include "vox.editor/editor_application.h"
#include "vox.editor/ui/inspector.h"
#include "vox.editor/view/asset_view.h"
#include "vox.editor/view/game_view.h"
#include "vox.editor/view/scene_view.h"
#include "vox.render/lua/script_interpreter.h"
#include "vox.render/scene_manager.h"
#include "vox.render/ui/ui_manager.h"

namespace vox {
editor::EditorActions *editor::EditorActions::GetSingletonPtr() { return ms_singleton; }

editor::EditorActions &editor::EditorActions::GetSingleton() {
    assert(ms_singleton);
    return (*ms_singleton);
}

namespace editor {
EditorActions::EditorActions(EditorApplication &app) : app_(app) {}

// MARK: - SCENE
void EditorActions::LoadEmptyScene() {
    if (CurrentEditorMode() != EditorMode::EDIT) StopPlaying();

    SceneManager::GetSingleton().LoadEmptyLightedScene();
    LOGI("New scene created")
}

void EditorActions::SaveCurrentSceneTo(const std::string &path) {
    SceneManager::GetSingleton().StoreCurrentSceneSourcePath(path);
    nlohmann::json root;
    SceneManager::GetSingleton().CurrentScene()->OnSerialize(root);

    nlohmann::json j = {
            {"root", root},
    };
    fs::WriteJson(j, path);
}

void EditorActions::LoadSceneFromDisk(const std::string &path, bool absolute) {
    if (CurrentEditorMode() != EditorMode::EDIT) StopPlaying();

    SceneManager::GetSingleton().LoadScene(path, absolute);
    LOGI("Scene loaded from disk: {}", SceneManager::GetSingleton().CurrentSceneSourcePath())
    app_.panels_manager_.GetPanelAs<ui::SceneView>("Scene View").Focus();
}

bool EditorActions::IsCurrentSceneLoadedFromDisk() const {
    return SceneManager::GetSingleton().IsCurrentSceneLoadedFromDisk();
}

void EditorActions::SaveSceneChanges() {
    if (IsCurrentSceneLoadedFromDisk()) {
        SaveCurrentSceneTo(SceneManager::GetSingleton().CurrentSceneSourcePath());
        LOGI("Current scene saved to: {}" + SceneManager::GetSingleton().CurrentSceneSourcePath())
    } else {
        SaveAs();
    }
}

void EditorActions::SaveAs() {
    // todo
}

// MARK: - SCRIPTING
void EditorActions::RefreshScripts() {
    ScriptInterpreter::GetSingleton().RefreshAll();
    app_.panels_manager_.GetPanelAs<ui::Inspector>("Inspector").Refresh();
    if (ScriptInterpreter::GetSingleton().IsOk()) {
        LOGI("Scripts interpretation succeeded!")
    }
}

// MARK: - BUILDING
std::optional<std::string> EditorActions::SelectBuildFolder() {
    // todo
    return std::nullopt;
}

void EditorActions::Build(bool auto_run, bool temp_folder) {
    // todo
}

void EditorActions::BuildAtLocation(const std::string &configuration, const std::string &build_path, bool auto_run) {
    // todo
}

// MARK: - ACTION_SYSTEM
void EditorActions::DelayAction(const std::function<void()> &action, uint32_t frames) {
    delayed_actions_.emplace_back(frames + 1, action);
}

void EditorActions::ExecuteDelayedActions() {
    std::for_each(delayed_actions_.begin(), delayed_actions_.end(),
                  [](std::pair<uint32_t, std::function<void()>> &element) {
                      --element.first;

                      if (element.first == 0) element.second();
                  });

    delayed_actions_.erase(
            std::remove_if(delayed_actions_.begin(), delayed_actions_.end(),
                           [](std::pair<uint32_t, std::function<void()>> &element) { return element.first == 0; }),
            delayed_actions_.end());
}

// MARK: - TOOLS
ui::PanelsManager &EditorActions::PanelsManager() { return app_.panels_manager_; }

// MARK: - SETTINGS
void EditorActions::SetEntitySpawnAtOrigin(bool value) {
    if (value)
        entity_spawn_mode_ = EntitySpawnMode::ORIGIN;
    else
        entity_spawn_mode_ = EntitySpawnMode::FRONT;
}

void EditorActions::SetEntitySpawnMode(EntitySpawnMode value) { entity_spawn_mode_ = value; }

void EditorActions::ResetLayout() {
    DelayAction([]() { UiManager::GetSingleton().ResetLayout("Config\\layout.ini"); });
}

void EditorActions::SetSceneViewCameraSpeed(int speed) {
    auto orbit_control = app_.panels_manager_.GetPanelAs<ui::SceneView>("Scene View").CameraControl();
    orbit_control->rotate_speed_ = speed;
    orbit_control->zoom_speed_ = speed;
}

int EditorActions::SceneViewCameraSpeed() {
    return app_.panels_manager_.GetPanelAs<ui::SceneView>("Scene View").CameraControl()->rotate_speed_;
}

void EditorActions::SetAssetViewCameraSpeed(int speed) {
    auto orbit_control = app_.panels_manager_.GetPanelAs<ui::AssetView>("Asset View").CameraControl();
    orbit_control->rotate_speed_ = speed;
    orbit_control->zoom_speed_ = speed;
}

int EditorActions::AssetViewCameraSpeed() {
    return app_.panels_manager_.GetPanelAs<ui::AssetView>("Asset View").CameraControl()->rotate_speed_;
}

void EditorActions::ResetSceneViewCameraPosition() {
    auto orbit_control = app_.panels_manager_.GetPanelAs<ui::SceneView>("Scene View").CameraControl();
    orbit_control->GetEntity()->transform->SetPosition({-10.0f, 4.0f, 10.0f});
}

void EditorActions::ResetAssetViewCameraPosition() {
    auto orbit_control = app_.panels_manager_.GetPanelAs<ui::AssetView>("Asset View").CameraControl();
    orbit_control->GetEntity()->transform->SetPosition({-10.0f, 4.0f, 10.0f});
}

// MARK: - GAME
EditorActions::EditorMode EditorActions::CurrentEditorMode() const { return editor_mode_; }

void EditorActions::SetEditorMode(EditorMode new_editor_mode) {
    editor_mode_ = new_editor_mode;
    editor_mode_changed_event_.Invoke(editor_mode_);
}

void EditorActions::StartPlaying() {
    if (editor_mode_ == EditorMode::EDIT) {
        ScriptInterpreter::GetSingleton().RefreshAll();
        app_.panels_manager_.GetPanelAs<ui::Inspector>("Inspector").Refresh();

        if (ScriptInterpreter::GetSingleton().IsOk()) {
            play_event_.Invoke();
            scene_backup_.clear();

            nlohmann::json root;
            SceneManager::GetSingleton().CurrentScene()->OnSerialize(root);
            scene_backup_.insert(scene_backup_.begin(), {"root", root});

            app_.panels_manager_.GetPanelAs<ui::GameView>("Game View").Focus();
            SceneManager::GetSingleton().CurrentScene()->Play();
            SetEditorMode(EditorMode::PLAY);
        }
    } else {
        // m_context.audioEngine->Unsuspend();
        SetEditorMode(EditorMode::PLAY);
    }
}

void EditorActions::PauseGame() { SetEditorMode(EditorMode::PAUSE); }

void EditorActions::StopPlaying() {
    if (editor_mode_ != EditorMode::EDIT) {
        // ImGui::GetIO().DisableMouseUpdate = false;
        // m_context.window->SetCursorMode(OvWindowing::Cursor::ECursorMode::NORMAL);
        SetEditorMode(EditorMode::EDIT);
        bool loaded_from_disk = SceneManager::GetSingleton().IsCurrentSceneLoadedFromDisk();
        std::string scene_source_path = SceneManager::GetSingleton().CurrentSceneSourcePath();

        std::string focused_actor_id;

        if (auto target_actor = app_.panels_manager_.GetPanelAs<ui::Inspector>("Inspector").TargetEntity())
            focused_actor_id = target_actor->name;

        SceneManager::GetSingleton().LoadSceneFromMemory(scene_backup_);
        if (loaded_from_disk) {
            // To bo able to save or reload the scene whereas the scene is loaded from memory (Supposed to have no path)
            SceneManager::GetSingleton().StoreCurrentSceneSourcePath(scene_source_path);
        }
        scene_backup_.clear();
        app_.panels_manager_.GetPanelAs<ui::SceneView>("Scene View").Focus();
        if (auto actor_instance = SceneManager::GetSingleton().CurrentScene()->FindEntityByName(focused_actor_id)) {
            app_.panels_manager_.GetPanelAs<ui::Inspector>("Inspector").FocusEntity(actor_instance);
        }
    }
}

void EditorActions::NextFrame() {
    if (editor_mode_ == EditorMode::PLAY || editor_mode_ == EditorMode::PAUSE)
        SetEditorMode(EditorMode::FRAME_BY_FRAME);
}

// MARK: - Entity_CREATION_DESTRUCTION
Point3F EditorActions::CalculateEntitySpawnPoint(float distance_to_camera) {
    auto camera_entity = app_.panels_manager_.GetPanelAs<ui::SceneView>("Scene View").CameraControl()->GetEntity();
    return camera_entity->transform->WorldPosition() + camera_entity->transform->WorldRotationQuaternion() *
                                                               camera_entity->transform->WorldForward() *
                                                               distance_to_camera;
}

Entity *EditorActions::CreateEmptyEntity(bool focus_on_creation, Entity *parent, const std::string &name) {
    const auto kCurrentScene = SceneManager::GetSingleton().CurrentScene();
    Entity *entity{nullptr};
    if (parent) {
        entity = parent->CreateChild(name);
    } else {
        entity = kCurrentScene->CreateRootEntity(name);
    }

    if (entity_spawn_mode_ == EntitySpawnMode::FRONT)
        entity->transform->SetWorldPosition(CalculateEntitySpawnPoint(10.0f));

    if (focus_on_creation) SelectEntity(entity);

    LOGI("Entity created")

    return entity;
}

Entity *EditorActions::CreateEntityWithModel(const std::string &path,
                                             bool focus_on_creation,
                                             Entity *parent,
                                             const std::string &name) {
    return nullptr;
}

bool EditorActions::DestroyEntity(Entity *entity) {
    // entity.MarkAsDestroy();
    LOGI("Entity destroyed")
    return true;
}

void EditorActions::DuplicateEntity(Entity *to_duplicate, Entity *forced_parent, bool focus) {}

// MARK: - ENTITY_MANIPULATION
void EditorActions::SelectEntity(Entity *target) {
    app_.panels_manager_.GetPanelAs<ui::Inspector>("Inspector").FocusEntity(target);
}

void EditorActions::UnselectEntity() { app_.panels_manager_.GetPanelAs<ui::Inspector>("Inspector").UnFocus(); }

bool EditorActions::IsAnyEntitySelected() const {
    return app_.panels_manager_.GetPanelAs<ui::Inspector>("Inspector").TargetEntity();
}

Entity *EditorActions::GetSelectedEntity() const {
    return app_.panels_manager_.GetPanelAs<ui::Inspector>("Inspector").TargetEntity();
}

void EditorActions::MoveToTarget(Entity *target) {}

// MARK: - RESOURCE_MANAGEMENT
void EditorActions::CompileShaders() {}

void EditorActions::SaveMaterials() {}

bool EditorActions::ImportAsset(const std::string &initial_destination_directory) { return false; }

bool EditorActions::ImportAssetAtLocation(const std::string &destination) { return false; }

std::string EditorActions::GetRealPath(const std::string &path) {
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

std::string EditorActions::GetResourcePath(const std::string &path, bool is_from_engine) {
    std::string result = path;

    if (Replace(result, is_from_engine ? app_.engine_assets_path_ : app_.project_assets_path_, "")) {
        if (is_from_engine) result = ':' + result;
    }
    return result;
}

std::string EditorActions::GetScriptPath(const std::string &path) {
    std::string result = path;
    Replace(result, app_.project_scripts_path_, "");
    Replace(result, ".lua", "");
    return result;
}

void EditorActions::PropagateFolderRename(const std::string &previous_name, const std::string &new_name) {}

void EditorActions::PropagateFolderDestruction(const std::string &folder_path) {}

void EditorActions::PropagateScriptRename(const std::string &previous_name, const std::string &new_name) {}

void EditorActions::PropagateFileRename(const std::string &previous_name, const std::string &new_name) {}

void EditorActions::PropagateFileRenameThroughSavedFilesOfType(const std::string &previous_name,
                                                               const std::string &new_name,
                                                               fs::FileType file_type) {}

}  // namespace editor
}  // namespace vox
