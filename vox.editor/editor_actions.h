//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.editor/panels_manager.h"
#include "vox.math/point3.h"
#include "vox.render/platform/filesystem.h"
#include "vox.render/scene_forward.h"
#include "vox.render/singleton.h"

namespace vox {
namespace editor {
class EditorApplication;
/**
 * A set of editor actions
 */
class EditorActions : public Singleton<EditorActions> {
public:
    static EditorActions &GetSingleton();

    static EditorActions *GetSingletonPtr();

    /**
     * Constructor
     */
    explicit EditorActions(EditorApplication &app);

    // MARK: - TOOLS
public:
    /**
     * Returns the panels manager
     */
    ui::PanelsManager &PanelsManager();

    // MARK: - SETTINGS
public:
    /**
     * A simple enumeration that define two entity spawn modes
     */
    enum class EntitySpawnMode { ORIGIN, FRONT };

    /**
     * Defines if new entities should be spawned at origin
     */
    void SetEntitySpawnAtOrigin(bool value);

    /**
     * Defines how new entities should be spawned
     */
    void SetEntitySpawnMode(EntitySpawnMode value);

    /**
     * Reset the editor layout
     */
    void ResetLayout();

    /**
     * Defines the scene view camera speed
     */
    void SetSceneViewCameraSpeed(int speed);

    /**
     * Returns the scene view camera speed
     */
    int SceneViewCameraSpeed();

    /**
     * Defines the asset view camera speed
     */
    void SetAssetViewCameraSpeed(int speed);

    /**
     * Returns the asset view camera speed
     */
    int AssetViewCameraSpeed();

    /**
     * Resets the scene view camera position to the default one
     */
    void ResetSceneViewCameraPosition();

    /**
     * Resets the scene view camera position to the default one
     */
    void ResetAssetViewCameraPosition();

    // MARK: - GAME
public:
    /**
     * Defines some states for the editor
     */
    enum class EditorMode { EDIT, PLAY, PAUSE, FRAME_BY_FRAME };

    /**
     * Returns the current editor state/mode
     */
    [[nodiscard]] EditorMode CurrentEditorMode() const;

    /**
     * Defines the editor state/mode
     */
    void SetEditorMode(EditorMode new_editor_mode);

    /**
     * Start playing the current scene and update the editor mode
     */
    void StartPlaying();

    /**
     * Pause the current playing scene and update the editor mode
     */
    void PauseGame();

    /**
     * Stop playing the current scene and update the editor mode
     */
    void StopPlaying();

    /**
     * Play the current frame and pause the editor
     */
    void NextFrame();

    // MARK: - Entity_CREATION_DESTRUCTION
public:
    /**
     * Create an entity with the given component type
     */
    template <typename T>
    Entity *CreateMonoComponentEntity(bool focus_on_creation = true, Entity *parent = nullptr);

    /**
     * Calculate the position where to spawn the entity using the current camera position and forward
     */
    Point3F CalculateEntitySpawnPoint(float distance_to_camera);

    /**
     * Create an empty entity
     */
    Entity *CreateEmptyEntity(bool focus_on_creation = true, Entity *parent = nullptr, const std::string &name = "");

    /**
     * Create an entity with a model renderer and a material renderer. The model renderer with use the model identified
     * by the given path
     */
    Entity *CreateEntityWithModel(const std::string &path,
                                  bool focus_on_creation = true,
                                  Entity *parent = nullptr,
                                  const std::string &name = "");

    /**
     * Destroy an entity from his scene
     */
    bool DestroyEntity(Entity *entity);

    /**
     * Duplicate an entity
     */
    void DuplicateEntity(Entity *to_duplicate, Entity *forced_parent = nullptr, bool focus = true);

    // MARK: - ENTITY_MANIPULATION
public:
    /**
     * Select an entity and show him in inspector
     */
    void SelectEntity(Entity *target);

    /**
     * Unselect any selected entity and clearing the inspector
     */
    void UnselectEntity();

    /**
     * Returns true if any entity is selected
     */
    [[nodiscard]] bool IsAnyEntitySelected() const;

    /**
     * Returns the selected entity. Make sur you verified that an entity is selected
     * with IsAnyEntitySelected() before calling this method
     */
    [[nodiscard]] Entity *GetSelectedEntity() const;

    /**
     * Moves the camera to the target entity
     */
    void MoveToTarget(Entity *target);

    // MARK: - RESOURCE_MANAGEMENT
public:
    /**
     * Compile every loaded shaders
     */
    void CompileShaders();

    /**
     * Save every materials to their respective files
     */
    void SaveMaterials();

    /**
     * Import an asset
     */
    bool ImportAsset(const std::string &initial_destination_directory);

    /**
     * Import an asset at location
     */
    bool ImportAssetAtLocation(const std::string &destination);

    /**
     * Returns the real path of a resource (complete absolute path)
     */
    std::string GetRealPath(const std::string &path);

    /**
     * Returns the resource path of a file
     */
    std::string GetResourcePath(const std::string &path, bool is_from_engine = false);

    /**
     * Returns the script path of a file
     */
    std::string GetScriptPath(const std::string &path);

    /**
     * Propagate the folder rename everywhere (Resource manager, scenes, materials...)
     */
    void PropagateFolderRename(const std::string &previous_name, const std::string &new_name);

    /**
     * Propagate the folder destruction everywhere (Resource manager, scenes, materials...)
     */
    void PropagateFolderDestruction(const std::string &folder_path);

    /**
     * Propagate the script rename in scenes and inspector
     */
    void PropagateScriptRename(const std::string &previous_name, const std::string &new_name);

    /**
     * Propagate the file rename everywhere it is used
     */
    void PropagateFileRename(const std::string &previous_name, const std::string &new_name);

    /**
     * Propagate the file rename through concerned files
     */
    void PropagateFileRenameThroughSavedFilesOfType(const std::string &previous_name,
                                                    const std::string &new_name,
                                                    fs::FileType file_type);

    // MARK: - SCENE
public:
    /**
     * Load an empty scene. Any unsaved changes to the current scene will be discarded
     */
    void LoadEmptyScene();

    /**
     * Save the current scene to the given path
     */
    void SaveCurrentSceneTo(const std::string &path);

    /**
     * Load a scene from the disk
     */
    void LoadSceneFromDisk(const std::string &path, bool absolute = false);

    /**
     * Returns true if the current scene has been loaded from disk
     */
    [[nodiscard]] bool IsCurrentSceneLoadedFromDisk() const;

    /**
     * Save the current scene to its disk location
     */
    void SaveSceneChanges();

    /**
     * Save the current scene to a new disk location (Can create a duplication of the scene file)
     */
    void SaveAs();

    // MARK: - SCRIPTING
public:
    /**
     * Refresh every scripts (Re-interpret)
     */
    void RefreshScripts();

    // MARK: - BUILDING
public:
    /**
     * Ask the user to select the build folder
     */
    std::optional<std::string> SelectBuildFolder();

    /**
     * Build the current project
     */
    void Build(bool auto_run = false, bool temp_folder = false);

    /**
     * Build the current project at the given location
     */
    void BuildAtLocation(const std::string &configuration, const std::string &build_path, bool auto_run = false);

    // MARK: - ACTION_SYSTEM
    /**
     * Prepare an action for a future call
     */
    void DelayAction(const std::function<void()> &action, uint32_t frames = 1);

    /**
     * Execute every actions that should be executed at this frame (Decrement the frame counter for each actions)
     */
    void ExecuteDelayedActions();

public:
    Event<Entity *> entity_selected_event_;
    Event<Entity *> entity_unselected_event_;
    Event<EditorMode> editor_mode_changed_event_;
    Event<> play_event_;

private:
    EditorApplication &app_;

    EntitySpawnMode entity_spawn_mode_ = EntitySpawnMode::ORIGIN;
    EditorMode editor_mode_ = EditorMode::EDIT;

    std::vector<std::pair<uint32_t, std::function<void()>>> delayed_actions_;

    nlohmann::json scene_backup_;
};

}  // namespace editor
template <>
inline editor::EditorActions *Singleton<editor::EditorActions>::ms_singleton{nullptr};
}  // namespace vox
#include "vox.editor/editor_actions-inl.h"
