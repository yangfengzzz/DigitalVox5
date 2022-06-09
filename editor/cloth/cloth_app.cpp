//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "editor/cloth/cloth_app.h"

#include "editor/cloth/cloth_inspector.h"
#include "vox.cloth/cloth_mesh_generator.h"
#include "vox.cloth/cloth_renderer.h"
#include "vox.cloth/NvCloth/Cloth.h"
#include "vox.cloth/NvCloth/Fabric.h"
#include "vox.cloth/NvCloth/Solver.h"
#include "vox.render/camera.h"
#include "vox.render/controls/orbit_control.h"
#include "vox.render/entity.h"
#include "vox.render/material/blinn_phong_material.h"
#include "vox.render/mesh/mesh_renderer.h"
#include "vox.render/mesh/primitive_mesh.h"

namespace vox::editor {
using namespace physx;

void ClothApp::InitializeCloth(Entity *entity, const physx::PxVec3 &offset) {
    solver_ = controller_.Factory()->createSolver();
    controller_.TrackSolver(solver_);

    vox::cloth::ClothMeshData cloth_mesh;
    physx::PxMat44 transform = PxTransform(PxVec3(0.f, 10.f, 0.f) + offset, PxQuat(0, PxVec3(1.f, 0.f, 0.f)));
    cloth_mesh.GeneratePlaneCloth(5.f, 6.f, 20, 20, false, transform);
    //    clothMesh.AttachClothPlaneByAngles(69, 79);
    cloth_mesh.SetInvMasses(0.1f);

    nv::cloth::ClothMeshDesc mesh_desc = cloth_mesh.GetClothMeshDesc();
    nv::cloth::Vector<int32_t>::Type phase_type_info;
    fabric_ = NvClothCookFabricFromMesh(controller_.Factory(), mesh_desc, physx::PxVec3(0.0f, 0.0f, 1.0f),
                                        &phase_type_info, false);
    controller_.TrackFabric(fabric_);

    cloth_actor_ = entity->AddComponent<vox::cloth::ClothRenderer>();
    cloth_actor_->SetClothMeshDesc(mesh_desc);
    auto material = std::make_shared<BlinnPhongMaterial>(*device_);
    material->SetRenderFace(RenderFace::DOUBLE);
    material->SetBaseColor(Color(247 / 256.0, 86 / 256.0, 11 / 256.0));
    cloth_actor_->SetMaterial(material);

    // Initialize start positions and masses for the actual cloth instance
    // (note: the particle/vertex positions do not have to match the mesh description here. Set the positions to the
    // initial shape of this cloth instance)
    std::vector<physx::PxVec4> particles_copy;
    particles_copy.resize(cloth_mesh.m_vertices.size());
    physx::PxVec3 cloth_offset = transform.getPosition();
    for (int i = 0; i < (int)cloth_mesh.m_vertices.size(); i++) {
        // To put attachment point closer to each other
        if (cloth_mesh.m_inv_masses[i] < 1e-6)
            cloth_mesh.m_vertices[i] = (cloth_mesh.m_vertices[i] - cloth_offset) * 0.95f + cloth_offset;

        particles_copy[i] = physx::PxVec4(
                cloth_mesh.m_vertices[i],
                cloth_mesh.m_inv_masses[i]);  // w component is 1/mass, or 0.0f for anchored/fixed particles
    }
    // Create the cloth from the initial positions/masses and the fabric
    cloth_actor_->cloth_ = controller_.Factory()->createCloth(
            nv::cloth::Range<physx::PxVec4>(&particles_copy[0], &particles_copy[0] + particles_copy.size()), *fabric_);
    cloth_actor_->cloth_->setGravity(physx::PxVec3(0.0f, -9.8f, 0.0f));
    cloth_actor_->cloth_->setDamping(physx::PxVec3(0.1f, 0.1f, 0.1f));
    cloth_actor_->cloth_->setFriction(1);
    physx::PxVec4 spheres[2] = {physx::PxVec4(physx::PxVec3(0.f, 0.f, 0.f) + offset, 1.0),
                                physx::PxVec4(physx::PxVec3(0.f, 0.f, 2.f) + offset, 1.0)};
    cloth_actor_->cloth_->setSpheres(nv::cloth::Range<physx::PxVec4>(spheres, spheres + 1), 0,
                                     cloth_actor_->cloth_->getNumSpheres());

    uint32_t caps[4];
    caps[0] = 0;
    caps[1] = 1;
    //    _clothActor.cloth->setCapsules(nv::cloth::Range<uint32_t>(caps, caps + 2), 0,
    //    _clothActor.cloth->getNumCapsules());

    // Setup phase configs
    std::vector<nv::cloth::PhaseConfig> phases(fabric_->getNumPhases());
    for (int i = 0; i < (int)phases.size(); i++) {
        phases[i].mPhaseIndex = i;
        phases[i].mStiffness = 0.6f;
        phases[i].mStiffnessMultiplier = 1.0f;
        phases[i].mCompressionLimit = 1.0f;
        phases[i].mStretchLimit = 1.0f;
    }
    cloth_actor_->cloth_->setPhaseConfig(nv::cloth::Range<nv::cloth::PhaseConfig>(&phases.front(), &phases.back()));
    cloth_actor_->cloth_->setDragCoefficient(0.5f);
    cloth_actor_->cloth_->setLiftCoefficient(0.6f);
    controller_.TrackClothActor(cloth_actor_);

    // Add the cloth to the solver for simulation
    controller_.AddClothToSolver(cloth_actor_, solver_);
}

Camera *ClothApp::LoadScene(Entity *root_entity) {
    wireframe_manager_ = std::make_unique<WireframeManager>(root_entity);

    auto camera_entity = root_entity->CreateChild();
    camera_entity->transform->SetPosition(10, 10, 10);
    camera_entity->transform->LookAt(Point3F(0, 0, 0));
    scene_camera_ = camera_entity->AddComponent<Camera>();
    camera_entity->AddComponent<control::OrbitControl>();

    // init point light
    auto light = root_entity->CreateChild("light");
    light->transform->SetPosition(0, 3, 0);
    auto point_light = light->AddComponent<PointLight>();
    point_light->intensity_ = 1.0;
    point_light->distance_ = 100;

    auto model_entity = root_entity->CreateChild();
    auto renderer = model_entity->AddComponent<MeshRenderer>();
    renderer->SetMesh(PrimitiveMesh::CreateSphere(0.9));
    auto material = std::make_shared<BlinnPhongMaterial>(*device_);
    material->SetBaseColor(Color(0.6, 0.4, 0.7, 1.0));
    renderer->SetMaterial(material);

    auto cloth_entity = root_entity->CreateChild();
    InitializeCloth(cloth_entity, physx::PxVec3(0.0f, 0.0f, 0.0f));

    return scene_camera_;
}

void ClothApp::SetupUi() {
    DemoApplication::SetupUi();

    PanelWindowSettings settings;
    settings.closable = true;
    settings.collapsable = true;
    settings.dockable = true;
    panels_manager_.CreatePanel<ui::ClothInspector>("Inspector", true, settings, controller_);
}

void ClothApp::InputEvent(const vox::InputEvent &input_event) {
    DemoApplication::InputEvent(input_event);
    controller_.HandlePickingEvent(scene_camera_, input_event);
}

void ClothApp::Update(float delta_time) {
    controller_.Update(delta_time);
    DemoApplication::Update(delta_time);
}

}  // namespace vox::editor
