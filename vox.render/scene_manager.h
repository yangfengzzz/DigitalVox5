//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.base/singleton.h"
#include "vox.render/event.h"
#include "vox.render/scene.h"

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
    void Update();

    /**
     * Load an play a scene with a delay
     * @param p_path p_path
     * @param p_absolute p_absolute
     */
    void LoadAndPlayDelayed(const std::string &p_path, bool p_absolute = false);

    /**
     * Load an empty scene in memory
     */
    void LoadEmptyScene();

    /**
     * Load an empty lighted scene in memory
     */
    void LoadEmptyLightedScene();

    /**
     * Load specific scene in memory
     * @param p_path p_path
     * @param p_absolute (If this setting is set to true, the scene loader will ignore the "SceneRootFolder" given on
     * SceneManager construction)
     */
    bool LoadScene(const std::string &p_path, bool p_absolute = false);

    /**
     * Load specific scene in memory
     */
    bool LoadSceneFromMemory(const nlohmann::json &data);

    /**
     * Destroy current scene from memory
     */
    void UnloadCurrentScene();

    /**
     * Return true if a scene is currently loaded
     */
    [[nodiscard]] bool HasCurrentScene() const;

    /*
     * Return current loaded scene
     */
    Scene *CurrentScene();

    /**
     * Return the current scene source path
     */
    [[nodiscard]] std::string CurrentSceneSourcePath() const;

    /**
     * Return true if the currently loaded scene has been loaded from a file
     */
    [[nodiscard]] bool IsCurrentSceneLoadedFromDisk() const;

    /**
     * Store the given path as the current scene source path
     * @param p_path p_path
     */
    void StoreCurrentSceneSourcePath(const std::string &p_path);

    /**
     * Reset the current scene source path to an empty string
     */
    void ForgetCurrentSceneSourcePath();

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

template <>
inline SceneManager *Singleton<SceneManager>::ms_singleton{nullptr};

}  // namespace vox
