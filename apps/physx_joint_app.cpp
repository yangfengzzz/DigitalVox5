//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "apps/physx_joint_app.h"

#include "vox.render/camera.h"
#include "vox.render/controls/orbit_control.h"
#include "vox.render/lighting/direct_light.h"
#include "vox.render/lighting/point_light.h"
#include "vox.render/material/blinn_phong_material.h"
#include "vox.render/mesh/mesh_renderer.h"
#include "vox.render/mesh/primitive_mesh.h"
#include "vox.render/physics/character_controller/capsule_character_controller.h"
#include "vox.render/physics/dynamic_collider.h"
#include "vox.render/physics/joint/fixed_joint.h"
#include "vox.render/physics/physics_manager.h"
#include "vox.render/physics/shape/box_collider_shape.h"
#include "vox.render/physics/shape/capsule_collider_shape.h"
#include "vox.render/physics/shape/plane_collider_shape.h"
#include "vox.render/physics/shape/sphere_collider_shape.h"
#include "vox.render/physics/static_collider.h"

namespace vox {

void PhysXJointApp::LoadScene() {
    u = std::uniform_real_distribution<float>(0, 1);
    auto scene = scene_manager_->CurrentScene();
    scene->AmbientLight()->SetDiffuseSolidColor(Color(1, 1, 1));

    root_entity_ = scene->CreateRootEntity();
    auto camera_entity = root_entity_->CreateChild("camera");
    camera_entity->transform->SetPosition(20, 20, 20);
    camera_entity->transform->LookAt(Point3F(0, 0, 0));
    main_camera_ = camera_entity->AddComponent<Camera>();
    camera_entity->AddComponent<control::OrbitControl>();

    auto add_plane = [&](const Vector3F &size, const Point3F &position, const QuaternionF &rotation) {
        auto mtl = std::make_shared<BlinnPhongMaterial>(*device_);
        mtl->SetBaseColor(Color(0.03179807202597362, 0.3939682161541871, 0.41177952549087604, 1.0));
        auto plane_entity = root_entity_->CreateChild();
        plane_entity->layer = Layer::LAYER_1;

        auto renderer = plane_entity->AddComponent<MeshRenderer>();
        renderer->receive_shadow_ = true;
        renderer->SetMesh(PrimitiveMesh::CreateCuboid(size.x, size.y, size.z));
        renderer->SetMaterial(mtl);
        plane_entity->transform->SetPosition(position);
        plane_entity->transform->SetRotationQuaternion(rotation);

        auto physics_plane = std::make_shared<physics::PlaneColliderShape>();
        auto plane_collider = plane_entity->AddComponent<physics::StaticCollider>();
        plane_collider->AddShape(physics_plane);

        return plane_entity;
    };

    auto add_box = [&](const Vector3F &size, const Point3F &position, const QuaternionF &rotation) {
        auto box_mtl = std::make_shared<BlinnPhongMaterial>(*device_);
        box_mtl->SetBaseColor(Color(u(e), u(e), u(e), 1.0));
        auto box_entity = root_entity_->CreateChild("BoxEntity");
        auto box_renderer = box_entity->AddComponent<MeshRenderer>();
        box_renderer->cast_shadow_ = true;
        box_renderer->SetMesh(PrimitiveMesh::CreateCuboid(size.x, size.y, size.z));
        box_renderer->SetMaterial(box_mtl);
        box_entity->transform->SetPosition(position);
        box_entity->transform->SetRotationQuaternion(rotation);

        auto physics_box = std::make_shared<physics::BoxColliderShape>();
        physics_box->SetSize(Vector3F(size.x, size.y, size.z));
        physics_box->Material()->setStaticFriction(1);
        physics_box->Material()->setDynamicFriction(2);
        physics_box->Material()->setRestitution(0.1);
        physics_box->SetTrigger(false);

        auto box_collider = box_entity->AddComponent<physics::DynamicCollider>();
        box_collider->AddShape(physics_box);

        return box_entity;
    };

    auto transform = [&](const Point3F &position, const QuaternionF &rotation, Point3F &out_position,
                         QuaternionF &out_rotation) {
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
            auto current_collider = current_entity->GetComponent<physics::DynamicCollider>();

            auto joint = physics::FixedJoint(prev_collider, current_collider);
            Transform3F local_pose;
            local_pose.setTranslation(prev_collider != nullptr ? offset : Vector3F(position.x, position.y, position.z));
            local_pose.setOrientation(prev_collider != nullptr ? QuaternionF() : rotation);
            joint.SetLocalPose(physx::PxJointActorIndex::Enum::eACTOR0, local_pose);
            local_pose.setTranslation(Vector3F(0, separation / 2, 0));
            local_pose.setOrientation(QuaternionF());
            joint.SetLocalPose(physx::PxJointActorIndex::Enum::eACTOR1, local_pose);
            prev_collider = current_collider;
        }
    };

    auto light = root_entity_->CreateChild("light");
    light->transform->SetPosition(10, 10, 0);
    light->transform->LookAt(Point3F());
    auto direct_light = light->AddComponent<DirectLight>();
    direct_light->intensity_ = 0.5;
    direct_light->SetEnableShadow(true);

    add_plane(Vector3F(30, 0.1, 30), Point3F(), QuaternionF());
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            add_box(Vector3F(1, 1, 1),
                    Point3F(-2.5f + static_cast<float>(i) + 0.1f * static_cast<float>(i), u(e) * 6.f + 1.f,
                            -2.5f + static_cast<float>(j) + 0.1f * static_cast<float>(j)),
                    QuaternionF(0, 0, 0.3, 0.7));
        }
    }
    create_chain(Point3F(0.0, 25.0, -10.0), QuaternionF(), 10, 2.0);

    scene->Play();
}

void PhysXJointApp::InputEvent(const vox::InputEvent &input_event) {
    ForwardApplication::InputEvent(input_event);

    if (input_event.GetSource() == EventSource::MOUSE) {
        const auto &mouse_button = static_cast<const MouseButtonInputEvent &>(input_event);
        if (mouse_button.GetAction() == MouseAction::DOWN) {
            Ray ray = main_camera_->ScreenPointToRay(Vector2F(mouse_button.GetPosX(), mouse_button.GetPosY()));

            physics::HitResult hit;
            auto result = physics::PhysicsManager::GetSingleton().Raycast(ray, std::numeric_limits<float>::max(),
                                                                          Layer::LAYER_0, hit);
            if (result) {
                auto mtl = std::make_shared<BlinnPhongMaterial>(*device_);
                mtl->SetBaseColor(Color(u(e), u(e), u(e), 1));

                auto meshes = hit.entity->GetComponentsIncludeChildren<MeshRenderer>();
                for (auto &mesh : meshes) {
                    mesh->SetMaterial(mtl);
                }
            }
        }
    } else if (input_event.GetSource() == EventSource::KEYBOARD) {
        const auto &key_event = static_cast<const KeyInputEvent &>(input_event);
        if (key_event.GetAction() == KeyAction::UP) {
            Vector3F dir = main_camera_->GetEntity()->transform->WorldForward();
            dir = dir * 50.0f;

            switch (key_event.GetCode()) {
                case KeyCode::ENTER:
                    AddSphere(0.5, main_camera_->GetEntity()->transform->Position(),
                              main_camera_->GetEntity()->transform->RotationQuaternion(), dir);
                    break;
                default:
                    break;
            }
        }
    }
}

Entity *PhysXJointApp::AddSphere(float radius,
                                   const Point3F &position,
                                   const QuaternionF &rotation,
                                   const Vector3F &velocity) {
    auto mtl = std::make_shared<BlinnPhongMaterial>(*device_);
    mtl->SetBaseColor(Color(u(e), u(e), u(e), 1.0));
    auto sphere_entity = root_entity_->CreateChild();
    auto renderer = sphere_entity->AddComponent<MeshRenderer>();
    renderer->cast_shadow_ = true;
    renderer->SetMesh(PrimitiveMesh::CreateSphere(radius));
    renderer->SetMaterial(mtl);
    sphere_entity->transform->SetPosition(position);
    sphere_entity->transform->SetRotationQuaternion(rotation);

    auto physics_sphere = std::make_shared<physics::SphereColliderShape>();
    physics_sphere->SetRadius(radius);
    physics_sphere->Material()->setStaticFriction(0.1);
    physics_sphere->Material()->setDynamicFriction(0.2);
    physics_sphere->Material()->setRestitution(1);
    physics_sphere->Material()->setRestitutionCombineMode(physx::PxCombineMode::Enum::eMIN);

    auto sphere_collider = sphere_entity->AddComponent<physics::DynamicCollider>();
    sphere_collider->AddShape(physics_sphere);
    sphere_collider->SetLinearVelocity(velocity);
    sphere_collider->SetAngularDamping(0.5);

    return sphere_entity;
}

Entity *PhysXJointApp::AddCapsule(float radius, float height, const Point3F &position, const QuaternionF &rotation) {
    auto mtl = std::make_shared<BlinnPhongMaterial>(*device_);
    mtl->SetBaseColor(Color(u(e), u(e), u(e), 1.0));
    auto capsule_entity = root_entity_->CreateChild();
    auto renderer = capsule_entity->AddComponent<MeshRenderer>();
    renderer->cast_shadow_ = true;
    renderer->SetMesh(PrimitiveMesh::CreateCapsule(radius, height));
    renderer->SetMaterial(mtl);
    capsule_entity->transform->SetPosition(position);
    capsule_entity->transform->SetRotationQuaternion(rotation);

    auto physics_capsule = std::make_shared<physics::CapsuleColliderShape>();
    physics_capsule->SetRadius(radius);
    physics_capsule->SetHeight(height);

    auto capsule_collider = capsule_entity->AddComponent<physics::DynamicCollider>();
    capsule_collider->AddShape(physics_capsule);

    return capsule_entity;
}

}  // namespace vox
