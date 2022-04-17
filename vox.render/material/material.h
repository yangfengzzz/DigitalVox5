//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "rendering/pipeline_state.h"
#include "shader/shader_data.h"
#include "enums/render_queue_type.h"
#include "resource_caching.h"

namespace vox {
/**
 * Material.
 */
class Material {
public:
    /** Name. */
    std::string name_;
    
    /** Render queue type. */
    RenderQueueType::Enum render_queue_ = RenderQueueType::Enum::OPAQUE;
    
    /** Shader used by the material. */
    std::vector<ShaderSource> shader_source_;
    
    /** Shader data. */
    ShaderData shader_data_;
    
    /** Render state. */
    InputAssemblyState input_assembly_state_;
    RasterizationState rasterization_state_;
    MultisampleState multisample_state_;
    DepthStencilState depth_stencil_state_;
    ColorBlendState color_blend_state_;
    
    /** sampler pool*/
    static std::unordered_map<VkSamplerCreateInfo, core::Sampler> sampler_pool_;
    
    const std::function<core::Sampler*(const VkSamplerCreateInfo& info)> get_sampler_;
    
    explicit Material(Device &device, std::string name);
    
    Material(Material &&other) = default;
    
    virtual ~Material() = default;
};

}        // namespace vox
