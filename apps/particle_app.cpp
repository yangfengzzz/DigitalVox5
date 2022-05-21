//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "particle_app.h"
#include "mesh/primitive_mesh.h"
#include "mesh/mesh_renderer.h"
#include "material/unlit_material.h"
#include "material/blinn_phong_material.h"
#include "camera.h"
#include "controls/orbit_control.h"

namespace vox {
void ParticleApp::LoadScene() {
    auto scene = scene_manager_->CurrentScene();
    scene->background_.solid_color = Color(0, 0, 0, 1);
    auto root_entity = scene->CreateRootEntity();
    
    auto camera_entity = root_entity->CreateChild();
    camera_entity->transform->SetPosition(-30, 30, 30);
    camera_entity->transform->LookAt(Point3F(0, 0, 0));
    main_camera_ = camera_entity->AddComponent<Camera>();
    camera_entity->AddComponent<control::OrbitControl>();
    
    // init point light
    auto light = root_entity->CreateChild("light");
    light->transform->SetPosition(0, 3, 0);
    auto point_light = light->AddComponent<PointLight>();
    point_light->intensity_ = 0.3;
    
    auto particle_entity = root_entity->CreateChild();
    auto particle = particle_entity->AddComponent<ParticleRenderer>();
    // emitter
    particle->SetParticleMinAge(50.f);
    particle->SetParticleMaxAge(100.f);
    particle->SetEmitterType(ParticleRenderer::EmitterType::SPHERE);
    particle->SetEmitterDirection(Vector3F(0, 1, 0));
    particle->SetEmitterPosition(Vector3F()); // todo
    particle->SetEmitterRadius(2.0f);
    particle->SetBoundingVolumeType(ParticleRenderer::SimulationVolume::SPHERE);
    particle->SetBboxSize(ParticleRenderer::k_default_simulation_volume_size_);
    
    // simulation
    particle->SetScatteringFactor(1.f);
    particle->SetVectorFieldFactor(1.f);
    particle->SetCurlNoiseFactor(16.f);
    particle->SetCurlNoiseScale(128.f);
    particle->SetVelocityFactor(8.f);
    
    // material
    particle->material().SetBirthGradient(Vector3F(0, 1, 0));
    particle->material().SetDeathGradient(Vector3F(1, 0, 0));
    particle->material().SetMinParticleSize(0.75f);
    particle->material().SetMaxParticleSize(4.0f);
    particle->material().SetFadeCoefficient(0.35f);
    particle->material().SetDebugDraw(false);
    
    auto cube_entity = root_entity->CreateChild();
    auto renderer = cube_entity->AddComponent<MeshRenderer>();
    renderer->SetMesh(PrimitiveMesh::CreatePlane(10, 10));
    auto material = std::make_shared<BlinnPhongMaterial>(*device_);
    material->SetBaseColor(Color(0.4, 0.6, 0.6));
    renderer->SetMaterial(material);
    
    scene->play();
}

}
