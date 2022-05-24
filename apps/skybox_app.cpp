//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "apps/skybox_app.h"

#include "vox.render/camera.h"
#include "vox.render/material/unlit_material.h"
#include "vox.render/mesh/mesh_renderer.h"
#include "vox.render/mesh/primitive_mesh.h"
#include "vox.render/rendering/subpasses/skybox_subpass.h"

namespace vox {
bool SkyboxApp::Prepare(Platform &platform) {
    ForwardApplication::Prepare(platform);

    auto scene = scene_manager_->CurrentScene();
    auto skybox = std::make_unique<SkyboxSubpass>(*render_context_, scene, main_camera_);
    skybox->CreateCuboid();
    skybox->FlipVertically();
    skybox->SetTextureCubeMap(TextureManager::GetSingleton().LoadTextureCubemap("Textures/uffizi_rgba16f_cube.ktx"));
    render_pipeline_->AddSubpass(std::move(skybox));

    return true;
}

void SkyboxApp::LoadScene() {
    auto scene = scene_manager_->CurrentScene();
    auto root_entity = scene->CreateRootEntity();

    auto camera_entity = root_entity->CreateChild();
    camera_entity->transform->SetPosition(10, 10, 10);
    camera_entity->transform->LookAt(Point3F(0, 0, 0));
    main_camera_ = camera_entity->AddComponent<Camera>();
    camera_entity->AddComponent<control::OrbitControl>();

    auto model_entity = root_entity->CreateChild();
    auto renderer = model_entity->AddComponent<MeshRenderer>();
    renderer->SetMesh(PrimitiveMesh::CreateCuboid());
    auto material = std::make_shared<UnlitMaterial>(*device_);
    material->SetBaseColor(Color(0.6, 0.4, 0.7, 1.0));
    renderer->SetMaterial(material);

    scene->Play();
}

}  // namespace vox
