//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/buffer_pool.h"

#include "vox.base/logging.h"
#include "vox.render/core/device.h"
#include "vox.render/error.h"

namespace vox {
BufferBlock::BufferBlock(Device &device, VkDeviceSize size, VkBufferUsageFlags usage, VmaMemoryUsage memory_usage)
    : buffer_{device, size, usage, memory_usage} {
    if (usage == VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT) {
        alignment_ = device.GetGpu().GetProperties().limits.minUniformBufferOffsetAlignment;
    } else if (usage == VK_BUFFER_USAGE_STORAGE_BUFFER_BIT) {
        alignment_ = device.GetGpu().GetProperties().limits.minStorageBufferOffsetAlignment;
    } else if (usage == VK_BUFFER_USAGE_UNIFORM_TEXEL_BUFFER_BIT) {
        alignment_ = device.GetGpu().GetProperties().limits.minTexelBufferOffsetAlignment;
    } else if (usage == VK_BUFFER_USAGE_INDEX_BUFFER_BIT || usage == VK_BUFFER_USAGE_VERTEX_BUFFER_BIT ||
               usage == VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT) {
        // Used to calculate the offset, required when allocating memory (its value should be power of 2)
        alignment_ = 16;
    } else {
        throw std::runtime_error("Usage not recognised");
    }
}

BufferAllocation BufferBlock::Allocate(const uint32_t allocate_size) {
    assert(allocate_size > 0 && "Allocation size must be greater than zero");

    auto aligned_offset = (offset_ + alignment_ - 1) & ~(alignment_ - 1);

    if (aligned_offset + allocate_size > buffer_.GetSize()) {
        // No more space available from the underlying buffer, return empty allocation
        return BufferAllocation{};
    }

    // Move the current offset and return an allocation
    offset_ = aligned_offset + allocate_size;
    return BufferAllocation{buffer_, allocate_size, aligned_offset};
}

VkDeviceSize BufferBlock::GetSize() const { return buffer_.GetSize(); }

void BufferBlock::Reset() { offset_ = 0; }

BufferPool::BufferPool(Device &device, VkDeviceSize block_size, VkBufferUsageFlags usage, VmaMemoryUsage memory_usage)
    : device_{device}, block_size_{block_size}, usage_{usage}, memory_usage_{memory_usage} {}

BufferBlock &BufferPool::RequestBufferBlock(VkDeviceSize minimum_size) {
    // Find the first block in the range of the inactive blocks
    // which can fit the minimum size
    auto it = std::upper_bound(
            buffer_blocks_.begin() + active_buffer_block_count_, buffer_blocks_.end(), minimum_size,
            [](const VkDeviceSize &a, const std::unique_ptr<BufferBlock> &b) -> bool { return a <= b->GetSize(); });

    if (it != buffer_blocks_.end()) {
        // Recycle inactive block
        active_buffer_block_count_++;
        return *it->get();
    }

    LOGD("Building #{} buffer block ({})", buffer_blocks_.size(), usage_)

    // Create a new block, store and return it
    buffer_blocks_.emplace_back(
            std::make_unique<BufferBlock>(device_, std::max(block_size_, minimum_size), usage_, memory_usage_));

    auto &block = buffer_blocks_[active_buffer_block_count_++];

    return *block;
}

void BufferPool::Reset() {
    for (auto &buffer_block : buffer_blocks_) {
        buffer_block->Reset();
    }

    active_buffer_block_count_ = 0;
}

BufferAllocation::BufferAllocation(core::Buffer &buffer, VkDeviceSize size, VkDeviceSize offset)
    : buffer_{&buffer}, size_{size}, base_offset_{offset} {}

void BufferAllocation::Update(const std::vector<uint8_t> &data, uint32_t offset) {
    assert(buffer_ && "Invalid buffer pointer");

    if (offset + data.size() <= size_) {
        buffer_->Update(data, utility::ToU32(base_offset_) + offset);
    } else {
        LOGE("Ignore buffer allocation update")
    }
}

bool BufferAllocation::Empty() const { return size_ == 0 || buffer_ == nullptr; }

VkDeviceSize BufferAllocation::GetSize() const { return size_; }

VkDeviceSize BufferAllocation::GetOffset() const { return base_offset_; }

core::Buffer &BufferAllocation::GetBuffer() {
    assert(buffer_ && "Invalid buffer pointer");
    return *buffer_;
}

}  // namespace vox
