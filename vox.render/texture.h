//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <volk.h>

#include <memory>
#include <string>
#include <typeinfo>
#include <vector>

#include "vox.render/core/image.h"
#include "vox.render/core/image_view.h"

namespace vox {
/**
 * @param format Vulkan format
 * @return Whether the vulkan format is ASTC
 */
bool IsAstc(VkFormat format);

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

class Texture {
public:
    std::string name_;

    explicit Texture(std::string name, std::vector<uint8_t> &&data = {}, std::vector<Mipmap> &&mipmaps = {{}});

    static std::shared_ptr<Texture> Load(const std::string &name, const std::string &uri);

    virtual ~Texture() = default;

    [[nodiscard]] const std::vector<uint8_t> &GetData() const;

    void ClearData();

    [[nodiscard]] VkFormat GetFormat() const;

    [[nodiscard]] const VkExtent3D &GetExtent() const;

    [[nodiscard]] uint32_t GetLayers() const;

    [[nodiscard]] const std::vector<Mipmap> &GetMipmaps() const;

    [[nodiscard]] const std::vector<std::vector<VkDeviceSize>> &GetOffsets() const;

    void GenerateMipmaps();

    void CreateVkImage(Device const &device,
                       VkImageCreateFlags flags = 0,
                       VkImageUsageFlags image_usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT);

    [[nodiscard]] const core::Image &GetVkImage() const;

    [[nodiscard]] const core::ImageView &GetVkImageView(VkImageViewType view_type = VK_IMAGE_VIEW_TYPE_2D,
                                                        uint32_t base_mip_level = 0,
                                                        uint32_t base_array_layer = 0,
                                                        uint32_t n_mip_levels = 0,
                                                        uint32_t n_array_layers = 0);

protected:
    friend class TextureManager;

    std::vector<uint8_t> &GetMutData();

    void SetData(const uint8_t *raw_data, size_t size);

    void SetFormat(VkFormat format);

    void SetWidth(uint32_t width);

    void SetHeight(uint32_t height);

    void SetDepth(uint32_t depth);

    void SetLayers(uint32_t layers);

    void SetOffsets(const std::vector<std::vector<VkDeviceSize>> &offsets);

    Mipmap &GetMipmap(size_t index);

    std::vector<Mipmap> &GetMutMipmaps();

private:
    std::vector<uint8_t> data_;

    VkFormat format_{VK_FORMAT_UNDEFINED};

    uint32_t layers_{1};

    std::vector<Mipmap> mipmaps_{{}};

    // Offsets stored like offsets[array_layer][mipmap_layer]
    std::vector<std::vector<VkDeviceSize>> offsets_;

    std::unique_ptr<core::Image> vk_image_;

    std::unordered_map<size_t, std::unique_ptr<core::ImageView>> vk_image_views_;
};

}  // namespace vox
