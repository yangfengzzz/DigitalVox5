//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "irradiance_app.h"

#include <utility>
#include "mesh/primitive_mesh.h"
#include "mesh/mesh_renderer.h"
#include "material/pbr_material.h"
#include "camera.h"
#include "image_manager.h"
#include "controls/orbit_control.h"

namespace vox {
class BakerMaterial : public BaseMaterial {
public:
    explicit BakerMaterial(Device& device) : BaseMaterial(device, "cubemapDebugger") {
        vertex_source_ = ShaderManager::get_singleton().load_shader("base/cubemap-debugger.vert");
        fragment_source_ = ShaderManager::get_singleton().load_shader("base/cubemap-debugger.frag");
    }
    
    void set_base_texture(const core::ImageView &image_view) {
        shader_data_.set_sampled_texture(base_texture_prop_, image_view,
                                         MaterialManager::get_singleton().get_sampler(MaterialManager::get_singleton().last_sampler_create_info_));
    }
    
    /// Tiling and offset of main textures.
    uint32_t face_index() {
        return face_index_;
    }
    
    void set_face_index(uint32_t new_value) {
        face_index_ = new_value;
        shader_data_.set_data(face_index_prop_, new_value);
    }
    
private:
    const std::string base_texture_prop_ = "baseTexture";
    
    uint32_t face_index_{};
    const std::string face_index_prop_ = "faceIndex";
};

void IrradianceApp::load_scene() {
    auto scene = scene_manager_->current_scene();
    auto root_entity = scene->create_root_entity();
    
    auto camera_entity = root_entity->create_child();
    camera_entity->transform_->set_position(0, 0, 10);
    camera_entity->transform_->look_at(Point3F(0, 0, 0));
    main_camera_ = camera_entity->add_component<Camera>();
    camera_entity->add_component<control::OrbitControl>();
    
    // Create Sphere
    auto sphere_entity = root_entity->create_child("box");
    sphere_entity->transform_->set_position(-1, 2, 0);
    auto sphere_material = std::make_shared<PbrMaterial>(*device_);
    sphere_material->set_roughness(0);
    sphere_material->set_metallic(1);
    auto renderer = sphere_entity->add_component<MeshRenderer>();
    renderer->set_mesh(PrimitiveMesh::create_sphere(1, 64));
    renderer->set_material(sphere_material);
    
    // Create planes
    std::array<Entity*, 6> planes{};
    std::array<std::shared_ptr<BakerMaterial>, 6> plane_materials{};
    
    for (int i = 0; i < 6; i++) {
        auto baker_entity = root_entity->create_child("IBL Baker Entity");
        baker_entity->transform_->set_rotation(90, 0, 0);
        auto baker_material = std::make_shared<BakerMaterial>(*device_);
        auto baker_renderer = baker_entity->add_component<MeshRenderer>();
        baker_renderer->set_mesh(PrimitiveMesh::create_plane(2, 2));
        baker_renderer->set_material(baker_material);
        planes[i] = baker_entity;
        plane_materials[i] = baker_material;
    }
    
    planes[0]->transform_->set_position(1, 0, 0); // PX
    planes[1]->transform_->set_position(-3, 0, 0); // NX
    planes[2]->transform_->set_position(1, 2, 0); // PY
    planes[3]->transform_->set_position(1, -2, 0); // NY
    planes[4]->transform_->set_position(-1, 0, 0); // PZ
    planes[5]->transform_->set_position(3, 0, 0); // NZ
    
    auto ibl_map = ImageManager::get_singleton().generate_ibl("Textures/uffizi_rgba16f_cube.ktx", *render_context_);
    scene->ambient_light()->set_specular_texture(ibl_map);
    
    auto change_mipmap = [&](uint32_t mipLevel) {
        for (uint32_t i = 0; i < 6; i++) {
            auto material = plane_materials[i];
            material->set_base_texture(ibl_map->get_vk_image_view(VK_IMAGE_VIEW_TYPE_2D, 0, i, 0, 1));
            material->set_face_index(i);
        }
    };
    change_mipmap(0);
    
    scene->play();
}


}
