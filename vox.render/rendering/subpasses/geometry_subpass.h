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
    enum class RenderMode {
        AUTO,
        MANUAL
    };
    
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
    
public:
    [[nodiscard]] RenderMode render_mode() const;
    
    void set_render_mode(RenderMode mode);
    
    void add_render_element(const RenderElement &element);
    
    void clear_all_render_element();
    
protected:
    void draw_element(CommandBuffer &command_buffer,
                      const std::vector<RenderElement> &items,
                      const ShaderVariant &variant);
    
    uint32_t thread_index_{0};
    
    RenderMode mode_ = RenderMode::AUTO;
    std::vector<RenderElement> elements_{};
};

}
