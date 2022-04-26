//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "shadowmap_app.h"
#include "mesh/primitive_mesh.h"
#include "mesh/mesh_renderer.h"
#include "material/blinn_phong_material.h"
#include "controls/orbit_control.h"
#include "camera.h"
#include "lighting/direct_light.h"
#include "lighting/spot_light.h"

namespace vox {
namespace {
class LightMovement : public Script {
public:
    const float speed_ = 1;
    float total_time_ = 0;
    
    explicit LightMovement(Entity *entity) : Script(entity) {
    }
    
    void on_update(float delta_time) override {
        total_time_ += delta_time;
        total_time_ = fmod(total_time_, 100);
        entity()->transform_->set_position(10 * std::sin(speed_ * total_time_), 10, 10 * std::cos(speed_ * total_time_));
        entity()->transform_->look_at(Point3F(0, 0, 0));
    }
};

class LightMovementReverse : public Script {
public:
    const float speed_ = 1;
    float total_time_ = 0;
    
    explicit LightMovementReverse(Entity *entity) : Script(entity) {
    }
    
    void on_update(float delta_time) override {
        total_time_ += delta_time;
        total_time_ = fmod(total_time_, 100);
        entity()->transform_->set_position(10 * std::cos(speed_ * total_time_), 10, 10 * std::sin(speed_ * total_time_));
        entity()->transform_->look_at(Point3F(0, 0, 0));
    }
};
}

void ShadowMapApp::load_scene() {
    auto scene = scene_manager_->current_scene();
    auto root_entity = scene->create_root_entity();
    
    auto camera_entity = root_entity->create_child();
    camera_entity->transform_->set_position(10, 10, 10);
    camera_entity->transform_->look_at(Point3F(0, 0, 0));
    main_camera_ = camera_entity->add_component<Camera>();
    camera_entity->add_component<control::OrbitControl>();
    
    // init point light
    auto light = root_entity->create_child("light");
    light->add_component<LightMovement>();
    auto spot_light = light->add_component<SpotLight>();
    spot_light->intensity_ = 0.2;
    spot_light->distance_ = 100;
//    spot_light->set_enable_shadow(true);
    
    auto light3 = root_entity->create_child("light3");
    light3->add_component<LightMovementReverse>();
    auto direct_light = light3->add_component<DirectLight>();
    direct_light->intensity_ = 0.2;
//    direct_light->set_enable_shadow(true);
    
    // create box test entity
    float cube_size = 2.0;
    auto box_entity = root_entity->create_child("BoxEntity");
    box_entity->transform_->set_position(Point3F(0, 2, 0));
    auto box_mtl = std::make_shared<BlinnPhongMaterial>(*device_);
    box_mtl->set_base_color(Color(0.3, 0.3, 0.3, 0.5));
    
    auto box_renderer = box_entity->add_component<MeshRenderer>();
    box_renderer->set_mesh(PrimitiveMesh::create_cuboid(cube_size, cube_size, cube_size));
    box_renderer->set_material(box_mtl);
    box_renderer->cast_shadow_ = true;
    
    auto plane_entity = root_entity->create_child("PlaneEntity");
    auto plane_mtl = std::make_shared<BlinnPhongMaterial>(*device_);
    plane_mtl->set_base_color(Color(1.0, 0, 0, 1.0));
    plane_mtl->set_render_face(RenderFace::DOUBLE);
    
    auto plane_renderer = plane_entity->add_component<MeshRenderer>();
    plane_renderer->set_mesh(PrimitiveMesh::create_plane(10, 10));
    plane_renderer->set_material(plane_mtl);
    plane_renderer->receive_shadow_ = true;
    
    scene->play();
}

}
