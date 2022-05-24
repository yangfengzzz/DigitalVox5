//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.render/rendering/subpass.h"
#include "vox.render/shader/shader_source.h"

namespace vox {
/**
 * @brief This subpass is responsible for rendering a Skybox
 */
class SkyboxSubpass : public Subpass {
public:
    /**
     * @brief Constructs a subpass for the geometry pass of Deferred rendering
     * @param render_context Render context
     * @param scene Scene to render on this subpass
     * @param camera Camera used to look at the scene
     */
    SkyboxSubpass(RenderContext &render_context, Scene *scene, Camera *camera);

    ~SkyboxSubpass() override = default;

    void Prepare() override;

    /**
     * @brief Record draw commands
     */
    void Draw(CommandBuffer &command_buffer) override;

public:
    void CreateCuboid();

    void FlipVertically();

    /**
     * Texture cube map of the sky box material.
     */
    [[nodiscard]] const std::shared_ptr<Texture> &TextureCubeMap() const;

    void SetTextureCubeMap(const std::shared_ptr<Texture> &v);

private:
    bool is_flip_vertically_{false};
    ModelMeshPtr mesh_{nullptr};
    std::shared_ptr<Texture> cube_map_{nullptr};
    std::unique_ptr<core::Sampler> cube_sampler_{nullptr};
    std::unique_ptr<core::Buffer> vp_matrix_{nullptr};

    InputAssemblyState input_assembly_state_;
    RasterizationState rasterization_state_;
    MultisampleState multisample_state_;
    DepthStencilState depth_stencil_state_;
    ColorBlendState color_blend_state_;

    ShaderVariant variant_;
    ShaderSource vert_shader_;
    ShaderSource frag_shader_;
};

}  // namespace vox
