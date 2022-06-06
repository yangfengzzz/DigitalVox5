//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "simulator/cloth/local_global_app.h"

#include "simulator/cloth/cloth_mesh_generator.h"
#include "simulator/cloth/simple_mesh_utils.h"
#include "vox.render/camera.h"
#include "vox.render/controls/orbit_control.h"
#include "vox.render/material/blinn_phong_material.h"
#include "vox.render/mesh/mesh_renderer.h"
#include "vox.render/mesh/primitive_mesh.h"

namespace vox::cloth {
using namespace physx;

void LocalGlobalApp::InitializeCloth(Entity* entity, int index, const physx::PxVec3& offset) {
    ClothMeshData cloth_mesh;
    physx::PxMat44 transform = PxTransform(PxVec3(0.f, 13.f, 0.f) + offset, PxQuat(PxPi / 2.0f, PxVec3(1.f, 0.f, 0.f)));
    cloth_mesh.GeneratePlaneCloth(5.f, 6.f, 20, 20, false, transform);
    // clothMesh.AttachClothPlaneByAngles(69, 79);
    cloth_mesh.SetInvMasses(0.5f + (float)index * 2.0f);

    nv::cloth::ClothMeshDesc mesh_desc = cloth_mesh.GetClothMeshDesc();
    nv::cloth::Vector<int32_t>::Type phase_type_info;
    fabric_[index] =
            NvClothCookFabricFromMesh(factory_, mesh_desc, physx::PxVec3(0.0f, 0.0f, 1.0f), &phase_type_info, false);
    TrackFabric(fabric_[index]);

    cloth_actor_[index].cloth_renderer = entity->AddComponent<ClothRenderer>();
    cloth_actor_[index].cloth_renderer->SetClothMeshDesc(mesh_desc);
    auto material = std::make_shared<BlinnPhongMaterial>(*device_);
    material->SetRenderFace(RenderFace::DOUBLE);
    material->SetBaseColor(Color(247 / 256.0, 186 / 256.0, 11 / 256.0));
    cloth_actor_[index].cloth_renderer->SetMaterial(material);

    // Initialize start positions and masses for the actual cloth instance
    // (note: the particle/vertex positions do not have to match the mesh description here. Set the positions to the
    // initial shape of this cloth instance)
    std::vector<physx::PxVec4> particles_copy;
    particles_copy.resize(cloth_mesh.m_vertices.size());
    physx::PxVec3 cloth_offset = transform.getPosition();
    for (int i = 0; i < (int)cloth_mesh.m_vertices.size(); i++) {
        // To put attachment point closer to each other
        if (cloth_mesh.m_inv_masses[i] < 1e-6)
            cloth_mesh.m_vertices[i] = (cloth_mesh.m_vertices[i] - cloth_offset) * 0.9f + cloth_offset;

        particles_copy[i] = physx::PxVec4(
                cloth_mesh.m_vertices[i],
                cloth_mesh.m_inv_masses[i]);  // w component is 1/mass, or 0.0f for anchored/fixed particles
    }

    if (index == 1) {
        attachment_vertices_[0] = 0;
        attachment_vertices_[1] = 69;
        attachment_vertex_original_positions_[0] = particles_copy[attachment_vertices_[0]];
        attachment_vertex_original_positions_[1] = particles_copy[attachment_vertices_[1]];
    }

    // Create the cloth from the initial positions/masses and the fabric
    cloth_actor_[index].cloth = factory_->createCloth(
            nv::cloth::Range<physx::PxVec4>(&particles_copy[0], &particles_copy[0] + particles_copy.size()),
            *fabric_[index]);
    cloth_actor_[index].cloth->setGravity(physx::PxVec3(0.0f, -9.8f, 0.0f));
    cloth_actor_[index].cloth->setDragCoefficient(0.1f);
    cloth_actor_[index].cloth->setLiftCoefficient(0.1f);

    // Setup phase configs
    std::vector<nv::cloth::PhaseConfig> phases(fabric_[index]->getNumPhases());
    for (int i = 0; i < (int)phases.size(); i++) {
        phases[i].mPhaseIndex = i;
        phases[i].mStiffness = 0.6f;
        phases[i].mStiffnessMultiplier = 1.0f;
        phases[i].mCompressionLimit = 1.0f;
        phases[i].mStretchLimit = 1.0f;
    }
    cloth_actor_[index].cloth->setPhaseConfig(
            nv::cloth::Range<nv::cloth::PhaseConfig>(&phases.front(), &phases.back()));

    solver_[index] = factory_->createSolver();
    TrackSolver(solver_[index]);
    TrackClothActor(&cloth_actor_[index]);

    // Add the cloth to the solver for simulation
    AddClothToSolver(&cloth_actor_[index], solver_[index]);
}

void LocalGlobalApp::LoadScene() {
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

    auto model_entity = root_entity->CreateChild();
    auto renderer = model_entity->AddComponent<MeshRenderer>();
    renderer->SetMesh(PrimitiveMesh::CreateSphere(0.9));
    auto material = std::make_shared<BlinnPhongMaterial>(*device_);
    material->SetBaseColor(Color(0.6, 0.4, 0.7, 1.0));
    renderer->SetMaterial(material);

    auto cloth_entity = root_entity->CreateChild();
    InitializeCloth(cloth_entity, 1, physx::PxVec3(0.0f, 0.0f, 0.0f));
    InitializeCloth(cloth_entity, 0, physx::PxVec3(0.0f, 0.0f, 0.0f));
}

void LocalGlobalApp::Update(float delta_time) {
    physx::PxVec3 position(sin(time_ * 2.0f) * 3.0f, sinf(time_) * 2.0f, cosf(time_) - 1.0f);
    physx::PxQuat rotation(sin(time_ * 1.0f) * 4.0f, physx::PxVec3(0.0f, 1.0f, 0.0f));

    time_ += delta_time;

    cloth_actor_[0].cloth->setTranslation(position);
    cloth_actor_[0].cloth->setRotation(rotation);
    // _clothActor[0]->mClothRenderable->setTransform(physx::PxTransform(position + physx::PxVec3(-4.f, 0.f, 0.f),
    // rotation));

    // _clothActor[1]->mClothRenderable->setTransform(physx::PxTransform(physx::PxVec3(4.f, 0.f, 0.f),
    // physx::PxQuat(1.f)));
    {
        nv::cloth::MappedRange<physx::PxVec4> particles = cloth_actor_[1].cloth->getCurrentParticles();
        for (int i = 0; i < 2; i++) {
            particles[attachment_vertices_[i]] = physx::PxVec4(
                    physx::PxTransform(position, rotation).transform(attachment_vertex_original_positions_[i].getXYZ()),
                    attachment_vertex_original_positions_[i].w);
        }
    }
    ClothApplication::Update(delta_time);
}

}  // namespace vox::cloth
