//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "apps/physx_app.h"

#include <random>

#include "vox.render/camera.h"
#include "vox.render/controls/orbit_control.h"
#include "vox.render/lighting/point_light.h"
#include "vox.render/material/blinn_phong_material.h"
#include "vox.render/material/unlit_material.h"
#include "vox.render/mesh/mesh_renderer.h"
#include "vox.render/mesh/primitive_mesh.h"
#include "vox.render/physics/dynamic_collider.h"
#include "vox.render/physics/shape/box_collider_shape.h"
#include "vox.render/physics/shape/sphere_collider_shape.h"
#include "vox.render/physics/static_collider.h"

namespace vox {
namespace {
class MoveScript : public Script {
    float pos_ = -5;
    float vel_ = 0.05;
    int8_t vel_sign_ = -1;

public:
    explicit MoveScript(Entity *entity) : Script(entity) {}

    void OnPhysicsUpdate() override {
        if (pos_ >= 5) {
            vel_sign_ = -1;
        }
        if (pos_ <= -5) {
            vel_sign_ = 1;
        }
        pos_ += vel_ * float(vel_sign_);

        GetEntity()->transform->SetWorldPosition(pos_, 0, 0);
    }
};

// Collision Detection
class CollisionScript : public Script {
    MeshRenderer *sphere_renderer_;
    std::default_random_engine e_;
    std::uniform_real_distribution<float> u_;

public:
    explicit CollisionScript(Entity *entity) : Script(entity) {
        sphere_renderer_ = entity->GetComponent<MeshRenderer>();
        u_ = std::uniform_real_distribution<float>(0, 1);
    }

    void OnTriggerExit(const physics::ColliderShapePtr &other) override {
        static_cast<BlinnPhongMaterial *>(sphere_renderer_->GetMaterial().get())
                ->SetBaseColor(Color(u_(e_), u_(e_), u_(e_), 1));
    }

    void OnTriggerEnter(const physics::ColliderShapePtr &other) override {
        static_cast<BlinnPhongMaterial *>(sphere_renderer_->GetMaterial().get())
                ->SetBaseColor(Color(u_(e_), u_(e_), u_(e_), 1));
    }
};
}  // namespace

void PhysXApp::LoadScene() {
    auto scene = scene_manager_->CurrentScene();
    scene->AmbientLight()->SetDiffuseSolidColor(Color(1, 1, 1));

    auto root_entity = scene->CreateRootEntity();
    auto camera_entity = root_entity->CreateChild("camera");
    camera_entity->transform->SetPosition(10, 10, 10);
    camera_entity->transform->LookAt(Point3F(0, 0, 0));
    main_camera_ = camera_entity->AddComponent<Camera>();
    camera_entity->AddComponent<control::OrbitControl>();

    // init point light
    auto light = root_entity->CreateChild("light");
    light->transform->SetPosition(0, 3, 0);
    auto point_light = light->AddComponent<PointLight>();
    point_light->intensity_ = 0.3;

    // create box test entity
    float cube_size = 2.0;
    auto box_entity = root_entity->CreateChild("BoxEntity");
    auto box_mtl = std::make_shared<BlinnPhongMaterial>(*device_);
    auto box_renderer = box_entity->AddComponent<MeshRenderer>();
    box_mtl->SetBaseColor(Color(0.8, 0.3, 0.3, 1.0));
    box_renderer->SetMesh(PrimitiveMesh::CreateCuboid(cube_size, cube_size, cube_size));
    box_renderer->SetMaterial(box_mtl);

    auto box_collider = box_entity->AddComponent<physics::StaticCollider>();
    // boxCollider->debugEntity = boxEntity;
    auto box_collider_shape = std::make_shared<physics::BoxColliderShape>();
    box_collider_shape->SetSize(Vector3F(cube_size, cube_size, cube_size));
    box_collider->AddShape(box_collider_shape);

    // create sphere test entity
    float radius = 1.25;
    auto sphere_entity = root_entity->CreateChild("SphereEntity");
    sphere_entity->transform->SetPosition(Point3F(-5, 0, 0));
    auto sphere_renderer = sphere_entity->AddComponent<MeshRenderer>();
    auto sphere_mtl = std::make_shared<BlinnPhongMaterial>(*device_);
    std::default_random_engine e;
    std::uniform_real_distribution<float> u(0, 1);
    sphere_mtl->SetBaseColor(Color(u(e), u(e), u(e), 1));
    sphere_renderer->SetMesh(PrimitiveMesh::CreateSphere(radius));
    sphere_renderer->SetMaterial(sphere_mtl);

    auto sphere_collider = sphere_entity->AddComponent<physics::DynamicCollider>();
    // sphereCollider->debugEntity = sphereEntity;
    auto sphere_collider_shape = std::make_shared<physics::SphereColliderShape>();
    sphere_collider_shape->SetRadius(radius);
    sphere_collider_shape->SetTrigger(true);
    sphere_collider->AddShape(sphere_collider_shape);
    sphere_collider->SetIsKinematic(true);

    sphere_entity->AddComponent<CollisionScript>();
    sphere_entity->AddComponent<MoveScript>();

    scene->Play();
}

}  // namespace vox
