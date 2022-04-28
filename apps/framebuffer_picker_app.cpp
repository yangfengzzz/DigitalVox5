//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "framebuffer_picker_app.h"
#include "mesh/primitive_mesh.h"
#include "mesh/mesh_renderer.h"
#include "material/unlit_material.h"
#include "material/blinn_phong_material.h"
#include "camera.h"

namespace vox {
void FramebufferPickerApp::load_scene() {
    auto scene = scene_manager_->current_scene();
    auto root_entity = scene->create_root_entity();
    
    auto camera_entity = root_entity->create_child("camera");
    camera_entity->transform_->set_position(10, 10, 10);
    camera_entity->transform_->look_at(Point3F(0, 0, 0));
    main_camera_ = camera_entity->add_component<Camera>();
    
    auto subpass = std::make_unique<ColorPickerSubpass>(*render_context_, scene, main_camera_);
    color_picker_subpass_ = subpass.get();
    color_picker_render_pipeline_ = std::make_unique<RenderPipeline>();
    color_picker_render_pipeline_->add_subpass(std::move(subpass));
    
    // init point light
    auto light = root_entity->create_child("light");
    light->transform_->set_position(0, 3, 0);
    auto point_light = light->add_component<PointLight>();
    point_light->intensity_ = 0.3;
    
    // create box test entity
    float cube_size = 2.0;
    auto box_entity = root_entity->create_child("BoxEntity");
    auto box_mtl = std::make_shared<BlinnPhongMaterial>(*device_);
    auto box_renderer = box_entity->add_component<MeshRenderer>();
    box_mtl->set_base_color(Color(0.8, 0.3, 0.3, 1.0));
    box_renderer->set_mesh(PrimitiveMesh::create_cuboid(cube_size, cube_size, cube_size));
    box_renderer->set_material(box_mtl);
    
    // create sphere test entity
    float radius = 1.25;
    auto sphere_entity = root_entity->create_child("SphereEntity");
    sphere_entity->transform_->set_position(Point3F(-5, 0, 0));
    auto sphere_renderer = sphere_entity->add_component<MeshRenderer>();
    auto sphere_mtl = std::make_shared<BlinnPhongMaterial>(*device_);
    std::default_random_engine e;
    std::uniform_real_distribution<float> u(0, 1);
    sphere_mtl->set_base_color(Color(u(e), u(e), u(e), 1));
    sphere_renderer->set_mesh(PrimitiveMesh::create_sphere(radius));
    sphere_renderer->set_material(sphere_mtl);
}

void FramebufferPickerApp::pick(float offset_x, float offset_y) {
    need_pick_ = true;
    pick_pos_ = Vector2F(offset_x, offset_y);
}

void FramebufferPickerApp::copy_render_target_to_buffer(CommandBuffer &command_buffer) {
    uint32_t client_width = main_camera_->width();
    uint32_t client_height = main_camera_->height();
    uint32_t canvas_width = main_camera_->framebuffer_width();
    uint32_t canvas_height = main_camera_->framebuffer_height();
    
    const float kPx = (pick_pos_.x / static_cast<float>(client_width)) * static_cast<float>(canvas_width);
    const float kPy = (pick_pos_.y / static_cast<float>(client_height)) * static_cast<float>(canvas_height);
    
    const auto kViewport = main_camera_->viewport();
    const auto kViewWidth = (kViewport.z - kViewport.x) * static_cast<float>(canvas_width);
    const auto kViewHeight = (kViewport.w - kViewport.y) * static_cast<float>(canvas_height);
    
    const float kNx = (kPx - kViewport.x) / kViewWidth;
    const float kNy = (kPy - kViewport.y) / kViewHeight;
    const uint32_t kLeft = std::floor(kNx * (canvas_width - 1));
    const uint32_t kBottom = std::floor((1 - kNy) * (canvas_height - 1));
    
    std::vector<VkBufferImageCopy> regions(1);
    regions[0].imageSubresource.layerCount = 1;
    regions[0].imageOffset.x = static_cast<float>(kLeft);
    regions[0].imageOffset.y = canvas_height - kBottom;
    command_buffer
        .copy_image_to_buffer(color_picker_render_target_->get_views().at(0).get_image(), VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                              *stage_buffer_, regions);
}

void FramebufferPickerApp::read_color_from_render_target() {
    uint8_t *raw = stage_buffer_->map();
    if (raw) {
        memcpy(pixel_.data(), raw, 4);
        auto result = color_picker_subpass_->get_object_by_color(pixel_);
        if (result.first) {
            pick_result_ = result;
        }
    }
    stage_buffer_->unmap();
}

}
