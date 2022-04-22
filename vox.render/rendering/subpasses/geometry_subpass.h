//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "rendering/subpass.h"

namespace vox {
/**
 * @brief This subpass is responsible for rendering a Scene
 */
class GeometrySubpass : public Subpass {
public:
    /**
     * @brief Constructs a subpass for the geometry pass of Deferred rendering
     * @param render_context Render context
     * @param scene Scene to render on this subpass
     * @param camera Camera used to look at the scene
     */
    GeometrySubpass(RenderContext &render_context, Scene *scene, Camera *camera);
    
    ~GeometrySubpass() override = default;
    
    void prepare() override;
    
    /**
     * @brief Record draw commands
     */
    void draw(CommandBuffer &command_buffer) override;
    
    /**
     * @brief Thread index to use for allocating resources
     */
    void set_thread_index(uint32_t index);
    
protected:
    void draw_element(CommandBuffer &command_buffer,
                      const std::vector<RenderElement> &items,
                      const ShaderVariant &variant);
    
    virtual PipelineLayout &prepare_pipeline_layout(CommandBuffer &command_buffer,
                                                    const std::vector<ShaderModule *> &shader_modules);
    
    uint32_t thread_index_{0};
};

}
