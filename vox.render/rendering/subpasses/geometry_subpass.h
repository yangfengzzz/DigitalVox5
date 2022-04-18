//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "rendering/subpass.h"
#include "scene.h"

namespace vox {
/**
 * @brief This subpass is responsible for rendering a Scene
 */
class GeometrySubpass : public Subpass {
public:
    /**
     * @brief Constructs a subpass for the geometry pass of Deferred rendering
     * @param render_context Render context
     * @param vertex_shader Vertex shader source
     * @param fragment_shader Fragment shader source
     * @param scene Scene to render on this subpass
     * @param camera Camera used to look at the scene
     */
    GeometrySubpass(RenderContext &render_context, ShaderSource &&vertex_shader,
                    ShaderSource &&fragment_shader, Scene *scene, Camera *camera);
    
    virtual ~GeometrySubpass() = default;
    
    virtual void prepare() override;
    
    /**
     * @brief Record draw commands
     */
    virtual void draw(CommandBuffer &command_buffer) override;
    
    /**
     * @brief Thread index to use for allocating resources
     */
    void set_thread_index(uint32_t index);
    
private:
    void _drawElement(CommandBuffer &command_buffer,
                      const std::vector<RenderElement> &items,
                      const ShaderVariant& variant);
    
protected:
    Camera *camera{nullptr};
    Scene *scene{nullptr};

    uint32_t thread_index{0};
};

}
