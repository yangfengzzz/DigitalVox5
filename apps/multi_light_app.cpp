//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "apps/multi_light_app.h"

#include <random>

#include "vox.render/camera.h"
#include "vox.render/controls/orbit_control.h"
#include "vox.render/lighting/debug/sprite_debug.h"
#include "vox.render/lighting/point_light.h"
#include "vox.render/material/blinn_phong_material.h"
#include "vox.render/mesh/mesh_renderer.h"
#include "vox.render/mesh/primitive_mesh.h"

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
    explicit MoveScript(Entity *entity) : Script(entity), gen_(rd_()) {
        dis_ = std::uniform_real_distribution<float>(-1.0, 1.0);
        pos_ = Point3F(10 * dis_(gen_), 0, 10 * dis_(gen_));
        vel_ = std::abs(dis_(gen_) * 4);
    }

    void OnUpdate(float delta_time) override {
        if (pos_.y >= 5) {
            vel_sign_ = -1;
        }
        if (pos_.y <= -5) {
            vel_sign_ = 1;
        }
        pos_.y += delta_time * vel_ * float(vel_sign_);

        GetEntity()->transform->SetPosition(pos_);
        GetEntity()->transform->LookAt(Point3F(0, 0, 0));
    }
};

}  // namespace

void MultiLightApp::LoadScene() {
    auto scene = scene_manager_->CurrentScene();
    scene->AmbientLight()->SetDiffuseSolidColor(Color(1, 1, 1));

    auto root_entity = scene->CreateRootEntity();
    root_entity->AddComponent<SpriteDebug>();

    auto camera_entity = root_entity->CreateChild("camera");
    camera_entity->transform->SetPosition(10, 10, 10);
    camera_entity->transform->LookAt(Point3F(0, 0, 0));
    main_camera_ = camera_entity->AddComponent<Camera>();
    camera_entity->AddComponent<control::OrbitControl>();

    std::default_random_engine e;
    std::uniform_real_distribution<float> u(0, 1);
    // init point light
    for (uint32_t i = 0; i < 50; i++) {
        auto light = root_entity->CreateChild("light");
        light->AddComponent<MoveScript>();
        auto point_light = light->AddComponent<PointLight>();
        point_light->color_ = Color(u(e), u(e), u(e), 1);
    }

    // init spot light
    for (uint32_t i = 0; i < 50; i++) {
        auto light = root_entity->CreateChild("light");
        light->AddComponent<MoveScript>();
        auto spot_light = light->AddComponent<SpotLight>();
        spot_light->color_ = Color(u(e), u(e), u(e), 1);
    }

    // create box test entity
    float cube_size = 20.0;
    auto box_entity = root_entity->CreateChild("BoxEntity");
    auto box_mtl = std::make_shared<BlinnPhongMaterial>(*device_);
    box_mtl->SetBaseColor(Color(0.0, 0.6, 0.3, 1.0));
    auto box_renderer = box_entity->AddComponent<MeshRenderer>();
    box_renderer->SetMesh(PrimitiveMesh::CreatePlane(cube_size, cube_size, 100, 1000));
    box_renderer->SetMaterial(box_mtl);

    scene->Play();
}

}  // namespace vox
