//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <random>

#include "vox.render/forward_application.h"
#include "vox.render/rendering/subpasses/color_picker_subpass.h"

namespace vox {
class FramebufferPickerApp : public ForwardApplication {
public:
    void LoadScene() override;

    void PickFunctor(Renderer *renderer, const MeshPtr &mesh);

public:
    bool Prepare(Platform &platform) override;

    bool Resize(uint32_t win_width, uint32_t win_height, uint32_t fb_width, uint32_t fb_height) override;

public:
    /**
     * Pick the object at the screen coordinate position.
     * @param offset_x Relative X coordinate of the canvas
     * @param offset_y Relative Y coordinate of the canvas
     */
    void Pick(float offset_x, float offset_y);

    void InputEvent(const vox::InputEvent &input_event) override;

    void Update(float delta_time) override;

    void Render(CommandBuffer &command_buffer, RenderTarget &render_target) override;

private:
    std::unique_ptr<RenderTarget> CreateRenderTarget(uint32_t width, uint32_t height);

    bool need_pick_;
    Vector2F pick_pos_;

    std::unique_ptr<RenderTarget> color_picker_render_target_{nullptr};
    std::unique_ptr<RenderPipeline> color_picker_render_pipeline_{nullptr};
    ColorPickerSubpass *color_picker_subpass_{nullptr};
    std::pair<Renderer *, MeshPtr> pick_result_;

    std::array<uint8_t, 4> pixel_{};
    std::unique_ptr<core::Buffer> stage_buffer_;

    std::vector<VkBufferImageCopy> regions_;

    void CopyRenderTargetToBuffer(CommandBuffer &command_buffer);
    void ReadColorFromRenderTarget();

private:
    std::default_random_engine e;
    std::uniform_real_distribution<float> u;
};

}  // namespace vox
