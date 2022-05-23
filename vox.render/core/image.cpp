//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/core/image.h"

#include "vox.render/core/device.h"
#include "vox.render/core/image_view.h"

namespace vox {
namespace {
inline VkImageType FindImageType(VkExtent3D extent) {
    VkImageType result;

    uint32_t dim_num{0};

    if (extent.width >= 1) {
        dim_num++;
    }

    if (extent.height >= 1) {
        dim_num++;
    }

    if (extent.depth > 1) {
        dim_num++;
    }

    switch (dim_num) {
        case 1:
            result = VK_IMAGE_TYPE_1D;
            break;
        case 2:
            result = VK_IMAGE_TYPE_2D;
            break;
        case 3:
            result = VK_IMAGE_TYPE_3D;
            break;
        default:
            throw std::runtime_error("No image type found.");
    }

    return result;
}
}  // namespace

namespace core {
Image::Image(Device const &device,
             const VkExtent3D &extent,
             VkFormat format,
             VkImageUsageFlags image_usage,
             VmaMemoryUsage memory_usage,
             VkSampleCountFlagBits sample_count,
             const uint32_t mip_levels,
             const uint32_t array_layers,
             VkImageTiling tiling,
             VkImageCreateFlags flags,
             uint32_t num_queue_families,
             const uint32_t *queue_families)
    : VulkanResource{VK_NULL_HANDLE, &device},
      type_{FindImageType(extent)},
      extent_{extent},
      format_{format},
      sample_count_{sample_count},
      usage_{image_usage},
      array_layer_count_{array_layers},
      tiling_{tiling} {
    assert(mip_levels > 0 && "Image should have at least one level");
    assert(array_layers > 0 && "Image should have at least one layer");

    subresource_.mipLevel = mip_levels;
    subresource_.arrayLayer = array_layers;

    VkImageCreateInfo image_info{VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO};
    image_info.flags = flags;
    image_info.imageType = type_;
    image_info.format = format;
    image_info.extent = extent;
    image_info.mipLevels = mip_levels;
    image_info.arrayLayers = array_layers;
    image_info.samples = sample_count;
    image_info.tiling = tiling;
    image_info.usage = image_usage;

    if (num_queue_families != 0) {
        image_info.sharingMode = VK_SHARING_MODE_CONCURRENT;
        image_info.queueFamilyIndexCount = num_queue_families;
        image_info.pQueueFamilyIndices = queue_families;
    }

    VmaAllocationCreateInfo memory_info{};
    memory_info.usage = memory_usage;

    if (image_usage & VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT) {
        memory_info.preferredFlags = VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT;
    }

    auto result = vmaCreateImage(device.GetMemoryAllocator(), &image_info, &memory_info, &handle_, &memory_, nullptr);

    if (result != VK_SUCCESS) {
        throw VulkanException{result, "Cannot create Image"};
    }
}

Image::Image(Device const &device,
             VkImage handle,
             const VkExtent3D &extent,
             VkFormat format,
             VkImageUsageFlags image_usage,
             VkSampleCountFlagBits sample_count)
    : VulkanResource{handle, &device},
      type_{FindImageType(extent)},
      extent_{extent},
      format_{format},
      sample_count_{sample_count},
      usage_{image_usage} {
    subresource_.mipLevel = 1;
    subresource_.arrayLayer = 1;
}

Image::Image(Image &&other) noexcept
    : VulkanResource{std::move(other)},
      memory_{other.memory_},
      type_{other.type_},
      extent_{other.extent_},
      format_{other.format_},
      sample_count_{other.sample_count_},
      usage_{other.usage_},
      tiling_{other.tiling_},
      subresource_{other.subresource_},
      mapped_data_{other.mapped_data_},
      mapped_{other.mapped_} {
    other.memory_ = VK_NULL_HANDLE;
    other.mapped_data_ = nullptr;
    other.mapped_ = false;

    // Update image views references to this image to avoid dangling pointers
    for (auto &view : views_) {
        view->SetImage(*this);
    }
}

Image::~Image() {
    if (handle_ != VK_NULL_HANDLE && memory_ != VK_NULL_HANDLE) {
        Unmap();
        vmaDestroyImage(device_->GetMemoryAllocator(), handle_, memory_);
    }
}

VmaAllocation Image::GetMemory() const { return memory_; }

uint8_t *Image::Map() {
    if (!mapped_data_) {
        if (tiling_ != VK_IMAGE_TILING_LINEAR) {
            LOGW("Mapping image memory that is not linear")
        }
        VK_CHECK(vmaMapMemory(device_->GetMemoryAllocator(), memory_, reinterpret_cast<void **>(&mapped_data_)));
        mapped_ = true;
    }
    return mapped_data_;
}

void Image::Unmap() {
    if (mapped_) {
        vmaUnmapMemory(device_->GetMemoryAllocator(), memory_);
        mapped_data_ = nullptr;
        mapped_ = false;
    }
}

VkImageType Image::GetType() const { return type_; }

const VkExtent3D &Image::GetExtent() const { return extent_; }

VkFormat Image::GetFormat() const { return format_; }

VkSampleCountFlagBits Image::GetSampleCount() const { return sample_count_; }

VkImageUsageFlags Image::GetUsage() const { return usage_; }

VkImageTiling Image::GetTiling() const { return tiling_; }

VkImageSubresource Image::GetSubresource() const { return subresource_; }

uint32_t Image::GetArrayLayerCount() const { return array_layer_count_; }

std::unordered_set<ImageView *> &Image::GetViews() { return views_; }

}  // namespace core
}  // namespace vox
