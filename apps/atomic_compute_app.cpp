//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "apps/atomic_compute_app.h"

#include "vox.render/camera.h"
#include "vox.render/controls/orbit_control.h"
#include "vox.render/material/unlit_material.h"
#include "vox.render/mesh/mesh_renderer.h"
#include "vox.render/mesh/primitive_mesh.h"

namespace vox {
class AtomicMaterial : public BaseMaterial {
private:
    const std::string atomic_prop_;
    std::unique_ptr<core::Buffer> atomic_buffer_{nullptr};

public:
    explicit AtomicMaterial(Device &device) : BaseMaterial(device, "atomicRender"), atomic_prop_("atomicCounter") {
        atomic_buffer_ = std::make_unique<core::Buffer>(device, sizeof(uint32_t), VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
                                                        VMA_MEMORY_USAGE_GPU_ONLY);
        shader_data_.SetBufferFunctor(atomic_prop_, [this]() -> core::Buffer * { return atomic_buffer_.get(); });

        vertex_source_ = ShaderManager::GetSingleton().LoadShader("base/unlit.vert");
        fragment_source_ = ShaderManager::GetSingleton().LoadShader("base/compute/atomic_counter.frag");
    }
};

// MARK: - AtomicComputeApp
void AtomicComputeApp::LoadScene() {
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
    point_light->intensity_ = 0.3;

    auto cube_entity = root_entity->CreateChild();
    auto renderer = cube_entity->AddComponent<MeshRenderer>();
    renderer->SetMesh(PrimitiveMesh::CreateCuboid(1));
    material_ = std::make_shared<AtomicMaterial>(*device_);
    renderer->SetMaterial(material_);

    scene->Play();
}

bool AtomicComputeApp::Prepare(Platform &platform) {
    ForwardApplication::Prepare(platform);

    pipeline_ = std::make_unique<PostProcessingPipeline>(*render_context_, ShaderSource());
    auto atomic_pass = &pipeline_->AddPass<PostProcessingComputePass>(
            ShaderManager::GetSingleton().LoadShader("base/compute/atomic_counter.comp"));
    atomic_pass->SetDispatchSize({1, 1, 1});
    atomic_pass->AttachShaderData(&material_->shader_data_);

    return true;
}

void AtomicComputeApp::UpdateGpuTask(CommandBuffer &command_buffer, RenderTarget &render_target) {
    ForwardApplication::UpdateGpuTask(command_buffer, render_target);
    pipeline_->Draw(command_buffer, render_target);
}

}  // namespace vox
