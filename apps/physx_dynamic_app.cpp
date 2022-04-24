//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "physx_dynamic_app.h"
#include "mesh/primitive_mesh.h"
#include "mesh/mesh_renderer.h"
#include "material/blinn_phong_material.h"
#include "camera.h"
#include "physics/static_collider.h"
#include "physics/dynamic_collider.h"
#include "physics/character_controller/capsule_character_controller.h"
#include "physics/joint/fixed_joint.h"
#include "physics/shape/box_collider_shape.h"
#include "physics/shape/sphere_collider_shape.h"
#include "physics/shape/plane_collider_shape.h"
#include "physics/shape/capsule_collider_shape.h"
#include "lighting/point_light.h"
#include "lighting/direct_light.h"
#include "controls/orbit_control.h"
#include "physics/physics_manager.h"

namespace vox {
namespace {
class ControllerScript : public Script {
private:
    Entity *camera_{nullptr};
    physics::CharacterController *character_ = nullptr;
    Vector3F displacement_ = Vector3F();
    
public:
    explicit ControllerScript(Entity *entity) : Script(entity) {
        character_ = entity->get_component<physics::CapsuleCharacterController>();
    }
    
    void target_camera(Entity *camera) {
        camera_ = camera;
    }
    
    void input_event(const InputEvent &input_event) override {
        if (input_event.get_source() == EventSource::KEYBOARD) {
            const auto &key_event = static_cast<const KeyInputEvent &>(input_event);
            
            Vector3F forward = entity()->transform_->position() - camera_->transform_->position();
            forward.y = 0;
            forward.normalize();
            Vector3F cross = Vector3F(forward.z, 0, -forward.x);
            
            switch (key_event.get_code()) {
                case KeyCode::W:displacement_ = forward * 0.3f;
                    break;
                case KeyCode::S:displacement_ = -forward * 0.3f;
                    break;
                case KeyCode::A:displacement_ = cross * 0.3f;
                    break;
                case KeyCode::D:displacement_ = -cross * 0.3f;
                    break;
                case KeyCode::SPACE:displacement_.x = 0;
                    displacement_.y = 2;
                    displacement_.z = 0;
                    break;
                default:break;
            }
        }
    }
    
    void on_physics_update() override {
        auto flags = character_->move(displacement_, 0.1, physics::PhysicsManager::fixed_time_step_);
        displacement_ = Vector3F();
        if (!flags.isSet(physx::PxControllerCollisionFlag::Enum::eCOLLISION_DOWN)) {
            character_->move(Vector3F(0, -0.2, 0), 0.1, physics::PhysicsManager::fixed_time_step_);
        }
    }
};

}

void PhysXDynamicApp::load_scene() {
    u = std::uniform_real_distribution<float>(0, 1);
    auto scene = scene_manager_->current_scene();
    scene->ambient_light()->set_diffuse_solid_color(Color(1, 1, 1));
    
    root_entity_ = scene->create_root_entity();
    auto camera_entity = root_entity_->create_child("camera");
    camera_entity->transform_->set_position(20, 20, 20);
    camera_entity->transform_->look_at(Point3F(0, 0, 0));
    main_camera_ = camera_entity->add_component<Camera>();
    camera_entity->add_component<control::OrbitControl>();
    
    auto add_plane = [&](const Vector3F &size, const Point3F &position, const QuaternionF &rotation) {
        auto mtl = std::make_shared<BlinnPhongMaterial>(*device_);
        mtl->set_base_color(Color(0.03179807202597362, 0.3939682161541871, 0.41177952549087604, 1.0));
        auto plane_entity = root_entity_->create_child();
        plane_entity->layer_ = Layer::LAYER_1;
        
        auto renderer = plane_entity->add_component<MeshRenderer>();
        renderer->receive_shadow_ = true;
        renderer->set_mesh(PrimitiveMesh::create_cuboid(size.x, size.y, size.z));
        renderer->set_material(mtl);
        plane_entity->transform_->set_position(position);
        plane_entity->transform_->set_rotation_quaternion(rotation);
        
        auto physics_plane = std::make_shared<physics::PlaneColliderShape>();
        auto plane_collider = plane_entity->add_component<physics::StaticCollider>();
        plane_collider->add_shape(physics_plane);
        
        return plane_entity;
    };
    
    auto add_box = [&](const Vector3F &size, const Point3F &position, const QuaternionF &rotation) {
        auto box_mtl = std::make_shared<BlinnPhongMaterial>(*device_);
        box_mtl->set_base_color(Color(u(e), u(e), u(e), 1.0));
        auto box_entity = root_entity_->create_child("BoxEntity");
        auto box_renderer = box_entity->add_component<MeshRenderer>();
        box_renderer->cast_shadow_ = true;
        box_renderer->set_mesh(PrimitiveMesh::create_cuboid(size.x, size.y, size.z));
        box_renderer->set_material(box_mtl);
        box_entity->transform_->set_position(position);
        box_entity->transform_->set_rotation_quaternion(rotation);
        
        auto physics_box = std::make_shared<physics::BoxColliderShape>();
        physics_box->set_size(Vector3F(size.x, size.y, size.z));
        physics_box->material()->setStaticFriction(1);
        physics_box->material()->setDynamicFriction(2);
        physics_box->material()->setRestitution(0.1);
        physics_box->set_trigger(false);
        
        auto box_collider = box_entity->add_component<physics::DynamicCollider>();
        box_collider->add_shape(physics_box);
        
        return box_entity;
    };
    
    auto add_player = [&](float radius, float height, const Point3F &position, const QuaternionF &rotation) {
        auto mtl = std::make_shared<BlinnPhongMaterial>(*device_);
        mtl->set_base_color(Color(u(e), u(e), u(e), 1.0));
        auto capsule_entity = root_entity_->create_child();
        auto renderer = capsule_entity->add_component<MeshRenderer>();
        renderer->cast_shadow_ = true;
        renderer->set_mesh(PrimitiveMesh::create_capsule(radius, height, 20));
        renderer->set_material(mtl);
        capsule_entity->transform_->set_position(position);
        capsule_entity->transform_->set_rotation_quaternion(rotation);
        
        auto character_controller = capsule_entity->add_component<physics::CapsuleCharacterController>();
        physx::PxCapsuleControllerDesc character_controller_desc;
        character_controller_desc.radius = radius;
        character_controller_desc.height = height;
        character_controller_desc.material = physics::PhysicsManager::native_physics_()->createMaterial(0, 0, 0);
        auto world_pos = capsule_entity->transform_->world_position();
        character_controller_desc.position = physx::PxExtendedVec3(world_pos.x, world_pos.y, world_pos.z);
        character_controller->set_desc(character_controller_desc);
        
        return capsule_entity;
    };
    
    auto transform = [&](const Point3F &position, const QuaternionF &rotation,
                         Point3F &out_position, QuaternionF &out_rotation) {
        out_rotation = rotation * out_rotation;
        out_position = rotation * out_position;
        out_position = out_position + Vector3F(position.x, position.y, position.z);
    };
    
    auto create_chain = [&](const Point3F &position, const QuaternionF &rotation, size_t length, float separation) {
        auto offset = Vector3F(0, -separation / 2, 0);
        physics::DynamicCollider *prev_collider = nullptr;
        for (size_t i = 0; i < length; i++) {
            auto local_tm_pos = Point3F(0, -separation / 2 * (2 * float(i) + 1), 0);
            auto local_tm_quat = QuaternionF();
            transform(position, rotation, local_tm_pos, local_tm_quat);
            
            auto current_entity = add_box(Vector3F(2.0, 2.0, 0.5), local_tm_pos, local_tm_quat);
            auto current_collider = current_entity->get_component<physics::DynamicCollider>();
            
            auto joint = physics::FixedJoint(prev_collider, current_collider);
            Transform3F local_pose;
            local_pose.setTranslation(prev_collider != nullptr ? offset : Vector3F(position.x, position.y, position.z));
            local_pose.setOrientation(prev_collider != nullptr ? QuaternionF() : rotation);
            joint.set_local_pose(physx::PxJointActorIndex::Enum::eACTOR0, local_pose);
            local_pose.setTranslation(Vector3F(0, separation / 2, 0));
            local_pose.setOrientation(QuaternionF());
            joint.set_local_pose(physx::PxJointActorIndex::Enum::eACTOR1, local_pose);
            prev_collider = current_collider;
        }
    };
    
    auto light = root_entity_->create_child("light");
    light->transform_->set_position(10, 10, 0);
    light->transform_->look_at(Point3F());
    auto direct_light = light->add_component<DirectLight>();
    direct_light->intensity_ = 0.5;
    direct_light->set_enable_shadow(true);
    
    auto player = add_player(1, 3, Point3F(0, 6.5, 0), QuaternionF());
    auto controller = player->add_component<ControllerScript>();
    controller->target_camera(camera_entity);
    
    add_plane(Vector3F(30, 0.1, 30), Point3F(), QuaternionF());
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            add_box(Vector3F(1, 1, 1),
                    Point3F(-2.5f + static_cast<float>(i) + 0.1f * static_cast<float>(i),
                            u(e) * 6.f + 1.f, -2.5f + static_cast<float>(j) + 0.1f * static_cast<float>(j)),
                    QuaternionF(0, 0, 0.3, 0.7));
        }
    }
    create_chain(Point3F(0.0, 25.0, -10.0), QuaternionF(), 10, 2.0);
    
    scene->play();
}

void PhysXDynamicApp::input_event(const InputEvent &input_event) {
    ForwardApplication::input_event(input_event);
    
    if (input_event.get_source() == EventSource::MOUSE) {
        const auto &mouse_button = static_cast<const MouseButtonInputEvent &>(input_event);
        if (mouse_button.get_action() == MouseAction::DOWN) {
            Ray ray = main_camera_->screen_point_to_ray(Vector2F(mouse_button.get_pos_x(), mouse_button.get_pos_y()));
            
            physics::HitResult hit;
            auto result = physics::PhysicsManager::get_singleton().raycast(ray, std::numeric_limits<float>::max(),
                                                                           Layer::LAYER_0, hit);
            if (result) {
                auto mtl = std::make_shared<BlinnPhongMaterial>(*device_);
                mtl->set_base_color(Color(u(e), u(e), u(e), 1));
                
                auto meshes = hit.entity->get_components_include_children<MeshRenderer>();
                for (auto &mesh : meshes) {
                    mesh->set_material(mtl);
                }
            }
        }
    } else if (input_event.get_source() == EventSource::KEYBOARD) {
        const auto &key_event = static_cast<const KeyInputEvent &>(input_event);
        if (key_event.get_action() == KeyAction::UP) {
            Vector3F dir = main_camera_->entity()->transform_->world_forward();
            dir = dir * 50.0f;
            
            switch (key_event.get_code()) {
                case KeyCode::ENTER:
                    add_sphere(0.5, main_camera_->entity()->transform_->position(),
                               main_camera_->entity()->transform_->rotation_quaternion(), dir);
                    break;
                default:break;
            }
        }
    }
}

Entity *PhysXDynamicApp::add_sphere(float radius, const Point3F &position,
                                    const QuaternionF &rotation, const Vector3F &velocity) {
    auto mtl = std::make_shared<BlinnPhongMaterial>(*device_);
    mtl->set_base_color(Color(u(e), u(e), u(e), 1.0));
    auto sphere_entity = root_entity_->create_child();
    auto renderer = sphere_entity->add_component<MeshRenderer>();
    renderer->cast_shadow_ = true;
    renderer->set_mesh(PrimitiveMesh::create_sphere(radius));
    renderer->set_material(mtl);
    sphere_entity->transform_->set_position(position);
    sphere_entity->transform_->set_rotation_quaternion(rotation);
    
    auto physics_sphere = std::make_shared<physics::SphereColliderShape>();
    physics_sphere->set_radius(radius);
    physics_sphere->material()->setStaticFriction(0.1);
    physics_sphere->material()->setDynamicFriction(0.2);
    physics_sphere->material()->setRestitution(1);
    physics_sphere->material()->setRestitutionCombineMode(physx::PxCombineMode::Enum::eMIN);
    
    auto sphere_collider = sphere_entity->add_component<physics::DynamicCollider>();
    sphere_collider->add_shape(physics_sphere);
    sphere_collider->set_linear_velocity(velocity);
    sphere_collider->set_angular_damping(0.5);
    
    return sphere_entity;
}

Entity *PhysXDynamicApp::add_capsule(float radius, float height,
                                     const Point3F &position, const QuaternionF &rotation) {
    auto mtl = std::make_shared<BlinnPhongMaterial>(*device_);
    mtl->set_base_color(Color(u(e), u(e), u(e), 1.0));
    auto capsule_entity = root_entity_->create_child();
    auto renderer = capsule_entity->add_component<MeshRenderer>();
    renderer->cast_shadow_ = true;
    renderer->set_mesh(PrimitiveMesh::create_capsule(radius, height));
    renderer->set_material(mtl);
    capsule_entity->transform_->set_position(position);
    capsule_entity->transform_->set_rotation_quaternion(rotation);
    
    auto physics_capsule = std::make_shared<physics::CapsuleColliderShape>();
    physics_capsule->set_radius(radius);
    physics_capsule->set_height(height);
    
    auto capsule_collider = capsule_entity->add_component<physics::DynamicCollider>();
    capsule_collider->add_shape(physics_capsule);
    
    return capsule_entity;
}

}