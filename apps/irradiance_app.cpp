//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "apps/irradiance_app.h"

#include <utility>

#include "vox.render/camera.h"
#include "vox.render/controls/orbit_control.h"
#include "vox.render/material/pbr_material.h"
#include "vox.render/mesh/mesh_renderer.h"
#include "vox.render/mesh/primitive_mesh.h"

namespace vox {
class BakerMaterial : public BaseMaterial {
public:
    explicit BakerMaterial(Device& device) : BaseMaterial(device, "cubemapDebugger") {
        vertex_source_ = ShaderManager::GetSingleton().LoadShader("base/cubemap-debugger.vert");
        fragment_source_ = ShaderManager::GetSingleton().LoadShader("base/cubemap-debugger.frag");
    }

    void SetBaseTexture(const core::ImageView& image_view) {
        shader_data_.SetSampledTexture(
                base_texture_prop_, image_view,
                &device_.GetResourceCache().RequestSampler(BaseMaterial::last_sampler_create_info_));
    }

    /// Tiling and offset of main textures.
    uint32_t FaceIndex() { return face_index_; }

    void SetFaceIndex(uint32_t new_value) {
        face_index_ = new_value;
        shader_data_.SetData(face_index_prop_, new_value);
    }

private:
    const std::string base_texture_prop_ = "baseTexture";

    uint32_t face_index_{};
    const std::string face_index_prop_ = "faceIndex";
};

void IrradianceApp::LoadScene() {
    auto scene = scene_manager_->CurrentScene();
    auto root_entity = scene->CreateRootEntity();

    auto camera_entity = root_entity->CreateChild();
    camera_entity->transform->SetPosition(0, 0, 10);
    camera_entity->transform->LookAt(Point3F(0, 0, 0));
    main_camera_ = camera_entity->AddComponent<Camera>();
    camera_entity->AddComponent<control::OrbitControl>();

    // Create Sphere
    auto sphere_entity = root_entity->CreateChild("box");
    sphere_entity->transform->SetPosition(-1, 2, 0);
    auto sphere_material = std::make_shared<PbrMaterial>(*device_);
    sphere_material->SetRoughness(0);
    sphere_material->SetMetallic(1);
    auto renderer = sphere_entity->AddComponent<MeshRenderer>();
    renderer->SetMesh(PrimitiveMesh::CreateSphere(1, 64));
    renderer->SetMaterial(sphere_material);

    // Create planes
    std::array<Entity*, 6> planes{};
    std::array<std::shared_ptr<BakerMaterial>, 6> plane_materials{};

    for (int i = 0; i < 6; i++) {
        auto baker_entity = root_entity->CreateChild("IBL Baker Entity");
        baker_entity->transform->SetRotation(90, 0, 0);
        auto baker_material = std::make_shared<BakerMaterial>(*device_);
        auto baker_renderer = baker_entity->AddComponent<MeshRenderer>();
        baker_renderer->SetMesh(PrimitiveMesh::CreatePlane(2, 2));
        baker_renderer->SetMaterial(baker_material);
        planes[i] = baker_entity;
        plane_materials[i] = baker_material;
    }

    planes[0]->transform->SetPosition(1, 0, 0);   // PX
    planes[1]->transform->SetPosition(-3, 0, 0);  // NX
    planes[2]->transform->SetPosition(1, 2, 0);   // PY
    planes[3]->transform->SetPosition(1, -2, 0);  // NY
    planes[4]->transform->SetPosition(-1, 0, 0);  // PZ
    planes[5]->transform->SetPosition(3, 0, 0);   // NZ

    auto ibl_map = TextureManager::GetSingleton().GenerateIBL("Textures/uffizi_rgba16f_cube.ktx", *render_context_);
    scene->AmbientLight()->SetSpecularTexture(ibl_map);

    auto change_mipmap = [&](uint32_t mip_level) {
        for (uint32_t i = 0; i < 6; i++) {
            auto material = plane_materials[i];
            material->SetBaseTexture(ibl_map->GetVkImageView(VK_IMAGE_VIEW_TYPE_2D, 0, i, 0, 1));
            material->SetFaceIndex(i);
        }
    };
    change_mipmap(0);

    scene->Play();
}

}  // namespace vox
