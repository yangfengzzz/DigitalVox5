//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/resource_binding_state.h"

namespace vox {
void ResourceBindingState::Reset() {
    ClearDirty();

    resource_sets_.clear();
}

bool ResourceBindingState::IsDirty() const { return dirty_; }

void ResourceBindingState::ClearDirty() { dirty_ = false; }

void ResourceBindingState::ClearDirty(uint32_t set) { resource_sets_[set].ClearDirty(); }

void ResourceBindingState::BindBuffer(const core::Buffer &buffer,
                                      VkDeviceSize offset,
                                      VkDeviceSize range,
                                      uint32_t set,
                                      uint32_t binding,
                                      uint32_t array_element) {
    resource_sets_[set].BindBuffer(buffer, offset, range, binding, array_element);

    dirty_ = true;
}

void ResourceBindingState::BindImage(const core::ImageView &image_view,
                                     const core::Sampler &sampler,
                                     uint32_t set,
                                     uint32_t binding,
                                     uint32_t array_element) {
    resource_sets_[set].BindImage(image_view, sampler, binding, array_element);

    dirty_ = true;
}

void ResourceBindingState::BindImage(const core::ImageView &image_view,
                                     uint32_t set,
                                     uint32_t binding,
                                     uint32_t array_element) {
    resource_sets_[set].BindImage(image_view, binding, array_element);

    dirty_ = true;
}

void ResourceBindingState::BindInput(const core::ImageView &image_view,
                                     uint32_t set,
                                     uint32_t binding,
                                     uint32_t array_element) {
    resource_sets_[set].BindInput(image_view, binding, array_element);

    dirty_ = true;
}

const std::unordered_map<uint32_t, ResourceSet> &ResourceBindingState::GetResourceSets() { return resource_sets_; }

void ResourceSet::Reset() {
    ClearDirty();

    resource_bindings_.clear();
}

bool ResourceSet::IsDirty() const { return dirty_; }

void ResourceSet::ClearDirty() { dirty_ = false; }

void ResourceSet::ClearDirty(uint32_t binding, uint32_t array_element) {
    resource_bindings_[binding][array_element].dirty = false;
}

void ResourceSet::BindBuffer(
        const core::Buffer &buffer, VkDeviceSize offset, VkDeviceSize range, uint32_t binding, uint32_t array_element) {
    resource_bindings_[binding][array_element].dirty = true;
    resource_bindings_[binding][array_element].buffer = &buffer;
    resource_bindings_[binding][array_element].offset = offset;
    resource_bindings_[binding][array_element].range = range;

    dirty_ = true;
}

void ResourceSet::BindImage(const core::ImageView &image_view,
                            const core::Sampler &sampler,
                            uint32_t binding,
                            uint32_t array_element) {
    resource_bindings_[binding][array_element].dirty = true;
    resource_bindings_[binding][array_element].image_view = &image_view;
    resource_bindings_[binding][array_element].sampler = &sampler;

    dirty_ = true;
}

void ResourceSet::BindImage(const core::ImageView &image_view, uint32_t binding, uint32_t array_element) {
    resource_bindings_[binding][array_element].dirty = true;
    resource_bindings_[binding][array_element].image_view = &image_view;
    resource_bindings_[binding][array_element].sampler = nullptr;

    dirty_ = true;
}

void ResourceSet::BindInput(const core::ImageView &image_view, uint32_t binding, uint32_t array_element) {
    resource_bindings_[binding][array_element].dirty = true;
    resource_bindings_[binding][array_element].image_view = &image_view;

    dirty_ = true;
}

const BindingMap<ResourceInfo> &ResourceSet::GetResourceBindings() const { return resource_bindings_; }

}  // namespace vox
