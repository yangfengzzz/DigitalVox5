//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "physx_app.h"
#include "mesh/primitive_mesh.h"
#include "mesh/mesh_renderer.h"
#include "material/unlit_material.h"
#include "material/blinn_phong_material.h"
#include "camera.h"
#include "physics/static_collider.h"
#include "physics/dynamic_collider.h"
#include "physics/shape/box_collider_shape.h"
#include "physics/shape/sphere_collider_shape.h"
#include "lighting/point_light.h"
#include "controls/orbit_control.h"
#include <random>

namespace vox {
namespace {
class MoveScript : public Script {
    float pos_ = -5;
    float vel_ = 0.05;
    int8_t vel_sign_ = -1;
    
public:
    explicit MoveScript(Entity *entity) : Script(entity) {
    }
    
    void on_physics_update() override {
        if (pos_ >= 5) {
            vel_sign_ = -1;
        }
        if (pos_ <= -5) {
            vel_sign_ = 1;
        }
        pos_ += vel_ * float(vel_sign_);
        
        entity()->transform_->set_world_position(pos_, 0, 0);
    }
};

// Collision Detection
class CollisionScript : public Script {
    MeshRenderer *sphere_renderer_;
    std::default_random_engine e_;
    std::uniform_real_distribution<float> u_;
    
public:
    explicit CollisionScript(Entity *entity) : Script(entity) {
        sphere_renderer_ = entity->get_component<MeshRenderer>();
        u_ = std::uniform_real_distribution<float>(0, 1);
    }
    
    void on_trigger_exit(const physics::ColliderShapePtr &other) override {
        static_cast<BlinnPhongMaterial *>(sphere_renderer_->get_material().get())->set_base_color(Color(u_(e_), u_(e_), u_(e_), 1));
    }
    
    void on_trigger_enter(const physics::ColliderShapePtr &other) override {
        static_cast<BlinnPhongMaterial *>(sphere_renderer_->get_material().get())->set_base_color(Color(u_(e_), u_(e_), u_(e_), 1));
    }
};
} // namespace

void PhysXApp::load_scene() {
    auto scene = scene_manager_->current_scene();
    scene->ambient_light()->set_diffuse_solid_color(Color(1, 1, 1));
    
    auto root_entity = scene->create_root_entity();
    auto camera_entity = root_entity->create_child("camera");
    camera_entity->transform_->set_position(10, 10, 10);
    camera_entity->transform_->look_at(Point3F(0, 0, 0));
    main_camera_ = camera_entity->add_component<Camera>();
    camera_entity->add_component<control::OrbitControl>();
    
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
    
    auto box_collider = box_entity->add_component<physics::StaticCollider>();
    // boxCollider->debugEntity = boxEntity;
    auto box_collider_shape = std::make_shared<physics::BoxColliderShape>();
    box_collider_shape->set_size(Vector3F(cube_size, cube_size, cube_size));
    box_collider->add_shape(box_collider_shape);
    
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
    
    auto sphere_collider = sphere_entity->add_component<physics::DynamicCollider>();
    // sphereCollider->debugEntity = sphereEntity;
    auto sphere_collider_shape = std::make_shared<physics::SphereColliderShape>();
    sphere_collider_shape->set_radius(radius);
    sphere_collider_shape->set_trigger(true);
    sphere_collider->add_shape(sphere_collider_shape);
    sphere_collider->set_is_kinematic(true);
    
    sphere_entity->add_component<CollisionScript>();
    sphere_entity->add_component<MoveScript>();
    
    scene->play();
}

}
