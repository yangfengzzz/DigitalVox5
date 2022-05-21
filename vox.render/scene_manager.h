//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "singleton.h"
#include "scene.h"
#include "event.h"

namespace vox {
/**
 * The scene manager of the current scene
 */
class SceneManager : public Singleton<SceneManager> {
public:
    static SceneManager &GetSingleton();
    
    static SceneManager *GetSingletonPtr();
    
    /**
     * Default constructor
     * @param p_scene_root_folder (Optional)
     */
    explicit SceneManager(Device &device, std::string p_scene_root_folder = "");
    
    /**
     * Default destructor
     */
    ~SceneManager();
    
    /**
     * Update
     */
    void update();
    
    /**
     * Load an play a scene with a delay
     * @param p_path p_path
     * @param p_absolute p_absolute
     */
    void load_and_play_delayed(const std::string &p_path, bool p_absolute = false);
    
    /**
     * Load an empty scene in memory
     */
    void load_empty_scene();
    
    /**
     * Load an empty lighted scene in memory
     */
    void load_empty_lighted_scene();
    
    /**
     * Load specific scene in memory
     * @param p_path p_path
     * @param p_absolute (If this setting is set to true, the scene loader will ignore the "SceneRootFolder" given on SceneManager construction)
     */
    bool load_scene(const std::string &p_path, bool p_absolute = false);
    
    /**
     * Load specific scene in memory
     */
    bool load_scene_from_memory(const nlohmann::json &data);
    
    /**
     * Destroy current scene from memory
     */
    void unload_current_scene();
    
    /**
     * Return true if a scene is currently loaded
     */
    [[nodiscard]] bool has_current_scene() const;
    
    /*
     * Return current loaded scene
     */
    Scene *current_scene();
    
    /**
     * Return the current scene source path
     */
    [[nodiscard]] std::string current_scene_source_path() const;
    
    /**
     * Return true if the currently loaded scene has been loaded from a file
     */
    [[nodiscard]] bool is_current_scene_loaded_from_disk() const;
    
    /**
     * Store the given path as the current scene source path
     * @param p_path p_path
     */
    void store_current_scene_source_path(const std::string &p_path);
    
    /**
     * Reset the current scene source path to an empty string
     */
    void forget_current_scene_source_path();
    
public:
    Event<> scene_load_event_;
    Event<> scene_unload_event_;
    Event<const std::string &> current_scene_source_path_changed_event_;
    
private:
    Device &device_;
    const std::string scene_root_folder_;
    std::unique_ptr<Scene> current_scene_{nullptr};
    
    bool current_scene_loaded_from_path_{false};
    std::string current_scene_source_path_;
    
    std::function<void()> delayed_load_call_;
};

template<> inline SceneManager *Singleton<SceneManager>::ms_singleton_{nullptr};

}
