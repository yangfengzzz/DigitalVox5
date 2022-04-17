//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <cstdint>
#include <functional>
#include <future>
#include <imgui.h>
#include <imgui_internal.h>
#include <thread>

#include "core/buffer.h"
#include "core/command_buffer.h"
#include "core/sampler.h"
#include "debug_info.h"
#include "platform/filesystem.h"
#include "platform/input_events.h"
#include "rendering/render_context.h"
#include "stats/stats.h"

namespace vox {
class Window;

/**
 * @brief Helper structure for fonts loaded from TTF
 */
struct Font {
    /**
     * @brief Constructor
     * @param name The name of the font file that exists within 'assets/fonts' (without extension)
     * @param size The font size, scaled by DPI
     */
    Font(const std::string &name, float size) :
    name{name},
    data{vox::fs::read_asset("fonts/" + name + ".ttf")},
    size{size} {
        // Keep ownership of the font data to avoid a double delete
        ImFontConfig font_config{};
        font_config.FontDataOwnedByAtlas = false;
        
        if (size < 1.0f) {
            size = 20.0f;
        }
        
        ImGuiIO &io = ImGui::GetIO();
        handle = io.Fonts->AddFontFromMemoryTTF(data.data(), static_cast<int>(data.size()), size, &font_config);
    }
    
    ImFont *handle{nullptr};
    
    std::string name;
    
    std::vector<uint8_t> data;
    
    float size{};
};

/**
 * @brief Responsible for drawing new elements into the gui
 */
class Drawer {
public:
    Drawer() = default;
    
    /**
     * @brief Clears the dirty bit set
     */
    void clear();
    
    /**
     * @brief Returns true if the drawer has been updated
     */
    [[nodiscard]] bool is_dirty() const;
    
    /**
     * @brief May be used to force drawer update
     */
    void set_dirty(bool dirty);
    
    /**
     * @brief Adds a collapsable header item to the gui
     * @param caption The text to display
     * @returns True if adding item was successful
     */
    static bool header(const char *caption);
    
    /**
     * @brief Adds a checkbox to the gui
     * @param caption The text to display
     * @param value The boolean value to map the checkbox to
     * @returns True if adding item was successful
     */
    bool checkbox(const char *caption, bool *value);
    
    /**
     * @brief Adds a checkbox to the gui
     * @param caption The text to display
     * @param value The integer value to map the checkbox to
     * @returns True if adding item was successful
     */
    bool checkbox(const char *caption, int32_t *value);
    
    /**
     * @brief Adds a number input field to the gui
     * @param caption The text to display
     * @param value The value to map to
     * @param step The step increment
     * @returns True if adding item was successful
     */
    bool input_float(const char *caption, float *value, float step);
    
    /**
     * @brief Adds a slide bar to the gui for floating points to the gui
     * @param caption The text to display
     * @param value The value to map to
     * @param min The minimum value
     * @param max The maximum value
     * @returns True if adding item was successful
     */
    bool slider_float(const char *caption, float *value, float min, float max);
    
    /**
     * @brief Adds a slide bar to the gui for integers to the gui
     * @param caption The text to display
     * @param value The value to map to
     * @param min The minimum value
     * @param max The maximum value
     * @returns True if adding item was successful
     */
    bool slider_int(const char *caption, int32_t *value, int32_t min, int32_t max);
    
    /**
     * @brief Adds a multiple choice drop box to the gui
     * @param caption The text to display
     * @param itemindex The item index to display
     * @param items The items to display in the box
     * @returns True if adding item was successful
     */
    bool combo_box(const char *caption, int32_t *itemindex, const std::vector<std::string> &items);
    
    /**
     * @brief Adds a clickable button to the gui
     * @param caption The text to display
     * @returns True if adding item was successful
     */
    bool button(const char *caption);
    
    /**
     * @brief Adds a label to the gui
     * @param formatstr The format string
     */
    static void text(const char *formatstr, ...);
    
private:
    bool dirty_{false};
};

class GraphicsApplication;

/**
 * @brief Vulkan helper class for Dear ImGui
 */
class Gui {
public:
    /**
     * @brief Helper class for drawing statistics
     */
    class StatsView {
    public:
        /**
         * @brief Constructs a StatsView
         * @param stats Const pointer to the Stats data object; may be null
         */
        explicit StatsView(const Stats *stats);
        
        /**
         * @brief Resets the max values for the stats
         *        which do not have a fixed max
         */
        void reset_max_values();
        
        /**
         * @brief Resets the max value for a specific stat
         */
        void reset_max_value(StatIndex index);
        
        std::map<StatIndex, StatGraphData> graph_map_;
        
        float graph_height_{50.0f};
        
        float top_padding_{1.1f};
    };
    
    /**
     * @brief Helper class for rendering debug statistics in the GUI
     */
    class DebugView {
    public:
        bool active_{false};
        
        float scale_{1.7f};
        
        uint32_t max_fields_{8};
        
        float label_column_width_{0};
    };
    
    // The name of the default font file to use
    static const std::string default_font_;
    
    struct PushConstBlock {
        Vector2F scale;
        Vector2F translate;
    } push_const_block_;
    
    /**
     * @brief Initializes the Gui
     * @param sample A vulkan render context
     * @param window A Window object from which to draw DPI and content scaling information
     * @param stats A statistics object (null if no statistics are used)
     * @param font_size The font size
     * @param explicit_update If true, update buffers every frame
     */
    Gui(GraphicsApplication &sample, const Window &window, const Stats *stats = nullptr,
		float font_size = 21.0f, bool explicit_update = false);
    
    /**
     * @brief Destroys the Gui
     */
    ~Gui();
    
    void prepare(VkPipelineCache pipeline_cache, VkRenderPass render_pass,
                 const std::vector<VkPipelineShaderStageCreateInfo> &shader_stages);
    
    /**
     * @brief Handles resizing of the window
     * @param width New width of the window
     * @param height New height of the window
     */
    static void resize(uint32_t width, uint32_t height);
    
    /**
     * @brief Starts a new ImGui frame
     *        to be called before drawing any window
     */
    static inline void new_frame();
    
    /**
     * @brief Updates the Gui
     * @param delta_time Time passed since last update
     */
    void update(float delta_time);
    
    bool update_buffers();
    
    /**
     * @brief Draws the Gui
     * @param command_buffer Command buffer to register draw-commands
     */
    void draw(CommandBuffer &command_buffer);
    
    /**
     * @brief Draws the Gui
     * @param command_buffer Command buffer to register draw-commands
     */
    void draw(VkCommandBuffer command_buffer);
    
    /**
     * @brief Shows an overlay top window with app info and maybe stats
     * @param app_name Application name
     * @param stats Statistics to show (can be null)
     * @param debug_info Debug info to show (can be null)
     */
    void
    show_top_window(const std::string &app_name, const Stats *stats = nullptr, DebugInfo *debug_info = nullptr);
    
    /**
     * @brief Shows the ImGui Demo window
     */
    static void show_demo_window();
    
    /**
     * @brief Shows an child with app info
     * @param app_name Application name
     */
    void show_app_info(const std::string &app_name);
    
    /**
     * @brief Shows a moveable window with debug information
     * @param debug_info The object holding the data fields to be displayed
     * @param position The absolute position to set
     */
    void show_debug_window(DebugInfo &debug_info, const ImVec2 &position);
    
    /**
     * @brief Shows a child with statistics
     * @param stats Statistics to show
     */
    void show_stats(const Stats &stats);
    
    /**
     * @brief Shows an options windows, to be filled by the sample,
     *        which will be positioned at the top
     * @param body ImGui commands defining the body of the window
     * @param lines The number of lines of text to draw in the window
     *        These will help the gui to calculate the height of the window
     */
    static void show_options_window(const std::function<void()> &body, uint32_t lines = 3);
    
    void show_simple_window(const std::string &name, uint32_t last_fps, const std::function<void()> &body);
    
    bool input_event(const InputEvent &input_event);
    
    /**
     * @return The stats view
     */
    StatsView &get_stats_view();
    
    Drawer &get_drawer();
    
    Font &get_font(const std::string &font_name = Gui::default_font_);
    
    [[nodiscard]] bool is_debug_view_active() const;
    
private:
    /**
     * @brief Block size of a buffer pool in kilobytes
     */
    static constexpr uint32_t buffer_pool_block_size_ = 256;
    
    /**
     * @brief Updates Vulkan buffers
     * @param render_frame Frame to render into
     */
    void update_buffers(CommandBuffer &command_buffer, RenderFrame &render_frame);
    
    static const double press_time_ms_;
    
    static const float overlay_alpha_;
    
    static const ImGuiWindowFlags common_flags_;
    
    static const ImGuiWindowFlags options_flags_;
    
    static const ImGuiWindowFlags info_flags_;
    
    GraphicsApplication &sample_;
    
    std::unique_ptr<core::Buffer> vertex_buffer_;
    
    std::unique_ptr<core::Buffer> index_buffer_;
    
    size_t last_vertex_buffer_size_{};
    
    size_t last_index_buffer_size_{};
    
    ///  Scale factor to apply due to a difference between the window and GL pixel sizes
    float content_scale_factor_{1.0f};
    
    /// Scale factor to apply to the size of gui elements (expressed in dp)
    float dpi_factor_{1.0f};
    
    bool explicit_update_{false};
    
    Drawer drawer_;
    
    std::vector<Font> fonts_;
    
    std::unique_ptr<core::Image> font_image_;
    std::unique_ptr<core::ImageView> font_image_view_;
    
    std::unique_ptr<core::Sampler> sampler_{nullptr};
    
    PipelineLayout *pipeline_layout_{nullptr};
    
    StatsView stats_view_;
    
    DebugView debug_view_;
    
    VkDescriptorPool descriptor_pool_{VK_NULL_HANDLE};
    
    VkDescriptorSetLayout descriptor_set_layout_{VK_NULL_HANDLE};
    
    VkDescriptorSet descriptor_set_{VK_NULL_HANDLE};
    
    VkPipeline pipeline_{VK_NULL_HANDLE};
    
    /// Used to measure duration of input events
    Timer timer_;
    
    /// Used to show/hide the GUI
    bool visible_{true};
    
    bool prev_visible_{true};
    
    /// Whether or not the GUI has detected a multi touch gesture
    bool two_finger_tap_ = false;
    
    bool show_graph_file_output_ = false;
};

void Gui::new_frame() {
    ImGui::NewFrame();
}

}        // namespace vox
