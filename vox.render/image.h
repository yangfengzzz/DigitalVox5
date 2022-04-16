//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <memory>
#include <string>
#include <typeinfo>
#include <vector>

#include <volk.h>

#include "core/image.h"
#include "core/image_view.h"

namespace vox {
/**
 * @param format Vulkan format
 * @return Whether the vulkan format is ASTC
 */
bool is_astc(VkFormat format);

/**
 * @brief Mipmap information
 */
struct Mipmap {
    /// Mipmap level
    uint32_t level = 0;
    
    /// Byte offset used for uploading
    uint32_t offset = 0;
    
    /// Width depth and height of the mipmap
    VkExtent3D extent = {0, 0, 0};
};

class Image {
public:
    std::string name_;
    
    explicit Image(std::string name, std::vector<uint8_t> &&data = {}, std::vector<Mipmap> &&mipmaps = {{}});
    
    static std::shared_ptr<Image> load(const std::string &name, const std::string &uri);
    
    virtual ~Image() = default;
    
    [[nodiscard]] const std::vector<uint8_t> &get_data() const;
    
    void clear_data();
    
    [[nodiscard]] VkFormat get_format() const;
    
    [[nodiscard]] const VkExtent3D &get_extent() const;
    
    [[nodiscard]] uint32_t get_layers() const;
    
    [[nodiscard]] const std::vector<Mipmap> &get_mipmaps() const;
    
    [[nodiscard]] const std::vector<std::vector<VkDeviceSize>> &get_offsets() const;
    
    void generate_mipmaps();
    
    void create_vk_image(Device const &device, VkImageViewType image_view_type = VK_IMAGE_VIEW_TYPE_2D,
                         VkImageCreateFlags flags = 0);
    
    [[nodiscard]] const core::Image &get_vk_image() const;
    
    [[nodiscard]] const core::ImageView &get_vk_image_view() const;
    
protected:
    std::vector<uint8_t> &get_mut_data();
    
    void set_data(const uint8_t *raw_data, size_t size);
    
    void set_format(VkFormat format);
    
    void set_width(uint32_t width);
    
    void set_height(uint32_t height);
    
    void set_depth(uint32_t depth);
    
    void set_layers(uint32_t layers);
    
    void set_offsets(const std::vector<std::vector<VkDeviceSize>> &offsets);
    
    Mipmap &get_mipmap(size_t index);
    
    std::vector<Mipmap> &get_mut_mipmaps();
    
private:
    std::vector<uint8_t> data_;
    
    VkFormat format_{VK_FORMAT_UNDEFINED};
    
    uint32_t layers_{1};
    
    std::vector<Mipmap> mipmaps_{{}};
    
    // Offsets stored like offsets[array_layer][mipmap_layer]
    std::vector<std::vector<VkDeviceSize>> offsets_;
    
    std::unique_ptr<core::Image> vk_image_;
    
    std::unique_ptr<core::ImageView> vk_image_view_;
};

}        // namespace vox
