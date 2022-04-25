//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "rendering/subpass.h"

namespace vox {
class ShadowSubpass : public Subpass {
public:
    ShadowSubpass(RenderContext &render_context,
                  Scene *scene,
                  Camera *camera);
    
    /**
     * @brief Thread index to use for allocating resources
     */
    void set_thread_index(uint32_t index);
    
    void set_view_projection_matrix(const Matrix4x4F &vp);
    
    void set_viewport(const std::optional<VkViewport> &viewport);
    
    void prepare() override;
    
    void draw(CommandBuffer &command_buffer) override;
    
protected:
    void draw_element(CommandBuffer &command_buffer,
                      const std::vector<RenderElement> &items,
                      const ShaderVariant &variant);
    
    InputAssemblyState input_assembly_state_;
    RasterizationState rasterization_state_;
    MultisampleState multisample_state_;
    DepthStencilState depth_stencil_state_;
    
    std::shared_ptr<ShaderSource> vertex_source_{nullptr};
    
    uint32_t thread_index_{0};
    Matrix4x4F vp_;
    std::vector<VkViewport> viewports_{};
};

}
