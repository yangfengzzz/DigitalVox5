//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.render/core/device.h"
#include "vox.render/material/enums/render_queue_type.h"
#include "vox.render/rendering/pipeline_state.h"
#include "vox.render/shader/shader_data.h"

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
    std::shared_ptr<ShaderSource> vertex_source_{nullptr};
    std::shared_ptr<ShaderSource> fragment_source_{nullptr};

    /** Shader data. */
    ShaderData shader_data_;

    /** Render state. */
    InputAssemblyState input_assembly_state_;
    RasterizationState rasterization_state_;
    MultisampleState multisample_state_;
    DepthStencilState depth_stencil_state_;
    ColorBlendState color_blend_state_;

    explicit Material(Device &device, std::string name = "");

    Material(Material &&other) = default;

    virtual ~Material() = default;

protected:
    Device &device_;
};

}  // namespace vox
