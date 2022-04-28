//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "forward_application.h"
#include "rendering/subpasses/color_picker_subpass.h"

namespace vox {
class FramebufferPickerApp : public ForwardApplication {
public:
    void load_scene() override;
    
    void pick_functor(Renderer *renderer, MeshPtr mesh);
    
    /**
     * Pick the object at the screen coordinate position.
     * @param offset_x Relative X coordinate of the canvas
     * @param offset_y Relative Y coordinate of the canvas
     */
    void pick(float offset_x, float offset_y);
    
private:
    bool need_pick_;
    Vector2F pick_pos_;
    
    std::unique_ptr<RenderTarget> color_picker_render_target_{nullptr};
    std::unique_ptr<RenderPipeline> color_picker_render_pipeline_{nullptr};
    ColorPickerSubpass *color_picker_subpass_{nullptr};
    std::pair<Renderer *, MeshPtr> pick_result_;
    
    std::array<uint8_t, 4> pixel_{};
    std::unique_ptr<core::Buffer> stage_buffer_;
    
    void copy_render_target_to_buffer(CommandBuffer &command_buffer);
    void read_color_from_render_target();
};

}
