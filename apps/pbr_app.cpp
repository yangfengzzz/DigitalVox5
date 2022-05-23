//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "apps/pbr_app.h"

#include "vox.render/camera.h"
#include "vox.render/controls/orbit_control.h"
#include "vox.render/lighting/direct_light.h"
#include "vox.render/material/pbr_material.h"
#include "vox.render/mesh/mesh_renderer.h"
#include "vox.render/mesh/primitive_mesh.h"

namespace vox {
void PbrApp::LoadScene() {
    materials_.resize(11);
    materials_[0] = Material("Gold", Color(1.0f, 0.765557f, 0.336057f, 1.0), 0.1f, 1.0f);
    materials_[1] = Material("Copper", Color(0.955008f, 0.637427f, 0.538163f, 1.0), 0.1f, 1.0f);
    materials_[2] = Material("Chromium", Color(0.549585f, 0.556114f, 0.554256f, 1.0), 0.1f, 1.0f);
    materials_[3] = Material("Nickel", Color(0.659777f, 0.608679f, 0.525649f, 1.0), 0.1f, 1.0f);
    materials_[4] = Material("Titanium", Color(0.541931f, 0.496791f, 0.449419f, 1.0), 0.1f, 1.0f);
    materials_[5] = Material("Cobalt", Color(0.662124f, 0.654864f, 0.633732f, 1.0), 0.1f, 1.0f);
    materials_[6] = Material("Platinum", Color(0.672411f, 0.637331f, 0.585456f, 1.0), 0.1f, 1.0f);
    // Testing materials
    materials_[7] = Material("White", Color(1.0f, 1.0, 1.0, 1.0), 0.1f, 1.0f);
    materials_[8] = Material("Red", Color(1.0f, 0.0f, 0.0f, 1.0), 0.1f, 1.0f);
    materials_[9] = Material("Blue", Color(0.0f, 0.0f, 1.0f, 1.0), 0.1f, 1.0f);
    materials_[10] = Material("Black", Color(0.0f, 1.0, 1.0, 1.0), 0.1f, 1.0f);

    const int kMaterialIndex = 0;
    Material mat = materials_[kMaterialIndex];

    auto scene = scene_manager_->CurrentScene();
    auto root_entity = scene->CreateRootEntity();

    auto camera_entity = root_entity->CreateChild();
    camera_entity->transform->SetPosition(10, 10, 10);
    camera_entity->transform->LookAt(Point3F(0, 0, 0));
    main_camera_ = camera_entity->AddComponent<Camera>();
    camera_entity->AddComponent<control::OrbitControl>();

    // init point light
    auto light = root_entity->CreateChild("light");
    light->transform->SetPosition(3, 3, 3);
    light->transform->LookAt(Point3F(0, 0, 0));
    auto direction_light = light->AddComponent<DirectLight>();
    direction_light->intensity_ = 0.3;

    auto sphere = PrimitiveMesh::CreateSphere(0.5, 30);
    for (int i = 0; i < 7; i++) {
        for (int j = 0; j < 7; j++) {
            auto sphere_entity = root_entity->CreateChild("SphereEntity" + std::to_string(i) + std::to_string(j));
            sphere_entity->transform->SetPosition(Point3F(i - 3, j - 3, 0));
            auto sphere_mtl = std::make_shared<PbrMaterial>(*device_);
            sphere_mtl->SetBaseColor(mat.base_color);
            sphere_mtl->SetMetallic(clamp(float(i) / float(7 - 1), 0.1f, 1.0f));
            sphere_mtl->SetRoughness(clamp(float(j) / float(7 - 1), 0.05f, 1.0f));

            auto sphere_renderer = sphere_entity->AddComponent<MeshRenderer>();
            sphere_renderer->SetMesh(sphere);
            sphere_renderer->SetMaterial(sphere_mtl);
        }
    }

    scene->Play();
}

}  // namespace vox
