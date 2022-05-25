//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.base/singleton.h"
#include "vox.math/spherical_harmonics3.h"
#include "vox.render/rendering/postprocessing_computepass.h"
#include "vox.render/rendering/postprocessing_pipeline.h"
#include "vox.render/texture.h"

namespace vox {
class TextureManager : public Singleton<TextureManager> {
public:
    static TextureManager &GetSingleton();

    static TextureManager *GetSingletonPtr();

    explicit TextureManager(Device &device);

    ~TextureManager() = default;

    void CollectGarbage();

public:
    /**
     * @brief Loads in a ktx 2D texture
     */
    std::shared_ptr<Texture> LoadTexture(const std::string &file);

    /**
     * @brief Loads in a ktx 2D texture array
     */
    std::shared_ptr<Texture> LoadTextureArray(const std::string &file);

    /**
     * @brief Loads in a ktx 2D texture cubemap
     */
    std::shared_ptr<Texture> LoadTextureCubemap(const std::string &file);

    void UploadTexture(Texture *image);

public:
    std::shared_ptr<Texture> GenerateIBL(const std::string &file, RenderContext &render_context);

    SphericalHarmonics3 GenerateSH(const std::string &file);

public:
    std::shared_ptr<Texture> PackedShadowMap(CommandBuffer &command_buffer,
                                             std::vector<RenderTarget *> used_shadow,
                                             uint32_t shadow_map_resolution);

private:
    Device &device_;
    std::unordered_map<std::string, std::shared_ptr<Texture>> image_pool_;
    VkSamplerCreateInfo sampler_create_info_;
    std::unique_ptr<core::Sampler> sampler_{nullptr};

    ShaderData shader_data_;
    std::unique_ptr<PostProcessingPipeline> pipeline_{nullptr};
    PostProcessingComputePass *ibl_pass_{nullptr};

    std::shared_ptr<Texture> packed_shadow_map_{nullptr};
};

template <>
inline TextureManager *Singleton<TextureManager>::ms_singleton{nullptr};

}  // namespace vox
