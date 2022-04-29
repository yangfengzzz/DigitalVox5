//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "editor_resources.h"
#include "raw_icon.h"
#include "platform/filesystem.h"
#include "core/device.h"
#include "ui/imgui_impl_vulkan.h"
#include <vector>

namespace vox::editor {
EditorResources::EditorResources(Device &device,
                                 const std::string &editor_assets_path) :
device_(device),
sampler_create_info_{VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO} {
    // Create a default sampler
    sampler_create_info_.magFilter = VK_FILTER_LINEAR;
    sampler_create_info_.minFilter = VK_FILTER_LINEAR;
    sampler_create_info_.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    sampler_create_info_.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    sampler_create_info_.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    sampler_create_info_.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    sampler_create_info_.mipLodBias = 0.0f;
    sampler_create_info_.compareOp = VK_COMPARE_OP_NEVER;
    sampler_create_info_.minLod = 0.0f;
    // Max level-of-detail should match mip level count
    sampler_create_info_.maxLod = 0.0f;
    // Only enable anisotropic filtering if enabled on the device
    // Note that for simplicity, we will always be using max. available anisotropy level for the current device
    // This may have an impact on performance, esp. on lower-specced devices
    // In a real-world scenario the level of anisotropy should be a user setting or e.g. lowered for mobile devices by default
    sampler_create_info_.maxAnisotropy = device.get_gpu().get_features().samplerAnisotropy
    ? (device.get_gpu().get_properties().limits.maxSamplerAnisotropy)
    : 1.0f;
    sampler_create_info_.anisotropyEnable = device.get_gpu().get_features().samplerAnisotropy;
    sampler_create_info_.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
    
    sampler_ = std::make_unique<core::Sampler>(device, sampler_create_info_);
    
    /* Buttons */
    {
        texture_ids_["Button_Play"] = create_from_pixel_buffer(BUTTON_PLAY, 64);
        texture_ids_["Button_Pause"] = create_from_pixel_buffer(BUTTON_PAUSE, 64);
        texture_ids_["Button_Stop"] = create_from_pixel_buffer(BUTTON_STOP, 64);
        texture_ids_["Button_Next"] = create_from_pixel_buffer(BUTTON_NEXT, 64);
        texture_ids_["Button_Refresh"] = create_from_pixel_buffer(BUTTON_REFRESH, 64);
    }
    
    /* Icons */
    {
        texture_ids_["Icon_Unknown"] = create_from_pixel_buffer(ICON_FILE, 16);
        texture_ids_["Icon_Folder"] = create_from_pixel_buffer(ICON_FOLDER, 16);
        texture_ids_["Icon_Texture"] = create_from_pixel_buffer(ICON_TEXTURE, 16);
        texture_ids_["Icon_Model"] = create_from_pixel_buffer(ICON_MODEL, 16);
        texture_ids_["Icon_Shader"] = create_from_pixel_buffer(ICON_SHADER, 16);
        texture_ids_["Icon_Material"] = create_from_pixel_buffer(ICON_MATERIAL, 16);
        texture_ids_["Icon_Scene"] = create_from_pixel_buffer(ICON_SCENE, 16);
        texture_ids_["Icon_Sound"] = create_from_pixel_buffer(ICON_SOUND, 16);
        texture_ids_["Icon_Script"] = create_from_pixel_buffer(ICON_SCRIPT, 16);
        texture_ids_["Icon_Font"] = create_from_pixel_buffer(ICON_FONT, 16);
        
        texture_ids_["Bill_Point_Light"] = create_from_pixel_buffer(BILL_PLIGHT, 128);
        texture_ids_["Bill_Spot_Light"] = create_from_pixel_buffer(BILL_SLIGHT, 128);
        texture_ids_["Bill_Directional_Light"] = create_from_pixel_buffer(BILL_DLIGHT, 128);
        texture_ids_["Bill_Ambient_Box_Light"] = create_from_pixel_buffer(BILL_ABLIGHT, 128);
        texture_ids_["Bill_Ambient_Sphere_Light"] = create_from_pixel_buffer(BILL_ASLIGHT, 128);
        
    }
}

VkDescriptorSet EditorResources::get_file_icon(const std::string &filename) {
    return get_texture("Icon_" + fs::file_type_to_string(fs::extra_file_type(filename)));
}

VkDescriptorSet EditorResources::get_texture(const std::string &id) {
    if (texture_ids_.find(id) != texture_ids_.end())
        return texture_ids_.at(id);
    
    return VK_NULL_HANDLE;
}

VkDescriptorSet EditorResources::create_from_pixel_buffer(const std::vector<uint64_t> &data, uint8_t width) {
    std::vector<uint8_t> raw_data{reinterpret_cast<const uint8_t *>(data.data()),
        reinterpret_cast<const uint8_t *>(data.data()) + sizeof(uint64_t) * data.size()};
    std::vector<Mipmap> mip_map(1);
    mip_map[0].extent.width = width;
    mip_map[0].extent.height = width;
    mip_map[0].extent.depth = 1;
    auto image = std::make_unique<Image>("icon", std::move(raw_data), std::move(mip_map));
    image->create_vk_image(device_);
    ImageManager::get_singleton().upload_image(image.get());
    
    auto descriptor = ImGui_ImplVulkan_AddTexture(sampler_->get_handle(), image->get_vk_image_view().get_handle(),
                                                  VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    images_.emplace_back(std::move(image));
    return descriptor;
}

}
