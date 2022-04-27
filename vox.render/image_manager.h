//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "image.h"
#include "singleton.h"
#include "rendering/postprocessing_pipeline.h"
#include "rendering/postprocessing_computepass.h"
#include "spherical_harmonics3.h"

namespace vox {
class ImageManager: public Singleton<ImageManager> {
public:
    static ImageManager &get_singleton();
    
    static ImageManager *get_singleton_ptr();
    
    explicit ImageManager(Device& device);
    
    ~ImageManager() = default;
    
    void collect_garbage();

public:
    /**
     * @brief Loads in a ktx 2D texture
     */
    std::shared_ptr<Image> load_texture(const std::string &file);
    
    /**
     * @brief Loads in a ktx 2D texture array
     */
    std::shared_ptr<Image> load_texture_array(const std::string &file);
    
    /**
     * @brief Loads in a ktx 2D texture cubemap
     */
    std::shared_ptr<Image> load_texture_cubemap(const std::string &file);
    
public:
    std::shared_ptr<Image> generate_ibl(const std::string &file,
                                        RenderContext &render_context);
    
    SphericalHarmonics3 generate_sh(const std::string &file);
    
public:
    std::shared_ptr<Image> packed_shadow_map(CommandBuffer &command_buffer,
                                             std::vector<RenderTarget*> used_shadow,
                                             uint32_t shadow_map_resolution);
    
private:
    Device& device_;
    std::unordered_map<std::string, std::shared_ptr<Image>> image_pool_;
    VkSamplerCreateInfo sampler_create_info_;
    std::unique_ptr<core::Sampler> sampler_{nullptr};
    
    ShaderData shader_data_;
    std::unique_ptr<PostProcessingPipeline> pipeline_{nullptr};
    PostProcessingComputePass* ibl_pass_{nullptr};
    
    std::shared_ptr<Image> packed_shadow_map_{nullptr};
};

template<> inline ImageManager *Singleton<ImageManager>::ms_singleton_{nullptr};

}
