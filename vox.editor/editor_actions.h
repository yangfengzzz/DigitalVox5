//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "panels_manager.h"
#include "vector3.h"
#include "platform/filesystem.h"
#include "scene_forward.h"
#include "singleton.h"

namespace vox {
namespace editor {
/**
 * A set of editor actions
 */
class EditorActions : public Singleton<EditorActions> {
public:
    static EditorActions &get_singleton();
    
    static EditorActions *get_singleton_ptr();
    
    /**
     * Constructor
     */
    explicit EditorActions(ui::PanelsManager &panels_manager);
    
    //MARK: - TOOLS
public:
    /**
     * Returns the panels manager
     */
    ui::PanelsManager &panels_manager();
    
    //MARK: - SETTINGS
public:
    /**
     * A simple enumeration that define two entity spawn modes
     */
    enum class EntitySpawnMode { ORIGIN, FRONT };
    
    /**
     * Defines if new entities should be spawned at origin
     */
    void set_entity_spawn_at_origin(bool value);
    
    /**
     * Defines how new entities should be spawned
     */
    void set_entity_spawn_mode(EntitySpawnMode value);
    
    /**
     * Reset the editor layout
     */
    void reset_layout();
    
    /**
     * Defines the scene view camera speed
     */
    void set_scene_view_camera_speed(int speed);
    
    /**
     * Returns the scene view camera speed
     */
    int scene_view_camera_speed();
    
    /**
     * Defines the asset view camera speed
     */
    void set_asset_view_camera_speed(int speed);
    
    /**
     * Returns the asset view camera speed
     */
    int asset_view_camera_speed();
    
    /**
     * Resets the scene view camera position to the default one
     */
    void reset_scene_view_camera_position();
    
    /**
     * Resets the scene view camera position to the default one
     */
    void reset_asset_view_camera_position();
    
    //MARK: - GAME
public:
    /**
     * Defines some states for the editor
     */
    enum class EditorMode { EDIT, PLAY, PAUSE, FRAME_BY_FRAME };
    
    /**
     * Returns the current editor state/mode
     */
    [[nodiscard]] EditorMode current_editor_mode() const;
    
    /**
     * Defines the editor state/mode
     */
    void set_editor_mode(EditorMode new_editor_mode);
    
    /**
     * Start playing the current scene and update the editor mode
     */
    void start_playing();
    
    /**
     * Pause the current playing scene and update the editor mode
     */
    void pause_game();
    
    /**
     * Stop playing the current scene and update the editor mode
     */
    void stop_playing();
    
    /**
     * Play the current frame and pause the editor
     */
    void next_frame();
    
    //MARK: - Entity_CREATION_DESTRUCTION
public:
    /**
     * Create an entity with the given component type
     */
    template<typename T>
    Entity *create_mono_component_entity(bool focus_on_creation = true, Entity *parent = nullptr);
    
    /**
     * Calculate the position where to spawn the entity using the current camera position and forward
     */
    Vector3F calculate_entity_spawn_point(float distance_to_camera);
    
    /**
     * Create an empty entity
     */
    Entity *create_empty_entity(bool focus_on_creation = true, Entity *parent = nullptr,
                                const std::string &name = "");
    
    /**
     * Create an entity with a model renderer and a material renderer. The model renderer with use the model identified
     * by the given path
     */
    Entity *create_entity_with_model(const std::string &path, bool focus_on_creation = true,
                                     Entity *parent = nullptr, const std::string &name = "");
    
    /**
     * Destroy an entity from his scene
     */
    bool destroy_entity(Entity *entity);
    
    /**
     * Duplicate an entity
     */
    void duplicate_entity(Entity *to_duplicate, Entity *forced_parent = nullptr, bool focus = true);
    
    
    //MARK: - ENTITY_MANIPULATION
public:
    /**
     * Select an entity and show him in inspector
     */
    void select_entity(Entity *target);
    
    /**
     * Unselect any selected entity and clearing the inspector
     */
    void unselect_entity();
    
    /**
     * Returns true if any entity is selected
     */
    [[nodiscard]] bool is_any_entity_selected() const;
    
    /**
     * Returns the selected entity. Make sur you verified that an entity is selected
     * with IsAnyEntitySelected() before calling this method
     */
    [[nodiscard]] Entity *get_selected_entity() const;
    
    /**
     * Moves the camera to the target entity
     */
    void move_to_target(Entity *target);
    
    //MARK: - RESOURCE_MANAGEMENT
public:
    /**
     * Compile every loaded shaders
     */
    void compile_shaders();
    
    /**
     * Save every materials to their respective files
     */
    void save_materials();
    
    /**
     * Import an asset
     */
    bool import_asset(const std::string &initial_destination_directory);
    
    /**
     * Import an asset at location
     */
    bool import_asset_at_location(const std::string &destination);
    
    /**
     * Returns the real path of a resource (complete absolute path)
     */
    std::string get_real_path(const std::string &path);
    
    /**
     * Returns the resource path of a file
     */
    std::string get_resource_path(const std::string &path, bool is_from_engine = false);
    
    /**
     * Returns the script path of a file
     */
    std::string get_script_path(const std::string &path);
    
    /**
     * Propagate the folder rename everywhere (Resource manager, scenes, materials...)
     */
    void propagate_folder_rename(const std::string &previous_name, const std::string &new_name);
    
    /**
     * Propagate the folder destruction everywhere (Resource manager, scenes, materials...)
     */
    void propagate_folder_destruction(const std::string &folder_path);
    
    /**
     * Propagate the script rename in scenes and inspector
     */
    void propagate_script_rename(const std::string &previous_name, const std::string &new_name);
    
    /**
     * Propagate the file rename everywhere it is used
     */
    void propagate_file_rename(const std::string &previous_name, const std::string &new_name);
    
    /**
     * Propagate the file rename through concerned files
     */
    void propagate_file_rename_through_saved_files_of_type(const std::string &previous_name, const std::string &new_name,
                                                           fs::FileType file_type);
    
    
    //MARK: - SCENE
public:
    /**
     * Load an empty scene. Any unsaved changes to the current scene will be discarded
     */
    void load_empty_scene();
    
    /**
     * Save the current scene to the given path
     */
    void save_current_scene_to(const std::string &path);
    
    /**
     * Load a scene from the disk
     */
    void load_scene_from_disk(const std::string &path, bool absolute = false);
    
    /**
     * Returns true if the current scene has been loaded from disk
     */
    [[nodiscard]] bool is_current_scene_loaded_from_disk() const;
    
    /**
     * Save the current scene to its disk location
     */
    void save_scene_changes();
    
    /**
     * Save the current scene to a new disk location (Can create a duplication of the scene file)
     */
    void save_as();
    
    //MARK: - SCRIPTING
public:
    /**
     * Refresh every scripts (Re-interpret)
     */
    void refresh_scripts();
    
    
    //MARK: - BUILDING
public:
    /**
     * Ask the user to select the build folder
     */
    std::optional<std::string> select_build_folder();
    
    /**
     * Build the current project
     */
    void build(bool auto_run = false, bool temp_folder = false);
    
    /**
     * Build the current project at the given location
     */
    void build_at_location(const std::string &configuration, const std::string &build_path, bool auto_run = false);
    
    //MARK: - ACTION_SYSTEM
    /**
     * Prepare an action for a future call
     */
    void delay_action(const std::function<void()> &action, uint32_t frames = 1);
    
    /**
     * Execute every actions that should be executed at this frame (Decrement the frame counter for each actions)
     */
    void execute_delayed_actions();
    
public:
    Event<Entity *> entity_selected_event_;
    Event<Entity *> entity_unselected_event_;
    Event<EditorMode> editor_mode_changed_event_;
    Event<> play_event_;
    
private:
    ui::PanelsManager &panels_manager_;
    
    EntitySpawnMode entity_spawn_mode_ = EntitySpawnMode::ORIGIN;
    EditorMode editor_mode_ = EditorMode::EDIT;
    
    std::vector<std::pair<uint32_t, std::function<void()>>> delayed_actions_;
    
    nlohmann::json scene_backup_;
};

}
template<> inline editor::EditorActions *Singleton<editor::EditorActions>::ms_singleton_{nullptr};
}
#include "editor_actions-inl.h"
