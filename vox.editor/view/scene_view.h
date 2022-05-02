//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "view.h"
#include "controls/orbit_control.h"
#include "imgui/imgui_zmo.h"
#include "rendering/subpasses/color_picker_subpass.h"

namespace vox {
using namespace ui;
class ColorPickerSubpass;

namespace editor::ui {
class SceneView : public View {
public:
    SceneView(const std::string &title, bool opened,
              const PanelWindowSettings &window_settings,
              RenderContext &render_context, Scene *scene);
    
    /**
     * Custom implementation of the draw method
     */
    void draw_impl() override;
    
    /**
     * Update the scene view
     */
    void update(float delta_time) override;
    
    /**
     * Custom implementation of the render method
     */
    void render(CommandBuffer &command_buffer) override;
    
    /**
     * Pick the object at the screen coordinate position.
     * @param offset_x Relative X coordinate of the canvas
     * @param offset_y Relative Y coordinate of the canvas
     */
    void pick(float offset_x, float offset_y);
    
    void input_event(const InputEvent &input_event);
    
    void load_scene(Entity *root_entity);
    
public:
    control::OrbitControl *camera_control();
    
private:
    float cam_distance_ = 8.f;
    ImGuizmo::OPERATION current_gizmo_operation_ = ImGuizmo::TRANSLATE;
    void edit_transform(float *camera_view, float *camera_projection, float *matrix);
    
private:
    bool elapsed_frames_{true};
    Camera *main_camera_{nullptr};
    Scene *scene_{nullptr};
    control::OrbitControl *camera_control_{nullptr};
    
private:    
    bool need_pick_{};
    Vector2F pick_pos_;
    
    std::unique_ptr<RenderTarget> color_picker_render_target_{nullptr};
    std::unique_ptr<RenderPipeline> color_picker_render_pipeline_{nullptr};
    ColorPickerSubpass *color_picker_subpass_{nullptr};
    std::pair<Renderer *, MeshPtr> pick_result_;
    
    std::array<uint8_t, 4> pixel_{};
    std::unique_ptr<core::Buffer> stage_buffer_;
    
    std::vector<VkBufferImageCopy> regions_;
    
    void copy_render_target_to_buffer(CommandBuffer &command_buffer);
    void read_color_from_render_target();
};

}
}
