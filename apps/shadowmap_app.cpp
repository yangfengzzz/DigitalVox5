//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "apps/shadowmap_app.h"

#include <unistd.h>

#include "vox.render/camera.h"
#include "vox.render/controls/orbit_control.h"
#include "vox.render/lighting/direct_light.h"
#include "vox.render/lighting/spot_light.h"
#include "vox.render/material/blinn_phong_material.h"
#include "vox.render/mesh/mesh_renderer.h"
#include "vox.render/mesh/primitive_mesh.h"

namespace vox {
namespace {
class LightMovement : public Script {
public:
    const float speed_ = 1;
    float total_time_ = 0;

    explicit LightMovement(Entity *entity) : Script(entity) {}

    void OnUpdate(float delta_time) override {
        total_time_ += delta_time;
        total_time_ = fmod(total_time_, 100);
        GetEntity()->transform->SetPosition(10 * std::sin(speed_ * total_time_), 10,
                                            10 * std::cos(speed_ * total_time_));
        GetEntity()->transform->LookAt(Point3F(0, 0, 0));
    }
};

class LightMovementReverse : public Script {
public:
    const float speed_ = 1;
    float total_time_ = 0;

    explicit LightMovementReverse(Entity *entity) : Script(entity) {}

    void OnUpdate(float delta_time) override {
        total_time_ += delta_time;
        total_time_ = fmod(total_time_, 100);
        GetEntity()->transform->SetPosition(10 * std::cos(speed_ * total_time_), 10,
                                            10 * std::sin(speed_ * total_time_));
        GetEntity()->transform->LookAt(Point3F(0, 0, 0));
    }
};
}  // namespace

void ShadowMapApp::LoadScene() {
    auto scene = scene_manager_->CurrentScene();
    auto root_entity = scene->CreateRootEntity();

    auto camera_entity = root_entity->CreateChild();
    camera_entity->transform->SetPosition(10, 10, 10);
    camera_entity->transform->LookAt(Point3F(0, 0, 0));
    main_camera_ = camera_entity->AddComponent<Camera>();
    camera_entity->AddComponent<control::OrbitControl>();

    // init point light
    auto light = root_entity->CreateChild("light");
    light->AddComponent<LightMovement>();
    auto spot_light = light->AddComponent<SpotLight>();
    spot_light->intensity_ = 0.2;
    spot_light->distance_ = 100;
    spot_light->SetEnableShadow(true);

    auto light3 = root_entity->CreateChild("light3");
    light3->AddComponent<LightMovementReverse>();
    auto direct_light = light3->AddComponent<DirectLight>();
    direct_light->intensity_ = 0.2;
    direct_light->SetEnableShadow(true);

    // create box test entity
    float cube_size = 2.0;
    auto box_entity = root_entity->CreateChild("BoxEntity");
    box_entity->transform->SetPosition(Point3F(0, 2, 0));
    auto box_mtl = std::make_shared<BlinnPhongMaterial>(*device_);
    box_mtl->SetBaseColor(Color(0.3, 0.3, 0.3, 0.5));

    auto box_renderer = box_entity->AddComponent<MeshRenderer>();
    box_renderer->SetMesh(PrimitiveMesh::CreateCuboid(cube_size, cube_size, cube_size));
    box_renderer->SetMaterial(box_mtl);
    box_renderer->cast_shadow_ = true;

    auto plane_entity = root_entity->CreateChild("PlaneEntity");
    auto plane_mtl = std::make_shared<BlinnPhongMaterial>(*device_);
    plane_mtl->SetBaseColor(Color(1.0, 0, 0, 1.0));
    plane_mtl->SetRenderFace(RenderFace::DOUBLE);

    auto plane_renderer = plane_entity->AddComponent<MeshRenderer>();
    plane_renderer->SetMesh(PrimitiveMesh::CreatePlane(10, 10));
    plane_renderer->SetMaterial(plane_mtl);
    plane_renderer->receive_shadow_ = true;

    scene->Play();
}

}  // namespace vox
