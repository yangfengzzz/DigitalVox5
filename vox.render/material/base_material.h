//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.math/vector4.h"
#include "vox.render/material/enums/blend_mode.h"
#include "vox.render/material/enums/render_face.h"
#include "vox.render/material/material.h"

namespace vox {
class BaseMaterial : public Material {
public:
    /**
     * Is this material transparent.
     * @remarks
     * If material is transparent, transparent blend mode will be affected by `blend_mode`, default is
     * `BlendMode.Normal`.
     */
    [[nodiscard]] bool IsTransparent() const;

    void SetIsTransparent(bool new_value);

    /**
     * Alpha cutoff value.
     * @remarks
     * Fragments with alpha channel lower than cutoff value will be discarded.
     * `0` means no fragment will be discarded.
     */
    [[nodiscard]] float AlphaCutoff() const;

    void SetAlphaCutoff(float new_value);

    /**
     * Set which face for render.
     */
    [[nodiscard]] const RenderFace &GetRenderFace() const;

    void SetRenderFace(const RenderFace &new_value);

    /**
     * Alpha blend mode.
     * @remarks
     * Only take effect when `is_transparent` is `true`.
     */
    [[nodiscard]] const BlendMode &GetBlendMode() const;

    void SetBlendMode(const BlendMode &new_value);

    /**
     * Tiling and offset of main textures.
     */
    [[nodiscard]] const Vector4F &TilingOffset() const;

    void SetTilingOffset(const Vector4F &new_value);

    /**
     * Create a BaseMaterial instance.
     */
    BaseMaterial(Device &device, const std::string &name = "");

protected:
    static VkSamplerCreateInfo last_sampler_create_info_;

private:
    float alpha_cutoff_ = 0.0;
    const std::string alpha_cutoff_prop_;

    Vector4F tiling_offset_ = Vector4F(1, 1, 0, 0);
    const std::string tiling_offset_prop_;

    RenderFace render_face_ = RenderFace::BACK;
    BlendMode blend_mode_ = BlendMode::NORMAL;
    bool is_transparent_ = false;
};

}  // namespace vox
