//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.
#pragma once

#include "vox.base/helper.h"
#include "vox.base/logging.h"
#include "vox.render/core/command_buffer.h"
#include "vox.render/core/command_pool.h"
#include "vox.render/core/debug.h"
#include "vox.render/core/descriptor_set.h"
#include "vox.render/core/descriptor_set_layout.h"
#include "vox.render/core/framebuffer.h"
#include "vox.render/core/instance.h"
#include "vox.render/core/physical_device.h"
#include "vox.render/core/pipeline.h"
#include "vox.render/core/pipeline_layout.h"
#include "vox.render/core/queue.h"
#include "vox.render/core/render_pass.h"
#include "vox.render/core/swapchain.h"
#include "vox.render/core/vulkan_resource.h"
#include "vox.render/fence_pool.h"
#include "vox.render/rendering/pipeline_state.h"
#include "vox.render/rendering/render_target.h"
#include "vox.render/resource_cache.h"
#include "vox.render/shader/shader_module.h"
#include "vox.render/vk_common.h"

namespace vox {
struct DriverVersion {
    uint16_t major;
    uint16_t minor;
    uint16_t patch;
};

class Device : public core::VulkanResource<VkDevice, VK_OBJECT_TYPE_DEVICE> {
public:
    /**
     * @brief Device constructor
     * @param gpu A valid Vulkan physical device and the requested gpu features
     * @param surface The surface
     * @param debug_utils The debug utils to be associated to this device
     * @param requested_extensions (Optional) List of required device extensions and whether support is optional or not
     */
    Device(PhysicalDevice &gpu,
           VkSurfaceKHR surface,
           std::unique_ptr<DebugUtils> &&debug_utils,
           std::unordered_map<const char *, bool> requested_extensions = {});

    /**
     * @brief Device constructor
     * @param gpu A valid Vulkan physical device and the requested gpu features
     * @param vulkan_device A valid Vulkan device
     * @param surface The surface
     */
    Device(PhysicalDevice &gpu, VkDevice &vulkan_device, VkSurfaceKHR surface);

    Device(const Device &) = delete;

    Device(Device &&) = delete;

    ~Device() override;

    Device &operator=(const Device &) = delete;

    Device &operator=(Device &&) = delete;

    const PhysicalDevice &GetGpu() const;

    VmaAllocator GetMemoryAllocator() const;

    /**
     * @brief Returns the debug utils associated with this Device.
     */
    inline const DebugUtils &GetDebugUtils() const { return *debug_utils_; }

    /**
     * @return The version of the driver of the current physical device
     */
    DriverVersion GetDriverVersion() const;

    /**
     * @return Whether an image format is supported by the GPU
     */
    bool IsImageFormatSupported(VkFormat format) const;

    const Queue &GetQueue(uint32_t queue_family_index, uint32_t queue_index);

    const Queue &GetQueueByFlags(VkQueueFlags queue_flags, uint32_t queue_index) const;

    const Queue &GetQueueByPresent(uint32_t queue_index) const;

    /**
     * @brief Manually adds a new queue from a given family index to this device
     * @param global_index Index at where the queue should be placed inside the already existing list of queues
     * @param family_index Index of the queue family from which the queue will be created
     * @param properties Vulkan queue family properties
     * @param can_present True if the queue is able to present images
     */
    void AddQueue(size_t global_index, uint32_t family_index, VkQueueFamilyProperties properties, VkBool32 can_present);

    /**
     * @brief Finds a suitable graphics queue to submit to
     * @return The first present supported queue, otherwise just any graphics queue
     */
    const Queue &GetSuitableGraphicsQueue() const;

    bool IsExtensionSupported(const std::string &extension);

    bool IsEnabled(const char *extension);

    uint32_t GetQueueFamilyIndex(VkQueueFlagBits queue_flag);

    uint32_t GetNumQueuesForQueueFamily(uint32_t queue_family_index);

    CommandPool &GetCommandPool() const;

    /**
     * @brief Checks that a given memory type is supported by the GPU
     * @param bits The memory requirement type bits
     * @param properties The memory property to search for
     * @param memory_type_found True if found, false if not found
     * @returns The memory type index of the found memory type
     */
    uint32_t GetMemoryType(uint32_t bits,
                           VkMemoryPropertyFlags properties,
                           VkBool32 *memory_type_found = nullptr) const;

    /**
     * @brief Creates a vulkan buffer
     * @param usage The buffer usage
     * @param properties The memory properties
     * @param size The size of the buffer
     * @param memory The pointer to the buffer memory
     * @param data The data to place inside the buffer
     * @returns A valid VkBuffer
     */
    VkBuffer CreateBuffer(VkBufferUsageFlags usage,
                          VkMemoryPropertyFlags properties,
                          VkDeviceSize size,
                          VkDeviceMemory *memory,
                          void *data = nullptr);

    /**
     * @brief Copies a buffer from one to another
     * @param src The buffer to copy from
     * @param dst The buffer to copy to
     * @param queue The queue to submit the copy command to
     * @param copy_region The amount to copy, if null copies the entire buffer
     */
    void CopyBuffer(vox::core::Buffer &src,
                    vox::core::Buffer &dst,
                    VkQueue queue,
                    VkBufferCopy *copy_region = nullptr) const;

    /**
     * @brief Creates a command pool
     * @param queue_index The queue index this command pool is associated with
     * @param flags The command pool flags
     * @returns A valid VkCommandPool
     */
    VkCommandPool CreateCommandPool(uint32_t queue_index,
                                    VkCommandPoolCreateFlags flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);

    /**
     * @brief Requests a command buffer from the device's command pool
     * @param level The command buffer level
     * @param begin Whether the command buffer should be implicitly started before it's returned
     * @returns A valid VkCommandBuffer
     */
    VkCommandBuffer CreateCommandBuffer(VkCommandBufferLevel level, bool begin = false) const;

    /**
     * @brief Submits and frees up a given command buffer
     * @param command_buffer The command buffer
     * @param queue The queue to submit the work to
     * @param free Whether the command buffer should be implicitly freed up
     * @param signal_semaphore An optional semaphore to signal when the commands have been executed
     */
    void FlushCommandBuffer(VkCommandBuffer command_buffer,
                            VkQueue queue,
                            bool free = true,
                            VkSemaphore signal_semaphore = VK_NULL_HANDLE) const;

    /**
     * @brief Requests a command buffer from the general command_pool
     * @return A new command buffer
     */
    CommandBuffer &RequestCommandBuffer() const;

    FencePool &GetFencePool() const;

    /**
     * @brief Creates the fence pool used by this device
     */
    void CreateInternalFencePool();

    /**
     * @brief Creates the command pool used by this device
     */
    void CreateInternalCommandPool();

    /**
     * @brief Creates and sets up the Vulkan memory allocator
     */
    void PrepareMemoryAllocator();

    /**
     * @brief Requests a fence to the fence pool
     * @return A vulkan fence
     */
    VkFence RequestFence() const;

    VkResult WaitIdle() const;

    ResourceCache &GetResourceCache();

private:
    const PhysicalDevice &gpu_;

    VkSurfaceKHR surface_{VK_NULL_HANDLE};

    std::unique_ptr<DebugUtils> debug_utils_;

    std::vector<VkExtensionProperties> device_extensions_;

    std::vector<const char *> enabled_extensions_{};

    VmaAllocator memory_allocator_{VK_NULL_HANDLE};

    std::vector<std::vector<Queue>> queues_;

    /// A command pool associated to the primary queue
    std::unique_ptr<CommandPool> command_pool_;

    /// A fence pool associated to the primary queue
    std::unique_ptr<FencePool> fence_pool_;

    ResourceCache resource_cache_;
};

}  // namespace vox
