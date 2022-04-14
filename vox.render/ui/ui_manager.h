//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef DIGITALVOX_VOX_RENDER_UI_UI_MANAGER_H_
#define DIGITALVOX_VOX_RENDER_UI_UI_MANAGER_H_

#include <string>
#include <unordered_map>
#include "ui/canvas.h"
#include "rendering/render_context.h"

struct GLFWwindow;

namespace vox::ui {
/**
 * Some styles that you can use to modify the UI looks
 */
enum class Style {
    IM_CLASSIC_STYLE,
    IM_DARK_STYLE,
    IM_LIGHT_STYLE,
    // https://www.unknowncheats.me/forum/direct3d/189635-imgui-style-settings.html
    DUNE_DARK,
    ALTERNATIVE_DARK
};

/**
 * Handle the creation and drawing of the UI
 */
class UiManager {
public:
    /**
     * Create the UI manager. Will setup ImGui internally\
     * @param p_glfw_window p_glfw_window
     * @param p_style p_style
     */
    UiManager(GLFWwindow *p_glfw_window,
              RenderContext *context, Style p_style = Style::IM_DARK_STYLE);
    
    /**
     * Destroy the UI manager. Will handle ImGui destruction internally
     */
    ~UiManager();
    
    /**
     * Apply a new style to the UI elements
     * @param p_style p_style
     */
    static void apply_style(Style p_style);
    
    /**
     * Load a font (Returns true on success)
     * @param p_id p_id
     * @param p_path p_path
     * @param p_font_size p_font_size
     */
    bool load_font(const std::string &p_id, const std::string &p_path, float p_font_size);
    
    /**
     * Unload a font (Returns true on success)
     * @param p_id p_id
     */
    bool unload_font(const std::string &p_id);
    
    /**
     * Set the given font as the current one (Returns true on success)
     */
    bool use_font(const std::string &p_id);
    
    /**
     * Use the default font (ImGui default font)
     */
    static void use_default_font();
    
    /**
     * Allow the user to enable/disable .ini generation to save his editor layout
     * @param p_value p_value
     */
    void enable_editor_layout_save(bool p_value);
    
    /**
     *  Return true if the editor layout save system is on
     */
    [[nodiscard]] static bool is_editor_layout_save_enabled();
    
    /**
     * Defines a filename for the editor layout save file
     */
    void set_editor_layout_save_filename(const std::string &p_filename);
    
    /**
     * Defines a frequency (in seconds) for the auto saving system of the editor layout
     * @param p_frequency p_frequency
     */
    static void set_editor_layout_autosave_frequency(float p_frequency);
    
    /**
     * Returns the current frequency (in seconds) for the auto saving system of the editor layout
     */
    static float editor_layout_autosave_frequency(float p_frequeny);
    
    /**
     * Enable the docking system
     * @param p_value p_value
     */
    void enable_docking(bool p_value);
    
    /**
     * Reset the UI layout to the given configuration file
     * @param p_config p_config
     */
    static void reset_layout(const std::string &p_config);
    
    /**
     * Return true if the docking system is enabled
     */
    [[nodiscard]] bool is_docking_enabled() const;
    
    /**
     * Defines the canvas to use
     * @param p_canvas p_canvas
     */
    void set_canvas(Canvas &p_canvas);
    
    /**
     * Stop considering the current canvas (if any)
     */
    void remove_canvas();
    
private:
    void push_current_font();
    
    void pop_current_font();
    
private:
    bool docking_state_{};
    Canvas *current_canvas_{nullptr};
    std::unordered_map<std::string, ImFont *> fonts_;
    std::string layout_save_filename_ = "imgui.ini";
};

}
#endif /* DIGITALVOX_VOX_RENDER_UI_UI_MANAGER_H_ */
