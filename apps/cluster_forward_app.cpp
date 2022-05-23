//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "apps/cluster_forward_app.h"

#include <random>

#include "vox.render/camera.h"
#include "vox.render/controls/orbit_control.h"
#include "vox.render/lighting/debug/cluster_debug_material.h"
#include "vox.render/lighting/debug/sprite_debug.h"
#include "vox.render/lighting/point_light.h"
#include "vox.render/mesh/mesh_renderer.h"
#include "vox.render/mesh/primitive_mesh.h"

namespace vox {
void ClusterForwardApp::LoadScene() {
    auto scene = scene_manager_->CurrentScene();
    scene->AmbientLight()->SetDiffuseSolidColor(Color(1, 1, 1));

    auto root_entity = scene->CreateRootEntity();
    root_entity->AddComponent<SpriteDebug>();

    auto camera_entity = root_entity->CreateChild("camera");
    camera_entity->transform->SetPosition(10, 10, 10);
    camera_entity->transform->LookAt(Point3F(0, 0, 0));
    main_camera_ = camera_entity->AddComponent<Camera>();
    camera_entity->AddComponent<control::OrbitControl>();

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dis(0.0, 1.0);
    // init point light
    for (uint32_t i = 0; i < 15; i++) {
        auto light = root_entity->CreateChild("light");
        light->transform->SetPosition(10 * dis(gen), 2, 10 * dis(gen));
        light->AddComponent<PointLight>();
    }

    // init spot light
    for (uint32_t i = 0; i < 15; i++) {
        auto light = root_entity->CreateChild("light");
        light->transform->SetPosition(10 * dis(gen), 2, 10 * dis(gen));
        light->AddComponent<SpotLight>();
    }

    // init point light
    auto light = root_entity->CreateChild("light");
    light->transform->SetPosition(3, 3, 0);
    light->AddComponent<PointLight>();

    auto light2 = root_entity->CreateChild("light");
    light2->transform->SetPosition(-3, 3, 0);
    light2->AddComponent<SpotLight>();

    // create box test entity
    float cube_size = 20.0;
    auto box_entity = root_entity->CreateChild("BoxEntity");
    auto box_mtl = std::make_shared<ClusterDebugMaterial>(*device_);
    auto box_renderer = box_entity->AddComponent<MeshRenderer>();
    box_renderer->SetMesh(PrimitiveMesh::CreatePlane(cube_size, cube_size));
    box_renderer->SetMaterial(box_mtl);

    scene->Play();
}

}  // namespace vox
