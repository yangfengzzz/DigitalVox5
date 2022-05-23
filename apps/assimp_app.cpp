//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "apps/assimp_app.h"

#include <assimp/postprocess.h>

#include "vox.render/assimp_parser.h"
#include "vox.render/camera.h"
#include "vox.render/controls/orbit_control.h"
#include "vox.render/mesh/mesh_renderer.h"

namespace vox {
void AssimpApp::LoadScene() {
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
    cube_entity->transform->SetScale(0.01, 0.01, 0.01);
    AssimpParser parser(*device_);
    parser.LoadModel(cube_entity, "Models/Temple.obj", aiProcess_FlipUVs);

    scene->Play();
}

}  // namespace vox
