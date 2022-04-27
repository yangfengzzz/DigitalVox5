//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "cluster_forward_app.h"
#include "mesh/primitive_mesh.h"
#include "mesh/mesh_renderer.h"
#include "camera.h"
#include "controls/orbit_control.h"
#include "lighting/point_light.h"
#include "lighting/debug/cluster_debug_material.h"
#include "lighting/debug/sprite_debug.h"
#include <random>

namespace vox {
void ClusterForwardApp::load_scene() {
    auto scene = scene_manager_->current_scene();
    scene->ambient_light()->set_diffuse_solid_color(Color(1, 1, 1));
    
    auto root_entity = scene->create_root_entity();
    root_entity->add_component<SpriteDebug>();
    
    auto camera_entity = root_entity->create_child("camera");
    camera_entity->transform_->set_position(10, 10, 10);
    camera_entity->transform_->look_at(Point3F(0, 0, 0));
    main_camera_ = camera_entity->add_component<Camera>();
    camera_entity->add_component<control::OrbitControl>();
    
    // init point light
    auto light = root_entity->create_child("light");
    light->transform_->set_position(3, 3, 0);
    light->add_component<PointLight>();
    
    auto light2 = root_entity->create_child("light");
    light2->transform_->set_position(-3, 3, 0);
    light2->add_component<SpotLight>();
    
    // create box test entity
    float cube_size = 20.0;
    auto box_entity = root_entity->create_child("BoxEntity");
    auto box_mtl = std::make_shared<ClusterDebugMaterial>(*device_);
    auto box_renderer = box_entity->add_component<MeshRenderer>();
    box_renderer->set_mesh(PrimitiveMesh::create_plane(cube_size, cube_size));
    box_renderer->set_material(box_mtl);
    
    scene->play();
}

}
