//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "skybox_app.h"
#include "mesh/primitive_mesh.h"
#include "mesh/mesh_renderer.h"
#include "material/unlit_material.h"
#include "rendering/subpasses/skybox_subpass.h"
#include "camera.h"
#include "image_manager.h"

namespace vox {
bool SkyboxApp::prepare(Platform &platform) {
    ForwardApplication::prepare(platform);
    
    auto scene = scene_manager_->current_scene();
    auto skybox = std::make_unique<SkyboxSubpass>(*render_context_,
                                                  scene, main_camera_);
    skybox->create_cuboid();
    skybox->flip_vertically();
    skybox->set_texture_cube_map(ImageManager::get_singleton().load_texture_cubemap("Textures/uffizi_rgba16f_cube.ktx"));
    render_pipeline_->add_subpass(std::move(skybox));
    
    return true;
}

void SkyboxApp::load_scene() {
    auto scene = scene_manager_->current_scene();
    auto root_entity = scene->create_root_entity();
    
    auto camera_entity = root_entity->create_child();
    camera_entity->transform_->set_position(10, 10, 10);
    camera_entity->transform_->look_at(Point3F(0, 0, 0));
    main_camera_ = camera_entity->add_component<Camera>();
    camera_entity->add_component<control::OrbitControl>();
    
    auto model_entity = root_entity->create_child();
    auto renderer = model_entity->add_component<MeshRenderer>();
    renderer->set_mesh(PrimitiveMesh::create_cuboid(*device_));
    auto material = std::make_shared<UnlitMaterial>(*device_);
    material->set_base_color(Color(0.6, 0.4, 0.7, 1.0));
    renderer->set_material(material);
    
    scene->play();
}

}
