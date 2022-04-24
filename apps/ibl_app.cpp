//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "ibl_app.h"
#include "mesh/primitive_mesh.h"
#include "mesh/mesh_renderer.h"
#include "material/pbr_material.h"
#include "rendering/subpasses/skybox_subpass.h"
#include "camera.h"
#include "image_manager.h"
#include "controls/orbit_control.h"

namespace vox {
bool IBLApp::prepare(Platform &platform) {
    ForwardApplication::prepare(platform);
    
    auto scene = scene_manager_->current_scene();
    auto skybox = std::make_unique<SkyboxSubpass>(*render_context_,
                                                  scene, main_camera_);
    skybox->create_cuboid();
    skybox->flip_vertically();
    skybox->set_texture_cube_map(ImageManager::get_singleton().load_texture_cubemap("Textures/uffizi_rgba16f_cube.ktx"));
    render_pipeline_->add_subpass(std::move(skybox));
    
    return true;
}

void IBLApp::load_scene() {
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
    
    const int kMaterialIndex = 7;
    Material mat = materials_[kMaterialIndex];
    
    auto scene = scene_manager_->current_scene();
    auto ibl_map = ImageManager::get_singleton().generate_ibl("Textures/uffizi_rgba16f_cube.ktx", *render_context_);
    auto sh = ImageManager::get_singleton().generate_sh("Textures/uffizi_rgba16f_cube.ktx");
    scene->ambient_light()->set_specular_texture(ibl_map);
    scene->ambient_light()->set_diffuse_mode(DiffuseMode::SPHERICAL_HARMONICS);
    scene->ambient_light()->set_diffuse_spherical_harmonics(sh);
    
    auto root_entity = scene->create_root_entity();
    
    auto camera_entity = root_entity->create_child();
    camera_entity->transform_->set_position(10, 10, 10);
    camera_entity->transform_->look_at(Point3F(0, 0, 0));
    main_camera_ = camera_entity->add_component<Camera>();
    camera_entity->add_component<control::OrbitControl>();
    
    // init point light
    auto light = root_entity->create_child("light");
    light->transform_->set_position(3, 3, 3);
    light->transform_->look_at(Point3F(0, 0, 0));
    auto direction_light = light->add_component<DirectLight>();
    direction_light->intensity_ = 0.3;
    
    auto sphere = PrimitiveMesh::create_sphere(0.5, 30);
    for (int i = 0; i < 7; i++) {
        for (int j = 0; j < 7; j++) {
            auto sphere_entity = root_entity->create_child("SphereEntity" + std::to_string(i) + std::to_string(j));
            sphere_entity->transform_->set_position(Point3F(i - 3, j - 3, 0));
            auto sphere_mtl = std::make_shared<PbrMaterial>(*device_);
            sphere_mtl->set_base_color(mat.base_color);
            sphere_mtl->set_metallic(clamp(float(i) / float(7 - 1), 0.1f, 1.0f));
            sphere_mtl->set_roughness(clamp(float(j) / float(7 - 1), 0.05f, 1.0f));
            
            auto sphere_renderer = sphere_entity->add_component<MeshRenderer>();
            sphere_renderer->set_mesh(sphere);
            sphere_renderer->set_material(sphere_mtl);
        }
    }
    
    scene->play();
}

}
