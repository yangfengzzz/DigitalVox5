//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "simulator/bar_app.h"

#include "vox.compute/simulation.h"
#include "vox.compute/simulation_model.h"
#include "vox.compute/time_manager.h"
#include "vox.compute/timing.h"
#include "vox.render/camera.h"
#include "vox.render/controls/orbit_control.h"
#include "vox.render/entity.h"
#include "vox.render/material/blinn_phong_material.h"
#include "vox.render/material/unlit_material.h"
#include "vox.render/mesh/mesh_renderer.h"
#include "vox.render/mesh/primitive_mesh.h"

namespace vox {
namespace {
class BarScript : public Script {
public:
    explicit BarScript(Entity *entity) : Script(entity) {
        auto *model = new compute::SimulationModel();
        model->Init();
        compute::Simulation::GetCurrent()->SetModel(model);
        BuildModel();
    }

    void OnUpdate(float delta_time) override {
        compute::SimulationModel *model = compute::Simulation::GetCurrent()->GetModel();
        const unsigned int kNumSteps = 1;
        for (unsigned int i = 0; i < kNumSteps; i++) {
            compute::Simulation::GetCurrent()->GetTimeStep()->Step(*model);
        }

        for (unsigned int i = 0; i < model->GetTetModels().size(); i++) {
            model->GetTetModels()[i]->UpdateMeshNormals(model->GetParticles());
        }
    }

    void BuildModel() {
        compute::TimeManager::GetCurrent()->SetTimeStepSize(0.005f);
        CreateMesh();
    }

    void CreateMesh() {
        compute::SimulationModel *model = compute::Simulation::GetCurrent()->GetModel();
        model->AddRegularTetModel(width_, height_, depth_, compute::Vector3r(5, 0, 0), compute::Matrix3r::Identity(),
                                  compute::Vector3r(10.0, 1.5, 1.5));

        compute::ParticleData &pd = model->GetParticles();
        for (unsigned int i = 0; i < 1; i++) {
            for (unsigned int j = 0; j < height_; j++) {
                for (unsigned int k = 0; k < depth_; k++) pd.SetMass(i * height_ * depth_ + j * depth_ + k, 0.0);
            }
        }

        // init constraints
        stiffness_ = 1.0;
        if (simulation_method_ == 5) stiffness_ = 100000;
        volume_stiffness_ = 1.0;
        if (simulation_method_ == 5) volume_stiffness_ = 100000;
        for (unsigned int cm = 0; cm < model->GetTetModels().size(); cm++) {
            model->AddSolidConstraints(model->GetTetModels()[cm], simulation_method_, stiffness_, poisson_ratio_,
                                       volume_stiffness_, normalize_stretch_, normalize_shear_);

            model->GetTetModels()[cm]->UpdateMeshNormals(pd);

            LOGI("Number of tets: {}", model->GetTetModels()[cm]->GetParticleMesh().NumTets());
            LOGI("Number of vertices: {}", width_ * height_ * depth_)
        }
    }

private:
    const unsigned int width_ = 30;
    const unsigned int depth_ = 5;
    const unsigned int height_ = 5;
    short simulation_method_ = 2;
    float stiffness_ = 1.0;
    float poisson_ratio_ = 0.3;
    bool normalize_stretch_ = false;
    bool normalize_shear_ = false;
    float volume_stiffness_ = 1.0;
};

}  // namespace

void BarApp::LoadScene() {
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
    cube_entity->AddComponent<BarScript>();
    scene->Play();
}

}  // namespace vox
