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
void ParticleApp::load_scene() {
    auto scene = scene_manager_->current_scene();
    scene->background_.solidColor_ = Color(0, 0, 0, 1);
    auto root_entity = scene->create_root_entity();
    
    auto camera_entity = root_entity->create_child();
    camera_entity->transform_->set_position(-30, 30, 30);
    camera_entity->transform_->look_at(Point3F(0, 0, 0));
    main_camera_ = camera_entity->add_component<Camera>();
    camera_entity->add_component<control::OrbitControl>();
    
    // init point light
    auto light = root_entity->create_child("light");
    light->transform_->set_position(0, 3, 0);
    auto point_light = light->add_component<PointLight>();
    point_light->intensity_ = 0.3;
    
    auto particle_entity = root_entity->create_child();
    auto particle = particle_entity->add_component<ParticleRenderer>();
    // emitter
    particle->set_particle_min_age(50.f);
    particle->set_particle_max_age(100.f);
    particle->set_emitter_type(ParticleRenderer::EmitterType::SPHERE);
    particle->set_emitter_direction(Vector3F(0, 1, 0));
    particle->set_emitter_position(Vector3F()); // todo
    particle->set_emitter_radius(2.0f);
    particle->set_bounding_volume_type(ParticleRenderer::SimulationVolume::SPHERE);
    particle->set_bbox_size(ParticleRenderer::k_default_simulation_volume_size_);
    
    // simulation
    particle->set_scattering_factor(1.f);
    particle->set_vector_field_factor(1.f);
    particle->set_curl_noise_factor(16.f);
    particle->set_curl_noise_scale(128.f);
    particle->set_velocity_factor(8.f);
    
    // material
    particle->material().set_birth_gradient(Vector3F(0, 1, 0));
    particle->material().set_death_gradient(Vector3F(1, 0, 0));
    particle->material().set_min_particle_size(0.75f);
    particle->material().set_max_particle_size(4.0f);
    particle->material().set_fade_coefficient(0.35f);
    particle->material().set_debug_draw(false);
    
    auto cube_entity = root_entity->create_child();
    auto renderer = cube_entity->add_component<MeshRenderer>();
    renderer->set_mesh(PrimitiveMesh::create_plane(10, 10));
    auto material = std::make_shared<BlinnPhongMaterial>(*device_);
    material->set_base_color(Color(0.4, 0.6, 0.6));
    renderer->set_material(material);
    
    scene->play();
}

}
