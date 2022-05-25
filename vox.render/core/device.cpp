//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/core/device.h"

VKBP_DISABLE_WARNINGS()

#define VMA_IMPLEMENTATION

#include <vk_mem_alloc.h>

VKBP_ENABLE_WARNINGS()

namespace vox {
Device::Device(PhysicalDevice &gpu,
               VkSurfaceKHR surface,
               std::unique_ptr<DebugUtils> &&debug_utils,
               std::unordered_map<const char *, bool> requested_extensions)
    : VulkanResource{VK_NULL_HANDLE, this},  // Recursive, but valid
      debug_utils_{std::move(debug_utils)},
      gpu_{gpu},
      resource_cache_{*this} {
    LOGI("Selected GPU: {}", gpu.GetProperties().deviceName)

    // Prepare the device queues
    uint32_t queue_family_properties_count = utility::ToU32(gpu.GetQueueFamilyProperties().size());
    std::vector<VkDeviceQueueCreateInfo> queue_create_infos(queue_family_properties_count,
                                                            {VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO});
    std::vector<std::vector<float>> queue_priorities(queue_family_properties_count);

    for (uint32_t queue_family_index = 0U; queue_family_index < queue_family_properties_count; ++queue_family_index) {
        const VkQueueFamilyProperties &queue_family_property = gpu.GetQueueFamilyProperties()[queue_family_index];

        if (gpu.HasHighPriorityGraphicsQueue()) {
            uint32_t graphics_queue_family = GetQueueFamilyIndex(VK_QUEUE_GRAPHICS_BIT);
            if (graphics_queue_family == queue_family_index) {
                queue_priorities[queue_family_index].reserve(queue_family_property.queueCount);
                queue_priorities[queue_family_index].push_back(1.0f);
                for (uint32_t i = 1; i < queue_family_property.queueCount; i++) {
                    queue_priorities[queue_family_index].push_back(0.5f);
                }
            } else {
                queue_priorities[queue_family_index].resize(queue_family_property.queueCount, 0.5f);
            }
        } else {
            queue_priorities[queue_family_index].resize(queue_family_property.queueCount, 0.5f);
        }

        VkDeviceQueueCreateInfo &queue_create_info = queue_create_infos[queue_family_index];

        queue_create_info.queueFamilyIndex = queue_family_index;
        queue_create_info.queueCount = queue_family_property.queueCount;
        queue_create_info.pQueuePriorities = queue_priorities[queue_family_index].data();
    }

    // Check extensions to enable Vma Dedicated Allocation
    uint32_t device_extension_count;
    VK_CHECK(vkEnumerateDeviceExtensionProperties(gpu.GetHandle(), nullptr, &device_extension_count, nullptr));
    device_extensions_ = std::vector<VkExtensionProperties>(device_extension_count);
    VK_CHECK(vkEnumerateDeviceExtensionProperties(gpu.GetHandle(), nullptr, &device_extension_count,
                                                  device_extensions_.data()));

    // Display supported extensions
    if (!device_extensions_.empty()) {
        LOGD("Device supports the following extensions:")
        for (auto &extension : device_extensions_) {
            LOGD("  \t{}", extension.extensionName)
        }
    }

    bool can_get_memory_requirements = IsExtensionSupported("VK_KHR_get_memory_requirements2");
    bool has_dedicated_allocation = IsExtensionSupported("VK_KHR_dedicated_allocation");

    if (can_get_memory_requirements && has_dedicated_allocation) {
        enabled_extensions_.push_back("VK_KHR_get_memory_requirements2");
        enabled_extensions_.push_back("VK_KHR_dedicated_allocation");

        LOGI("Dedicated Allocation enabled")
    }

    // For performance queries, we also use host query reset since queryPool resets cannot
    // live in the same command buffer as beginQuery
    if (IsExtensionSupported("VK_KHR_performance_query") && IsExtensionSupported("VK_EXT_host_query_reset")) {
        auto perf_counter_features = gpu.RequestExtensionFeatures<VkPhysicalDevicePerformanceQueryFeaturesKHR>(
                VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PERFORMANCE_QUERY_FEATURES_KHR);
        auto host_query_reset_features = gpu.RequestExtensionFeatures<VkPhysicalDeviceHostQueryResetFeatures>(
                VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_HOST_QUERY_RESET_FEATURES);

        if (perf_counter_features.performanceCounterQueryPools && host_query_reset_features.hostQueryReset) {
            enabled_extensions_.push_back("VK_KHR_performance_query");
            enabled_extensions_.push_back("VK_EXT_host_query_reset");
            LOGI("Performance query enabled")
        }
    }

    // Check that extensions are supported before trying to create the device
    std::vector<const char *> unsupported_extensions{};
    for (auto &extension : requested_extensions) {
        if (IsExtensionSupported(extension.first)) {
            enabled_extensions_.emplace_back(extension.first);
        } else {
            unsupported_extensions.emplace_back(extension.first);
        }
    }

    if (!enabled_extensions_.empty()) {
        LOGI("Device supports the following requested extensions:")
        for (auto &extension : enabled_extensions_) {
            LOGI("  \t{}", extension)
        }
    }

    if (!unsupported_extensions.empty()) {
        auto error = false;
        for (auto &extension : unsupported_extensions) {
            auto extension_is_optional = requested_extensions[extension];
            if (extension_is_optional) {
                LOGW("Optional device extension {} not available, some features may be disabled", extension)
            } else {
                LOGE("Required device extension {} not available, cannot run", extension)
                error = true;
            }
        }

        if (error) {
            throw VulkanException(VK_ERROR_EXTENSION_NOT_PRESENT, "Extensions not present");
        }
    }

    VkDeviceCreateInfo create_info{VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO};

    // Latest requested feature will have the pNext's all set up for device creation.
    create_info.pNext = gpu.GetExtensionFeatureChain();

    create_info.pQueueCreateInfos = queue_create_infos.data();
    create_info.queueCreateInfoCount = utility::ToU32(queue_create_infos.size());
    create_info.enabledExtensionCount = utility::ToU32(enabled_extensions_.size());
    create_info.ppEnabledExtensionNames = enabled_extensions_.data();

    const auto kRequestedGpuFeatures = gpu.GetRequestedFeatures();
    create_info.pEnabledFeatures = &kRequestedGpuFeatures;

    VkResult result = vkCreateDevice(gpu.GetHandle(), &create_info, nullptr, &handle_);

    if (result != VK_SUCCESS) {
        throw VulkanException{result, "Cannot create device"};
    }

    queues_.resize(queue_family_properties_count);

    for (uint32_t queue_family_index = 0U; queue_family_index < queue_family_properties_count; ++queue_family_index) {
        const VkQueueFamilyProperties &queue_family_property = gpu.GetQueueFamilyProperties()[queue_family_index];

        VkBool32 present_supported = gpu.IsPresentSupported(surface, queue_family_index);

        for (uint32_t queue_index = 0U; queue_index < queue_family_property.queueCount; ++queue_index) {
            queues_[queue_family_index].emplace_back(*this, queue_family_index, queue_family_property,
                                                     present_supported, queue_index);
        }
    }

    VmaVulkanFunctions vma_vulkan_func{};
    vma_vulkan_func.vkAllocateMemory = vkAllocateMemory;
    vma_vulkan_func.vkBindBufferMemory = vkBindBufferMemory;
    vma_vulkan_func.vkBindImageMemory = vkBindImageMemory;
    vma_vulkan_func.vkCreateBuffer = vkCreateBuffer;
    vma_vulkan_func.vkCreateImage = vkCreateImage;
    vma_vulkan_func.vkDestroyBuffer = vkDestroyBuffer;
    vma_vulkan_func.vkDestroyImage = vkDestroyImage;
    vma_vulkan_func.vkFlushMappedMemoryRanges = vkFlushMappedMemoryRanges;
    vma_vulkan_func.vkFreeMemory = vkFreeMemory;
    vma_vulkan_func.vkGetBufferMemoryRequirements = vkGetBufferMemoryRequirements;
    vma_vulkan_func.vkGetImageMemoryRequirements = vkGetImageMemoryRequirements;
    vma_vulkan_func.vkGetPhysicalDeviceMemoryProperties = vkGetPhysicalDeviceMemoryProperties;
    vma_vulkan_func.vkGetPhysicalDeviceProperties = vkGetPhysicalDeviceProperties;
    vma_vulkan_func.vkInvalidateMappedMemoryRanges = vkInvalidateMappedMemoryRanges;
    vma_vulkan_func.vkMapMemory = vkMapMemory;
    vma_vulkan_func.vkUnmapMemory = vkUnmapMemory;
    vma_vulkan_func.vkCmdCopyBuffer = vkCmdCopyBuffer;

    VmaAllocatorCreateInfo allocator_info{};
    allocator_info.physicalDevice = gpu.GetHandle();
    allocator_info.device = handle_;
    allocator_info.instance = gpu.GetInstance().GetHandle();

    if (can_get_memory_requirements && has_dedicated_allocation) {
        allocator_info.flags |= VMA_ALLOCATOR_CREATE_KHR_DEDICATED_ALLOCATION_BIT;
        vma_vulkan_func.vkGetBufferMemoryRequirements2KHR = vkGetBufferMemoryRequirements2KHR;
        vma_vulkan_func.vkGetImageMemoryRequirements2KHR = vkGetImageMemoryRequirements2KHR;
    }

    if (IsExtensionSupported(VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME) &&
        IsEnabled(VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME)) {
        allocator_info.flags |= VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;
    }

    allocator_info.pVulkanFunctions = &vma_vulkan_func;

    result = vmaCreateAllocator(&allocator_info, &memory_allocator_);

    if (result != VK_SUCCESS) {
        throw VulkanException{result, "Cannot create allocator"};
    }

    command_pool_ = std::make_unique<CommandPool>(
            *this, GetQueueByFlags(VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT, 0).GetFamilyIndex());
    fence_pool_ = std::make_unique<FencePool>(*this);
}

Device::Device(PhysicalDevice &gpu, VkDevice &vulkan_device, VkSurfaceKHR surface) : gpu_{gpu}, resource_cache_{*this} {
    handle_ = vulkan_device;
    debug_utils_ = std::make_unique<DummyDebugUtils>();
}

Device::~Device() {
    resource_cache_.Clear();

    command_pool_.reset();
    fence_pool_.reset();

    if (memory_allocator_ != VK_NULL_HANDLE) {
        VmaStats stats;
        vmaCalculateStats(memory_allocator_, &stats);

        LOGI("Total device memory leaked: {} bytes.", stats.total.usedBytes)

        vmaDestroyAllocator(memory_allocator_);
    }

    if (handle_ != VK_NULL_HANDLE) {
        vkDestroyDevice(handle_, nullptr);
    }
}

bool Device::IsExtensionSupported(const std::string &extension) {
    return std::find_if(device_extensions_.begin(), device_extensions_.end(), [extension](auto &device_extension) {
               return std::strcmp(device_extension.extensionName, extension.c_str()) == 0;
           }) != device_extensions_.end();
}

bool Device::IsEnabled(const char *extension) {
    return std::find_if(enabled_extensions_.begin(), enabled_extensions_.end(),
                        [extension](const char *enabled_extension) {
                            return strcmp(extension, enabled_extension) == 0;
                        }) != enabled_extensions_.end();
}

const PhysicalDevice &Device::GetGpu() const { return gpu_; }

VmaAllocator Device::GetMemoryAllocator() const { return memory_allocator_; }

DriverVersion Device::GetDriverVersion() const {
    DriverVersion version{};

    switch (gpu_.GetProperties().vendorID) {
        case 0x10DE: {
            // Nvidia
            version.major = (gpu_.GetProperties().driverVersion >> 22) & 0x3ff;
            version.minor = (gpu_.GetProperties().driverVersion >> 14) & 0x0ff;
            version.patch = (gpu_.GetProperties().driverVersion >> 6) & 0x0ff;
            // Ignoring optional tertiary info in lower 6 bits
            break;
        }
        default: {
            version.major = VK_VERSION_MAJOR(gpu_.GetProperties().driverVersion);
            version.minor = VK_VERSION_MINOR(gpu_.GetProperties().driverVersion);
            version.patch = VK_VERSION_PATCH(gpu_.GetProperties().driverVersion);
        }
    }

    return version;
}

bool Device::IsImageFormatSupported(VkFormat format) const {
    VkImageFormatProperties format_properties;

    auto result = vkGetPhysicalDeviceImageFormatProperties(gpu_.GetHandle(), format, VK_IMAGE_TYPE_2D,
                                                           VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_SAMPLED_BIT,
                                                           0,  // no create flags
                                                           &format_properties);
    return result != VK_ERROR_FORMAT_NOT_SUPPORTED;
}

uint32_t Device::GetMemoryType(uint32_t bits, VkMemoryPropertyFlags properties, VkBool32 *memory_type_found) const {
    for (uint32_t i = 0; i < gpu_.GetMemoryProperties().memoryTypeCount; i++) {
        if ((bits & 1) == 1) {
            if ((gpu_.GetMemoryProperties().memoryTypes[i].propertyFlags & properties) == properties) {
                if (memory_type_found) {
                    *memory_type_found = true;
                }
                return i;
            }
        }
        bits >>= 1;
    }

    if (memory_type_found) {
        *memory_type_found = false;
        return 0;
    } else {
        throw std::runtime_error("Could not find a matching memory type");
    }
}

const Queue &Device::GetQueue(uint32_t queue_family_index, uint32_t queue_index) {
    return queues_[queue_family_index][queue_index];
}

const Queue &Device::GetQueueByFlags(VkQueueFlags queue_flags, uint32_t queue_index) const {
    for (const auto &queue : queues_) {
        Queue const &first_queue = queue[0];

        VkQueueFlags queue_flags = first_queue.GetProperties().queueFlags;
        uint32_t queue_count = first_queue.GetProperties().queueCount;

        if (((queue_flags & queue_flags) == queue_flags) && queue_index < queue_count) {
            return queue[queue_index];
        }
    }

    throw std::runtime_error("Queue not found");
}

const Queue &Device::GetQueueByPresent(uint32_t queue_index) const {
    for (const auto &queue : queues_) {
        Queue const &first_queue = queue[0];

        uint32_t queue_count = first_queue.GetProperties().queueCount;

        if (first_queue.SupportPresent() && queue_index < queue_count) {
            return queue[queue_index];
        }
    }

    throw std::runtime_error("Queue not found");
}

void Device::AddQueue(size_t global_index,
                      uint32_t family_index,
                      VkQueueFamilyProperties properties,
                      VkBool32 can_present) {
    if (queues_.size() < global_index + 1) {
        queues_.resize(global_index + 1);
    }
    queues_[global_index].emplace_back(*this, family_index, properties, can_present, 0);
}

uint32_t Device::GetNumQueuesForQueueFamily(uint32_t queue_family_index) {
    const auto &queue_family_properties = gpu_.GetQueueFamilyProperties();
    return queue_family_properties[queue_family_index].queueCount;
}

uint32_t Device::GetQueueFamilyIndex(VkQueueFlagBits queue_flag) {
    const auto &queue_family_properties = gpu_.GetQueueFamilyProperties();

    // Dedicated queue for compute
    // Try to find a queue family index that supports compute but not graphics
    if (queue_flag & VK_QUEUE_COMPUTE_BIT) {
        for (uint32_t i = 0; i < static_cast<uint32_t>(queue_family_properties.size()); i++) {
            if ((queue_family_properties[i].queueFlags & queue_flag) &&
                !(queue_family_properties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)) {
                return i;
            }
        }
    }

    // Dedicated queue for transfer
    // Try to find a queue family index that supports transfer but not graphics and compute
    if (queue_flag & VK_QUEUE_TRANSFER_BIT) {
        for (uint32_t i = 0; i < static_cast<uint32_t>(queue_family_properties.size()); i++) {
            if ((queue_family_properties[i].queueFlags & queue_flag) &&
                !(queue_family_properties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) &&
                !(queue_family_properties[i].queueFlags & VK_QUEUE_COMPUTE_BIT)) {
                return i;
            }
        }
    }

    // For other queue types or if no separate compute queue is present, return the first one to support the requested
    // flags
    for (uint32_t i = 0; i < static_cast<uint32_t>(queue_family_properties.size()); i++) {
        if (queue_family_properties[i].queueFlags & queue_flag) {
            return i;
        }
    }

    throw std::runtime_error("Could not find a matching queue family index");
}

const Queue &Device::GetSuitableGraphicsQueue() const {
    for (const auto &queue : queues_) {
        Queue const &first_queue = queue[0];

        uint32_t queue_count = first_queue.GetProperties().queueCount;

        if (first_queue.SupportPresent() && 0 < queue_count) {
            return queue[0];
        }
    }

    return GetQueueByFlags(VK_QUEUE_GRAPHICS_BIT, 0);
}

VkBuffer Device::CreateBuffer(VkBufferUsageFlags usage,
                              VkMemoryPropertyFlags properties,
                              VkDeviceSize size,
                              VkDeviceMemory *memory,
                              void *data) {
    VkBuffer buffer = VK_NULL_HANDLE;

    // Create the buffer handle_
    VkBufferCreateInfo buffer_create_info{};
    buffer_create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    buffer_create_info.usage = usage;
    buffer_create_info.size = size;
    buffer_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    VK_CHECK(vkCreateBuffer(handle_, &buffer_create_info, nullptr, &buffer));

    // Create the memory backing up the buffer handle_
    VkMemoryRequirements memory_requirements;
    VkMemoryAllocateInfo memory_allocation{};
    memory_allocation.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    vkGetBufferMemoryRequirements(handle_, buffer, &memory_requirements);
    memory_allocation.allocationSize = memory_requirements.size;
    // Find a memory type index that fits the properties of the buffer
    memory_allocation.memoryTypeIndex = GetMemoryType(memory_requirements.memoryTypeBits, properties);
    VK_CHECK(vkAllocateMemory(handle_, &memory_allocation, nullptr, memory));

    // If a pointer to the buffer data has been passed, map the buffer and copy over the
    if (data != nullptr) {
        void *mapped;
        VK_CHECK(vkMapMemory(handle_, *memory, 0, size, 0, &mapped));
        memcpy(mapped, data, static_cast<size_t>(size));
        // If host coherency hasn't been requested, do a manual flush to make writes visible
        if ((properties & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) == 0) {
            VkMappedMemoryRange mapped_range{};
            mapped_range.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
            mapped_range.memory = *memory;
            mapped_range.offset = 0;
            mapped_range.size = size;
            vkFlushMappedMemoryRanges(handle_, 1, &mapped_range);
        }
        vkUnmapMemory(handle_, *memory);
    }

    // Attach the memory to the buffer object
    VK_CHECK(vkBindBufferMemory(handle_, buffer, *memory, 0));

    return buffer;
}

void Device::CopyBuffer(vox::core::Buffer &src,
                        vox::core::Buffer &dst,
                        VkQueue queue,
                        VkBufferCopy *copy_region) const {
    assert(dst.GetSize() <= src.GetSize());
    assert(src.GetHandle());

    VkCommandBuffer command_buffer = CreateCommandBuffer(VK_COMMAND_BUFFER_LEVEL_PRIMARY, true);

    VkBufferCopy buffer_copy{};
    if (copy_region == nullptr) {
        buffer_copy.size = src.GetSize();
    } else {
        buffer_copy = *copy_region;
    }

    vkCmdCopyBuffer(command_buffer, src.GetHandle(), dst.GetHandle(), 1, &buffer_copy);

    FlushCommandBuffer(command_buffer, queue);
}

VkCommandPool Device::CreateCommandPool(uint32_t queue_index, VkCommandPoolCreateFlags flags) {
    VkCommandPoolCreateInfo command_pool_info = {};
    command_pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    command_pool_info.queueFamilyIndex = queue_index;
    command_pool_info.flags = flags;
    VkCommandPool command_pool;
    VK_CHECK(vkCreateCommandPool(handle_, &command_pool_info, nullptr, &command_pool));
    return command_pool;
}

VkCommandBuffer Device::CreateCommandBuffer(VkCommandBufferLevel level, bool begin) const {
    assert(command_pool_ && "No command pool exists in the device");

    VkCommandBufferAllocateInfo cmd_buf_allocate_info{};
    cmd_buf_allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    cmd_buf_allocate_info.commandPool = command_pool_->GetHandle();
    cmd_buf_allocate_info.level = level;
    cmd_buf_allocate_info.commandBufferCount = 1;

    VkCommandBuffer command_buffer;
    VK_CHECK(vkAllocateCommandBuffers(handle_, &cmd_buf_allocate_info, &command_buffer));

    // If requested, also start recording for the new command buffer
    if (begin) {
        VkCommandBufferBeginInfo command_buffer_info{};
        command_buffer_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        VK_CHECK(vkBeginCommandBuffer(command_buffer, &command_buffer_info));
    }

    return command_buffer;
}

void Device::FlushCommandBuffer(VkCommandBuffer command_buffer,
                                VkQueue queue,
                                bool free,
                                VkSemaphore signal_semaphore) const {
    if (command_buffer == VK_NULL_HANDLE) {
        return;
    }

    VK_CHECK(vkEndCommandBuffer(command_buffer));

    VkSubmitInfo submit_info{};
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &command_buffer;
    if (signal_semaphore) {
        submit_info.pSignalSemaphores = &signal_semaphore;
        submit_info.signalSemaphoreCount = 1;
    }

    // Create fence to ensure that the command buffer has finished executing
    VkFenceCreateInfo fence_info{};
    fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fence_info.flags = VK_FLAGS_NONE;

    VkFence fence;
    VK_CHECK(vkCreateFence(handle_, &fence_info, nullptr, &fence));

    // Submit to the queue
    VkResult result = vkQueueSubmit(queue, 1, &submit_info, fence);
    // Wait for the fence to signal that command buffer has finished executing
    VK_CHECK(vkWaitForFences(handle_, 1, &fence, VK_TRUE, DEFAULT_FENCE_TIMEOUT));

    vkDestroyFence(handle_, fence, nullptr);

    if (command_pool_ && free) {
        vkFreeCommandBuffers(handle_, command_pool_->GetHandle(), 1, &command_buffer);
    }
}

CommandPool &Device::GetCommandPool() const { return *command_pool_; }

FencePool &Device::GetFencePool() const { return *fence_pool_; }

void Device::CreateInternalFencePool() { fence_pool_ = std::make_unique<FencePool>(*this); }

void Device::CreateInternalCommandPool() {
    command_pool_ = std::make_unique<CommandPool>(
            *this, GetQueueByFlags(VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT, 0).GetFamilyIndex());
}

void Device::PrepareMemoryAllocator() {
    bool can_get_memory_requirements = IsExtensionSupported("VK_KHR_get_memory_requirements2");
    bool has_dedicated_allocation = IsExtensionSupported("VK_KHR_dedicated_allocation");

    VmaVulkanFunctions vma_vulkan_func{};
    vma_vulkan_func.vkAllocateMemory = vkAllocateMemory;
    vma_vulkan_func.vkBindBufferMemory = vkBindBufferMemory;
    vma_vulkan_func.vkBindImageMemory = vkBindImageMemory;
    vma_vulkan_func.vkCreateBuffer = vkCreateBuffer;
    vma_vulkan_func.vkCreateImage = vkCreateImage;
    vma_vulkan_func.vkDestroyBuffer = vkDestroyBuffer;
    vma_vulkan_func.vkDestroyImage = vkDestroyImage;
    vma_vulkan_func.vkFlushMappedMemoryRanges = vkFlushMappedMemoryRanges;
    vma_vulkan_func.vkFreeMemory = vkFreeMemory;
    vma_vulkan_func.vkGetBufferMemoryRequirements = vkGetBufferMemoryRequirements;
    vma_vulkan_func.vkGetImageMemoryRequirements = vkGetImageMemoryRequirements;
    vma_vulkan_func.vkGetPhysicalDeviceMemoryProperties = vkGetPhysicalDeviceMemoryProperties;
    vma_vulkan_func.vkGetPhysicalDeviceProperties = vkGetPhysicalDeviceProperties;
    vma_vulkan_func.vkInvalidateMappedMemoryRanges = vkInvalidateMappedMemoryRanges;
    vma_vulkan_func.vkMapMemory = vkMapMemory;
    vma_vulkan_func.vkUnmapMemory = vkUnmapMemory;
    vma_vulkan_func.vkCmdCopyBuffer = vkCmdCopyBuffer;

    VmaAllocatorCreateInfo allocator_info{};
    allocator_info.physicalDevice = gpu_.GetHandle();
    allocator_info.device = handle_;
    allocator_info.instance = gpu_.GetInstance().GetHandle();

    if (can_get_memory_requirements && has_dedicated_allocation) {
        allocator_info.flags |= VMA_ALLOCATOR_CREATE_KHR_DEDICATED_ALLOCATION_BIT;
        vma_vulkan_func.vkGetBufferMemoryRequirements2KHR = vkGetBufferMemoryRequirements2KHR;
        vma_vulkan_func.vkGetImageMemoryRequirements2KHR = vkGetImageMemoryRequirements2KHR;
    }

    if (IsExtensionSupported(VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME) &&
        IsEnabled(VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME)) {
        allocator_info.flags |= VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;
    }

    allocator_info.pVulkanFunctions = &vma_vulkan_func;

    VkResult result = vmaCreateAllocator(&allocator_info, &memory_allocator_);

    if (result != VK_SUCCESS) {
        throw VulkanException{result, "Cannot create allocator"};
    }
}

CommandBuffer &Device::RequestCommandBuffer() const { return command_pool_->RequestCommandBuffer(); }

VkFence Device::RequestFence() const { return fence_pool_->request_fence(); }

VkResult Device::WaitIdle() const { return vkDeviceWaitIdle(handle_); }

ResourceCache &Device::GetResourceCache() { return resource_cache_; }

}  // namespace vox
