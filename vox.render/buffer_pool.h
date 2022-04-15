//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "helpers.h"
#include "core/buffer.h"

namespace vox {
class Device;

/**
 * @brief An allocation of vulkan memory; different buffer allocations,
 *        with different offset and size, may come from the same Vulkan buffer
 */
class BufferAllocation {
public:
    BufferAllocation() = default;
    
    BufferAllocation(core::Buffer &buffer, VkDeviceSize size, VkDeviceSize offset);
    
    BufferAllocation(const BufferAllocation &) = delete;
    
    BufferAllocation(BufferAllocation &&) = default;
    
    BufferAllocation &operator=(const BufferAllocation &) = delete;
    
    BufferAllocation &operator=(BufferAllocation &&) = default;
    
    void update(const std::vector<uint8_t> &data, uint32_t offset = 0);
    
    template<class T>
    void update(const T &value, uint32_t offset = 0) {
        update(to_bytes(value), offset);
    }
    
    [[nodiscard]] bool empty() const;
    
    [[nodiscard]] VkDeviceSize get_size() const;
    
    [[nodiscard]] VkDeviceSize get_offset() const;
    
    core::Buffer &get_buffer();
    
private:
    core::Buffer *buffer{nullptr};
    
    VkDeviceSize base_offset{0};
    
    VkDeviceSize size{0};
};

/**
 * @brief Helper class which handles multiple allocation from the same underlying Vulkan buffer.
 */
class BufferBlock {
public:
    BufferBlock(Device &device, VkDeviceSize size, VkBufferUsageFlags usage, VmaMemoryUsage memory_usage);
    
    /**
     * @return An usable view on a portion of the underlying buffer
     */
    BufferAllocation allocate(uint32_t size);
    
    [[nodiscard]] VkDeviceSize get_size() const;
    
    void reset();
    
private:
    core::Buffer buffer;
    
    // Memory alignment, it may change according to the usage
    VkDeviceSize alignment{0};
    
    // Current offset, it increases on every allocation
    VkDeviceSize offset{0};
};

/**
 * @brief A pool of buffer blocks for a specific usage.
 * It may contain inactive blocks that can be recycled.
 *
 * BufferPool is a linear allocator for buffer chunks, it gives you a view of the size you want.
 * A BufferBlock is the corresponding VkBuffer and you can get smaller offsets inside it.
 * Since a shader cannot specify dynamic UBOs, it has to be done from the code
 * (set_resource_dynamic).
 *
 * When a new frame starts, buffer blocks are returned: the offset is reset and contents are
 * overwritten. The minimum allocation size is 256 kb, if you ask for more you get a dedicated
 * buffer allocation.
 *
 * We re-use descriptor sets: we only need one for the corresponding buffer infos (and we only
 * have one VkBuffer per BufferBlock), then it is bound and we use dynamic offsets.
 */
class BufferPool {
public:
    BufferPool(Device &device, VkDeviceSize block_size, VkBufferUsageFlags usage,
               VmaMemoryUsage memory_usage = VMA_MEMORY_USAGE_CPU_TO_GPU);
    
    BufferBlock &request_buffer_block(VkDeviceSize minimum_size);
    
    void reset();
    
private:
    Device &device;
    
    /// List of blocks requested
    std::vector<std::unique_ptr<BufferBlock>> buffer_blocks;
    
    /// Minimum size of the blocks
    VkDeviceSize block_size{0};
    
    VkBufferUsageFlags usage{};
    
    VmaMemoryUsage memory_usage{};
    
    /// Numbers of active blocks from the start of buffer_blocks
    uint32_t active_buffer_block_count{0};
};


}        // namespace vox
