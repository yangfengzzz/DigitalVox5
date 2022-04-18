//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#ifdef _WIN32
#	pragma comment(linker, "/subsystem:windows")
#	include <ShellScalingAPI.h>
#	include <fcntl.h>
#	include <io.h>
#	include <windows.h>
#endif

#include <chrono>
#include <iostream>
#include <random>
#include <sys/stat.h>

#include "old_camera.h"
#include "vector4.h"
#include "old_renderer.h"
#include "error.h"
#include "vk_common.h"
#include "vk_initializers.h"
#include "core/buffer.h"
#include "core/swapchain.h"
#include "gui.h"
#include "platform/platform.h"
#include "rendering/render_context.h"
#include "image.h"
#include "image.h"
#include "graphics_application.h"

namespace vox {
/**
 * @brief A swapchain buffer
 */
struct SwapchainBuffer {
    VkImage image;
    VkImageView view;
};

/**
 * @brief A texture wrapper that owns its image data and links it with a sampler
 */
struct Texture {
    std::shared_ptr<Image> image;
    VkSampler sampler;
};

/**
 * @brief The structure of a vertex
 */
struct Vertex {
    Vector3F pos;
    Vector3F normal;
    Vector2F uv;
    Vector4F joint0;
    Vector4F weight0;
};

/**
 * @brief Sascha Willems base class for use in his ported samples into the framework
 *
 * See GraphicsApplication for documentation
 */
class ForwardApplication : public GraphicsApplication {
public:
    ForwardApplication() = default;
    
    ~ForwardApplication() override;
    
    bool prepare(Platform &platform) override;
    
    void input_event(const InputEvent &input_event) override;
    
    void update(float delta_time) override;
    
    bool resize(uint32_t width, uint32_t height) override;
    
    virtual void render(float delta_time) = 0;
    
    Device &get_device();
    
    enum RenderPassCreateFlags {
        COLOR_ATTACHMENT_LOAD = 0x00000001
    };
    
protected:
    /// Stores the swapchain image buffers
    std::vector<SwapchainBuffer> swapchain_buffers_;
    
    void create_render_context(Platform &platform) override;
    
    void prepare_render_context() override;
    
    // Handle to the device graphics queue that command buffers are submitted to
    VkQueue queue_{};
    
    // Depth buffer format (selected during Vulkan initialization)
    VkFormat depth_format_;
    
    // Command buffer pool
    VkCommandPool cmd_pool_{};
    
    /** @brief Pipeline stages used to wait at for graphics queue submissions */
    VkPipelineStageFlags submit_pipeline_stages_ = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    
    // Contains command buffers and semaphores to be presented to the queue
    VkSubmitInfo submit_info_{};
    
    // Command buffers used for rendering
    std::vector<VkCommandBuffer> draw_cmd_buffers_;
    
    // Global render pass for frame buffer writes
    VkRenderPass render_pass_{};
    
    // List of available frame buffers (same as number of swap chain images)
    std::vector<VkFramebuffer> framebuffers_;
    
    // Active frame buffer index
    uint32_t current_buffer_ = 0;
    
    // Descriptor set pool
    VkDescriptorPool descriptor_pool_ = VK_NULL_HANDLE;
    
    // List of shader modules created (stored for cleanup)
    std::vector<VkShaderModule> shader_modules_;
    
    // Pipeline cache object
    VkPipelineCache pipeline_cache_{};
    
    // Synchronization semaphores
    struct {
        // Swap chain image presentation
        VkSemaphore acquired_image_ready_;
        
        // Command buffer submission and execution
        VkSemaphore render_complete_;
    } semaphores_{};
    
    // Synchronization fences
    std::vector<VkFence> wait_fences_;
    
    /**
     * @brief Populates the swapchain_buffers vector with the image and imageviews
     */
    void create_swapchain_buffers();
    
    /**
     * @brief Updates the swapchains image usage, if a swapchain exists and recreates all resources based on swapchain images
     * @param image_usage_flags The usage flags the new swapchain images will have
     */
    void update_swapchain_image_usage_flags(const std::set<VkImageUsageFlagBits> &image_usage_flags);
    
    /**
     * @brief Handles changes to the surface, e.g. on resize
     */
    void handle_surface_changes();
    
    /**
     * @brief Creates a buffer descriptor
     * @param buffer The buffer from which to create the descriptor from
     * @param size The size of the descriptor (default: VK_WHOLE_SIZE)
     * @param offset The offset of the descriptor (default: 0)
     */
    static VkDescriptorBufferInfo
    create_descriptor(core::Buffer &buffer, VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
    
    /**
     * @brief Creates an image descriptor
     * @param texture The texture from which to create the descriptor from
     * @param descriptor_type The type of image descriptor (default: VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER)
     */
    static VkDescriptorImageInfo
    create_descriptor(Texture &texture,
                      VkDescriptorType descriptor_type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
    
    /**
     * @brief Loads in a ktx 2D texture
     * @param file The filename of the texture to load
     */
    Texture load_texture(const std::string &file);
    
    /**
     * @brief Laods in a ktx 2D texture array
     * @param file The filename of the texture to load
     */
    Texture load_texture_array(const std::string &file);
    
    /**
     * @brief Loads in a ktx 2D texture cubemap
     * @param file The filename of the texture to load
     */
    Texture load_texture_cubemap(const std::string &file);
    
    /**
     * @brief Loads in a single model from a GLTF file
     * @param file The filename of the model to load
     * @param index The index of the model to load from the GLTF file (default: 0)
     */
    std::unique_ptr<sg::Renderer> load_model(const std::string &file, uint32_t index = 0);
    
    /**
     * @brief Records the necessary drawing commands to a command buffer
     * @param model The model to draw
     * @param command_buffer The command buffer to record to
     */
    static void draw_model(std::unique_ptr<sg::Renderer> &model, VkCommandBuffer command_buffer);
    
    /**
     * @brief Synchronously execute a block code within a command buffer, then submit the command buffer and wait for completion.
     * @param f a block of code which is passed a command buffer which is already in the begin state.
     * @param signal_semaphore An optional semaphore to signal when the commands have completed execution.
     */
    void with_command_buffer(const std::function<void(VkCommandBuffer command_buffer)> &f,
                             VkSemaphore signal_semaphore = VK_NULL_HANDLE);
    
public:
    /**
     * @brief Called when a view change occurs, can be overriden in derived samples to handle updating uniforms
     */
    virtual void view_changed();
    
    /**
     * @brief Called after the mouse cursor is moved and before internal events (like camera rotation) is handled
     * @param x The width from the origin
     * @param y The height from the origin
     * @param handled Whether the event was handled
     */
    virtual void mouse_moved(double x, double y, bool &handled);
    
    /**
     * @brief To be overridden by the derived class. Records the relevant commands to the rendering command buffers
     *        Called when the framebuffers need to be rebuilt
     */
    virtual void build_command_buffers() = 0;
    
    /**
     * @brief Creates the fences for rendering
     */
    void create_synchronization_primitives();
    
    /**
     * @brief Creates a new (graphics) command pool object storing command buffers
     */
    void create_command_pool();
    
    /**
     * @brief Setup default depth and stencil views
     */
    virtual void setup_depth_stencil();
    
    /**
     * @brief Create framebuffers for all requested swap chain images
     *        Can be overriden in derived class to setup a custom framebuffer (e.g. for MSAA)
     */
    virtual void setup_framebuffer();
    
    /**
     * @brief Setup a default render pass
     *        Can be overriden in derived class to setup a custom render pass (e.g. for MSAA)
     */
    virtual void setup_render_pass();
    
    /**
     * @brief Update flags for the default render pass and recreate it
     * @param flags Optional flags for render pass creation
     */
    void update_render_pass_flags(uint32_t flags = 0);
    
    /**
     * @brief Check if command buffers are valid (!= VK_NULL_HANDLE)
     */
    bool check_command_buffers();
    
    /**
     * @brief Create command buffers for drawing commands
     */
    void create_command_buffers();
    
    /**
     * @brief Destroy all command buffers, may be necessary during runtime if options are toggled
     */
    void destroy_command_buffers();
    
    /**
     * @brief Create a cache pool for rendering pipelines
     */
    void create_pipeline_cache();
    
    /**
     * @brief Load a SPIR-V shader
     * @param file The file location of the shader relative to the shaders folder
     * @param stage The shader stage
     */
    VkPipelineShaderStageCreateInfo load_shader(const std::string &file, VkShaderStageFlagBits stage);
    
    /**
     * @brief Updates the overlay
     * @param delta_time The time taken since the last frame
     */
    void update_overlay(float delta_time);
    
    /**
     * @brief If the gui is enabled, then record the drawing commands to a command buffer
     * @param command_buffer A valid command buffer that is ready to be recorded to
     */
    void draw_ui(VkCommandBuffer command_buffer);
    
    /**
     * @brief Prepare the frame for workload submission, acquires the next image from the swap chain and
     *        sets the default wait and signal semaphores
     */
    void prepare_frame();
    
    /**
     * @brief Submit the frames' workload
     */
    void submit_frame();
    
    /**
     * @brief Called when the UI overlay is updating, can be used to add custom elements to the overlay
     * @param drawer The drawer from the gui to draw certain elements
     */
    virtual void on_update_ui_overlay(Drawer &drawer);
    
private:
    /** brief Indicates that the view (position, rotation) has changed and buffers containing camera matrices need to be updated */
    bool view_updated_ = false;
    // Destination dimensions for resizing the window
    uint32_t dest_width_{};
    uint32_t dest_height_{};
    bool resizing_ = false;
    
    void handle_mouse_move(int32_t x, int32_t y);
    
#if defined(VKB_DEBUG) || defined(VKB_VALIDATION_LAYERS)
    /// The debug report callback
    VkDebugReportCallbackEXT debug_report_callback_{VK_NULL_HANDLE};
#endif
    
public:
    bool prepared_ = false;
    uint32_t width_ = 1280;
    uint32_t height_ = 720;
    
    /** @brief Example settings that can be changed e.g. by command line arguments */
    struct Settings {
        /** @brief Set to true if fullscreen mode has been requested via command line */
        bool fullscreen = false;
        /** @brief Set to true if v-sync will be forced for the swapchain */
        bool vsync = false;
    } settings_;
    
    VkClearColorValue default_clear_color_ = {{0.025f, 0.025f, 0.025f, 1.0f}};
    
    float zoom_ = 0;
    
    // Defines a frame rate independent timer value clamped from -1.0...1.0
    // For use in animations, rotations, etc.
    float timer_ = 0.0f;
    // Multiplier for speeding up (or slowing down) the global timer
    float timer_speed_ = 0.0025f;
    
    bool paused_ = false;
    
    // Use to adjust mouse rotation speed
    float rotation_speed_ = 1.0f;
    // Use to adjust mouse zoom speed
    float zoom_speed_ = 1.0f;
    
    Camera camera_;
    
    Vector3F rotation_ = Vector3F();
    Vector3F camera_pos_ = Vector3F();
    Vector2F mouse_pos_;
    
    std::string title_ = "Vulkan Example";
    std::string name_ = "vulkanExample";
    
    struct {
        VkImage image_;
        VkDeviceMemory mem_;
        VkImageView view_;
    } depth_stencil_{};
    
    struct {
        Vector2F axis_left_ = Vector2F();
        Vector2F axis_right_ = Vector2F();
    } game_pad_state_;
    
    struct {
        bool left_ = false;
        bool right_ = false;
        bool middle_ = false;
    } mouse_buttons_;
    
    struct TouchPos {
        int32_t x;
        int32_t y;
    } touch_pos_{};
    bool touch_down_ = false;
    double touch_timer_ = 0.0;
    int64_t last_tap_time_ = 0;
};

}