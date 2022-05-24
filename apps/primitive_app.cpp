//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "apps/primitive_app.h"

#include "vox.render/camera.h"
#include "vox.render/controls/orbit_control.h"
#include "vox.render/entity.h"
#include "vox.render/material/blinn_phong_material.h"
#include "vox.render/material/unlit_material.h"
#include "vox.render/mesh/mesh_renderer.h"
#include "vox.render/mesh/primitive_mesh.h"

namespace vox {
namespace {
class MoveScript : public Script {
public:
    explicit MoveScript(Entity *entity) : Script(entity) {}

    void OnUpdate(float delta_time) override {
        r_tri_ += 90 * delta_time;
        GetEntity()->transform->SetRotation(0, r_tri_, 0);
    }

private:
    float r_tri_ = 0;
};

}  // namespace

void PrimitiveApp::LoadScene() {
    auto scene = scene_manager_->CurrentScene();
    auto root_entity = scene->CreateRootEntity();

    auto camera_entity = root_entity->CreateChild();
    camera_entity->transform->SetPosition(10, 10, 10);
    camera_entity->transform->LookAt(Point3F(0, 0, 0));
    main_camera_ = camera_entity->AddComponent<Camera>();
    camera_entity->AddComponent<control::OrbitControl>();

    // init point light
    auto light = root_entity->CreateChild("light");
    light->transform->SetPosition(0, 3, 0);
    auto point_light = light->AddComponent<PointLight>();
    point_light->intensity_ = 1.0;
    point_light->distance_ = 100;

    auto cube_entity = root_entity->CreateChild();
    cube_entity->AddComponent<MoveScript>();
    auto renderer = cube_entity->AddComponent<MeshRenderer>();
    renderer->SetMesh(PrimitiveMesh::CreateCuboid(1));
    auto material = std::make_shared<BlinnPhongMaterial>(*device_);
    material->SetBaseColor(Color(0.4, 0.6, 0.6));
    renderer->SetMaterial(material);

    auto plane_entity = root_entity->CreateChild();
    plane_entity->transform->SetPosition(0, 5, 0);
    auto plane_renderer = plane_entity->AddComponent<MeshRenderer>();
    plane_renderer->SetMesh(PrimitiveMesh::CreateSphere(1));
    auto textured_material = std::make_shared<BlinnPhongMaterial>(*device_);
    textured_material->SetBaseTexture(TextureManager::GetSingleton().LoadTexture("Textures/wood.png"));
    plane_renderer->SetMaterial(textured_material);

    scene->Play();
}

}  // namespace vox
