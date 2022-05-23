//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "apps/cascade_shadowmap_app.h"

#include "vox.render/camera.h"
#include "vox.render/controls/orbit_control.h"
#include "vox.render/lighting/direct_light.h"
#include "vox.render/material/blinn_phong_material.h"
#include "vox.render/mesh/mesh_renderer.h"
#include "vox.render/mesh/primitive_mesh.h"

namespace vox {
namespace {
class ShadowDebugMaterial : public BaseMaterial {
public:
    ShadowDebugMaterial(Device &device) : BaseMaterial(device, "") {
        vertex_source_ = ShaderManager::GetSingleton().LoadShader("base/blinn-phong.vert");
        fragment_source_ = ShaderManager::GetSingleton().LoadShader("base/shadow/cascade-shadow-debugger.frag");
    }
};

}  // namespace

void CascadeShadowMapApp::LoadScene() {
    auto scene = scene_manager_->CurrentScene();
    auto root_entity = scene->CreateRootEntity();

    auto camera_entity = root_entity->CreateChild();
    camera_entity->transform->SetPosition(0, 10, 50);
    camera_entity->transform->LookAt(Point3F(0, 0, 0));
    main_camera_ = camera_entity->AddComponent<Camera>();
    camera_entity->AddComponent<control::OrbitControl>();

    auto light = root_entity->CreateChild("light");
    light->transform->SetPosition(10, 10, 0);
    light->transform->LookAt(Point3F());
    auto direct_light = light->AddComponent<DirectLight>();
    direct_light->intensity_ = 1.0;
    direct_light->SetEnableShadow(true);

    // create box test entity
    float cube_size = 2.0;
    auto box_mesh = PrimitiveMesh::CreateCuboid(cube_size, cube_size, cube_size);
    auto box_mtl = std::make_shared<BlinnPhongMaterial>(*device_);
    box_mtl->SetBaseColor(Color(0.3, 0.3, 0.3, 0.5));
    for (int i = 0; i < 40; i++) {
        auto box_entity = root_entity->CreateChild("BoxEntity");
        box_entity->transform->SetPosition(Point3F(0, 2, i * 10 - 200));

        auto box_renderer = box_entity->AddComponent<MeshRenderer>();
        box_renderer->SetMesh(box_mesh);
        box_renderer->SetMaterial(box_mtl);
        box_renderer->cast_shadow_ = true;
    }

    auto plane_entity = root_entity->CreateChild("PlaneEntity");
    auto plane_mtl = std::make_shared<BlinnPhongMaterial>(*device_);
    plane_mtl->SetBaseColor(Color(1.0, 0, 0, 1.0));
    plane_mtl->SetRenderFace(RenderFace::DOUBLE);

    auto shadow_debug = std::make_shared<ShadowDebugMaterial>(*device_);

    auto plane_renderer = plane_entity->AddComponent<MeshRenderer>();
    plane_renderer->SetMesh(PrimitiveMesh::CreatePlane(10, 400));
    plane_renderer->SetMaterial(plane_mtl);
    // plane_renderer->set_material(shadow_debug);
    plane_renderer->receive_shadow_ = true;

    scene->Play();
}

}  // namespace vox
