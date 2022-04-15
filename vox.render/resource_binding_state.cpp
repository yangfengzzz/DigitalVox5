//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "resource_binding_state.h"

namespace vox {
void ResourceBindingState::reset() {
    clear_dirty();
    
    resource_sets_.clear();
}

bool ResourceBindingState::is_dirty() const {
    return dirty_;
}

void ResourceBindingState::clear_dirty() {
    dirty_ = false;
}

void ResourceBindingState::clear_dirty(uint32_t set) {
    resource_sets_[set].clear_dirty();
}

void
ResourceBindingState::bind_buffer(const core::Buffer &buffer, VkDeviceSize offset, VkDeviceSize range, uint32_t set,
                                  uint32_t binding, uint32_t array_element) {
    resource_sets_[set].bind_buffer(buffer, offset, range, binding, array_element);
    
    dirty_ = true;
}

void ResourceBindingState::bind_image(const core::ImageView &image_view, const core::Sampler &sampler, uint32_t set,
                                      uint32_t binding, uint32_t array_element) {
    resource_sets_[set].bind_image(image_view, sampler, binding, array_element);
    
    dirty_ = true;
}

void ResourceBindingState::bind_image(const core::ImageView &image_view, uint32_t set, uint32_t binding,
                                      uint32_t array_element) {
    resource_sets_[set].bind_image(image_view, binding, array_element);
    
    dirty_ = true;
}

void ResourceBindingState::bind_input(const core::ImageView &image_view, uint32_t set, uint32_t binding,
                                      uint32_t array_element) {
    resource_sets_[set].bind_input(image_view, binding, array_element);
    
    dirty_ = true;
}

const std::unordered_map<uint32_t, ResourceSet> &ResourceBindingState::get_resource_sets() {
    return resource_sets_;
}

void ResourceSet::reset() {
    clear_dirty();
    
    resource_bindings_.clear();
}

bool ResourceSet::is_dirty() const {
    return dirty_;
}

void ResourceSet::clear_dirty() {
    dirty_ = false;
}

void ResourceSet::clear_dirty(uint32_t binding, uint32_t array_element) {
    resource_bindings_[binding][array_element].dirty = false;
}

void ResourceSet::bind_buffer(const core::Buffer &buffer, VkDeviceSize offset, VkDeviceSize range, uint32_t binding,
                              uint32_t array_element) {
    resource_bindings_[binding][array_element].dirty = true;
    resource_bindings_[binding][array_element].buffer = &buffer;
    resource_bindings_[binding][array_element].offset = offset;
    resource_bindings_[binding][array_element].range = range;
    
    dirty_ = true;
}

void ResourceSet::bind_image(const core::ImageView &image_view, const core::Sampler &sampler, uint32_t binding,
                             uint32_t array_element) {
    resource_bindings_[binding][array_element].dirty = true;
    resource_bindings_[binding][array_element].image_view = &image_view;
    resource_bindings_[binding][array_element].sampler = &sampler;
    
    dirty_ = true;
}

void ResourceSet::bind_image(const core::ImageView &image_view, uint32_t binding, uint32_t array_element) {
    resource_bindings_[binding][array_element].dirty = true;
    resource_bindings_[binding][array_element].image_view = &image_view;
    resource_bindings_[binding][array_element].sampler = nullptr;
    
    dirty_ = true;
}

void
ResourceSet::bind_input(const core::ImageView &image_view, const uint32_t binding, const uint32_t array_element) {
    resource_bindings_[binding][array_element].dirty = true;
    resource_bindings_[binding][array_element].image_view = &image_view;
    
    dirty_ = true;
}

const BindingMap<ResourceInfo> &ResourceSet::get_resource_bindings() const {
    return resource_bindings_;
}

}        // namespace vox
