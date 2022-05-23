//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.render/core/buffer.h"
#include "vox.render/core/image_view.h"
#include "vox.render/core/sampler.h"
#include "vox.render/vk_common.h"

namespace vox {
/**
 * @brief A resource info is a struct containing the actual resource data.
 *
 * This will be referenced by a buffer info or image info descriptor inside a descriptor set.
 */
struct ResourceInfo {
    bool dirty{false};

    const core::Buffer *buffer{nullptr};

    VkDeviceSize offset{0};

    VkDeviceSize range{0};

    const core::ImageView *image_view{nullptr};

    const core::Sampler *sampler{nullptr};
};

/**
 * @brief A resource set is a set of bindings containing resources that were bound
 *        by a command buffer.
 *
 * The ResourceSet has a one to one mapping with a DescriptorSet.
 */
class ResourceSet {
public:
    void Reset();

    [[nodiscard]] bool IsDirty() const;

    void ClearDirty();

    void ClearDirty(uint32_t binding, uint32_t array_element);

    void BindBuffer(const core::Buffer &buffer,
                    VkDeviceSize offset,
                    VkDeviceSize range,
                    uint32_t binding,
                    uint32_t array_element);

    void BindImage(const core::ImageView &image_view,
                   const core::Sampler &sampler,
                   uint32_t binding,
                   uint32_t array_element);

    void BindImage(const core::ImageView &image_view, uint32_t binding, uint32_t array_element);

    void BindInput(const core::ImageView &image_view, uint32_t binding, uint32_t array_element);

    [[nodiscard]] const BindingMap<ResourceInfo> &GetResourceBindings() const;

private:
    bool dirty_{false};

    BindingMap<ResourceInfo> resource_bindings_;
};

/**
 * @brief The resource binding state of a command buffer.
 *
 * Keeps track of all the resources bound by the command buffer. The ResourceBindingState is used by
 * the command buffer to create the appropriate descriptor sets when it comes to draw.
 */
class ResourceBindingState {
public:
    void Reset();

    [[nodiscard]] bool IsDirty() const;

    void ClearDirty();

    void ClearDirty(uint32_t set);

    void BindBuffer(const core::Buffer &buffer,
                    VkDeviceSize offset,
                    VkDeviceSize range,
                    uint32_t set,
                    uint32_t binding,
                    uint32_t array_element);

    void BindImage(const core::ImageView &image_view,
                   const core::Sampler &sampler,
                   uint32_t set,
                   uint32_t binding,
                   uint32_t array_element);

    void BindImage(const core::ImageView &image_view, uint32_t set, uint32_t binding, uint32_t array_element);

    void BindInput(const core::ImageView &image_view, uint32_t set, uint32_t binding, uint32_t array_element);

    const std::unordered_map<uint32_t, ResourceSet> &GetResourceSets();

private:
    bool dirty_{false};

    std::unordered_map<uint32_t, ResourceSet> resource_sets_;
};

}  // namespace vox
