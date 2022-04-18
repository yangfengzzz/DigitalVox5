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

class GraphicsApplication;

/**
 * @brief Vulkan helper class for Dear ImGui
 */
class Gui {
public:
    // The name of the default font file to use
    static const std::string default_font_;
    
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
        
    /**
     * @brief Draws the Gui
     * @param command_buffer Command buffer to register draw-commands
     */
    void draw(CommandBuffer &command_buffer);
    
    bool input_event(const InputEvent &input_event);
    
    Font &get_font(const std::string &font_name = Gui::default_font_);
        
private:
    /**
     * @brief Updates Vulkan buffers
     * @param render_frame Frame to render into
     */
    void update_buffers(CommandBuffer &command_buffer, RenderFrame &render_frame);
    
    static const double press_time_ms_;
    
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
        
    std::vector<Font> fonts_;
    
    std::unique_ptr<core::Image> font_image_;
    std::unique_ptr<core::ImageView> font_image_view_;
    
    std::unique_ptr<core::Sampler> sampler_{nullptr};
    
    PipelineLayout *pipeline_layout_{nullptr};
    
    VkDescriptorPool descriptor_pool_{VK_NULL_HANDLE};
    
    VkDescriptorSetLayout descriptor_set_layout_{VK_NULL_HANDLE};
    
    VkDescriptorSet descriptor_set_{VK_NULL_HANDLE};
    
    VkPipeline pipeline_{VK_NULL_HANDLE};
};

void Gui::new_frame() {
    ImGui::NewFrame();
}

}        // namespace vox
