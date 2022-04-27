//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "multi_light_app.h"
#include "mesh/primitive_mesh.h"
#include "mesh/mesh_renderer.h"
#include "camera.h"
#include "controls/orbit_control.h"
#include "lighting/point_light.h"
#include "lighting/debug/sprite_debug.h"
#include "material/blinn_phong_material.h"

#include <random>

namespace vox {
namespace {
class MoveScript : public Script {
    std::random_device rd_;
    std::mt19937 gen_;
    std::uniform_real_distribution<float> dis_;
    
    Point3F pos_;
    float vel_;
    int8_t vel_sign_ = -1;
    
public:
    explicit MoveScript(Entity *entity) :
    Script(entity),
    gen_(rd_()) {
        dis_ = std::uniform_real_distribution<float>(-1.0, 1.0);
        pos_ = Point3F(10 * dis_(gen_), 0, 10 * dis_(gen_));
        vel_ = std::abs(dis_(gen_) * 4);
    }
    
    void on_update(float delta_time) override {
        if (pos_.y >= 5) {
            vel_sign_ = -1;
        }
        if (pos_.y <= -5) {
            vel_sign_ = 1;
        }
        pos_.y += delta_time * vel_ * float(vel_sign_);
        
        entity()->transform_->set_position(pos_);
        entity()->transform_->look_at(Point3F(0, 0, 0));
    }
};

}

void MultiLightApp::load_scene() {
    auto scene = scene_manager_->current_scene();
    scene->ambient_light()->set_diffuse_solid_color(Color(1, 1, 1));
    
    auto root_entity = scene->create_root_entity();
    root_entity->add_component<SpriteDebug>();
    
    auto camera_entity = root_entity->create_child("camera");
    camera_entity->transform_->set_position(10, 10, 10);
    camera_entity->transform_->look_at(Point3F(0, 0, 0));
    main_camera_ = camera_entity->add_component<Camera>();
    camera_entity->add_component<control::OrbitControl>();
    
    std::default_random_engine e;
    std::uniform_real_distribution<float> u(0, 1);
    // init point light
    for (uint32_t i = 0; i < 50; i++) {
        auto light = root_entity->create_child("light");
        light->add_component<MoveScript>();
        auto point_light = light->add_component<PointLight>();
        point_light->color_ = Color(u(e), u(e), u(e), 1);
    }
    
    // init spot light
    for (uint32_t i = 0; i < 50; i++) {
        auto light = root_entity->create_child("light");
        light->add_component<MoveScript>();
        auto spot_light = light->add_component<SpotLight>();
        spot_light->color_ = Color(u(e), u(e), u(e), 1);
    }
    
    // create box test entity
    float cube_size = 20.0;
    auto box_entity = root_entity->create_child("BoxEntity");
    auto box_mtl = std::make_shared<BlinnPhongMaterial>(*device_);
    box_mtl->set_base_color(Color(0.0, 0.6, 0.3, 1.0));
    auto box_renderer = box_entity->add_component<MeshRenderer>();
    box_renderer->set_mesh(PrimitiveMesh::create_plane(cube_size, cube_size, 100, 1000));
    box_renderer->set_material(box_mtl);
    
    scene->play();
}

}
