//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <string>
#include <unordered_map>
#include "image_manager.h"
#include "core/sampler.h"

namespace vox::editor {
/**
 * Handle the creation and storage of editor specific resources
 */
class EditorResources {
public:
    EditorResources(Device &device,
                    const std::string &editor_assets_path);
    
    /**
     * Destructor
     */
    ~EditorResources() = default;
    
    /**
     * Returns the file icon identified by the given string or nullptr on fail
     */
    VkDescriptorSet get_file_icon(const std::string &filename);
    
    /**
     * Returns the texture identified by the given string or nullptr on fail
     */
    VkDescriptorSet get_texture(const std::string &id);
    
private:
    Device &device_;
    std::unordered_map<std::string, VkDescriptorSet> texture_ids_;
    std::vector<std::unique_ptr<::vox::Image>> images_;
    VkSamplerCreateInfo sampler_create_info_;
    std::unique_ptr<core::Sampler> sampler_{nullptr};
    
    VkDescriptorSet create_from_pixel_buffer(const std::vector<uint64_t> &data, uint8_t width);
};

}
