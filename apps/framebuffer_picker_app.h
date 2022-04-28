//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "forward_application.h"
#include "rendering/subpasses/color_picker_subpass.h"
#include <random>

namespace vox {
class FramebufferPickerApp : public ForwardApplication {
public:
    void load_scene() override;
    
    void pick_functor(Renderer *renderer, const MeshPtr &mesh);
    
public:
    bool prepare(Platform &platform) override;
    
    bool resize(uint32_t win_width, uint32_t win_height,
                uint32_t fb_width, uint32_t fb_height) override;
    
public:
    /**
     * Pick the object at the screen coordinate position.
     * @param offset_x Relative X coordinate of the canvas
     * @param offset_y Relative Y coordinate of the canvas
     */
    void pick(float offset_x, float offset_y);
    
    void input_event(const InputEvent &input_event) override;
    
    void update(float delta_time) override;
    
    void render(CommandBuffer &command_buffer, RenderTarget &render_target) override;
    
private:
    std::unique_ptr<RenderTarget> create_render_target(uint32_t width, uint32_t height);
    
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
    
private:
    std::default_random_engine e;
    std::uniform_real_distribution<float> u;
};

}
