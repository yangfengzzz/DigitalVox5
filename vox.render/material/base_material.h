//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "material.h"
#include "enums/blend_mode.h"
#include "enums/render_face.h"

namespace vox {
class BaseMaterial : public Material {
public:
    /**
     * Is this material transparent.
     * @remarks
     * If material is transparent, transparent blend mode will be affected by `blend_mode`, default is `BlendMode.Normal`.
     */
    [[nodiscard]] bool is_transparent() const;
    
    void set_is_transparent(bool new_value);
    
    /**
     * Alpha cutoff value.
     * @remarks
     * Fragments with alpha channel lower than cutoff value will be discarded.
     * `0` means no fragment will be discarded.
     */
    [[nodiscard]] float alpha_cutoff() const;
    
    void set_alpha_cutoff(float new_value);
    
    /**
     * Set which face for render.
     */
    [[nodiscard]] const RenderFace &render_face() const;
    
    void set_render_face(const RenderFace &new_value);
    
    /**
     * Alpha blend mode.
     * @remarks
     * Only take effect when `is_transparent` is `true`.
     */
    [[nodiscard]] const BlendMode &blend_mode() const;
    
    void set_blend_mode(const BlendMode &new_value);
    
    /**
     * Create a BaseMaterial instance.
     */
    BaseMaterial(Device &device, const std::string &name);
    
protected:
    static VkSamplerCreateInfo last_sampler_create_info_;
    
    std::function<core::Sampler*(const VkSamplerCreateInfo& info)> get_sampler_;
    
private:
    float alpha_cutoff_ = 0.0;
    ShaderProperty alpha_cutoff_prop_;
    
    RenderFace render_face_ = RenderFace::BACK;
    BlendMode blend_mode_ = BlendMode::NORMAL;
    bool is_transparent_ = false;
};

}
