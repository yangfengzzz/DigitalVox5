//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "self_collision_app.h"

#include "camera.h"
#include "cloth/cloth_mesh_generator.h"
#include "cloth/simple_mesh_utils.h"
#include "controls/orbit_control.h"
#include "material/blinn_phong_material.h"
#include "mesh/mesh_renderer.h"
#include "mesh/primitive_mesh.h"

namespace vox::cloth {
using namespace physx;

void SelfCollisionApp::InitializeCloth(Entity* entity, int index, physx::PxMat44 transform) {
    solver_ = factory_->createSolver();
    TrackSolver(solver_);

    float w = 5.f - index;
    float h = 6.f + index;
    transform *= PxTransform(PxVec3(0.f, 13.f, 0.f), PxQuat(0, PxVec3(1.f, 0.f, 0.f)));
    ClothMeshData cloth_mesh;
    cloth_mesh.GeneratePlaneCloth(w, h, 5 * w, 5 * h, false, transform);
    cloth_mesh.AttachClothPlaneByAngles(5 * w, 5 * h);
    cloth_mesh.SetInvMasses(1.0f / (1000.0f / (5.0f * w * 5.0f * h)));

    float w_2 = w - 1.0f;
    float h_2 = h + 1.0f;
    transform *= PxTransform(PxVec3(0.f, 0.8f, -0.2f), PxQuat(0, PxVec3(1.f, 0.f, 0.f)));
    ClothMeshData cloth_mesh_2;
    cloth_mesh_2.GeneratePlaneCloth(w_2, h_2, 5 * w_2, 5 * h_2, false, transform);
    cloth_mesh_2.AttachClothPlaneByAngles(5 * w_2, 5 * h_2);
    cloth_mesh_2.SetInvMasses(1.0f / (1000.0f / (5.0f * w_2 * 5.0f * h_2)));
    int first_particle_index_cloth_2 = (int)cloth_mesh.m_vertices.size();
    cloth_mesh.Merge(cloth_mesh_2);

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

    // Create the cloth from the initial positions/masses and the fabric
    cloth_actor_[index].cloth = factory_->createCloth(
            nv::cloth::Range<physx::PxVec4>(&particles_copy[0], &particles_copy[0] + particles_copy.size()),
            *fabric_[index]);

    std::vector<physx::PxVec4> planes;
    planes.emplace_back(physx::PxVec3(0.0f, 1.f, 0.0f), -0.01f);

    nv::cloth::Range<const physx::PxVec4> planes_r(&planes[0], &planes[0] + planes.size());
    cloth_actor_[index].cloth->setPlanes(planes_r, 0, cloth_actor_[index].cloth->getNumPlanes());
    std::vector<uint32_t> indices;
    indices.resize(planes.size());
    for (int i = 0; i < (int)indices.size(); i++) indices[i] = 1 << i;
    nv::cloth::Range<uint32_t> cind(&indices[0], &indices[0] + indices.size());
    cloth_actor_[index].cloth->setConvexes(cind, 0, cloth_actor_[index].cloth->getNumConvexes());

    cloth_actor_[index].cloth->setGravity(physx::PxVec3(0.0f, -1.0f, 0.0f));
    cloth_actor_[index].cloth->setFriction(0.1);
    cloth_actor_[index].cloth->setDragCoefficient(0.1f);
    cloth_actor_[index].cloth->setLiftCoefficient(0.1f);
    cloth_actor_[index].cloth->setSolverFrequency(120.0f);
    cloth_actor_[index].cloth->setSelfCollisionDistance(0.26);
    cloth_actor_[index].cloth->setSelfCollisionStiffness(0.95);

    std::vector<uint32_t> self_collision_indices;

    // only enable every other particle for self collision
    for (int y = 0; y < 5 * h + 1; y++)
        for (int x = 0; x < 5 * w + 1; x++)
            if ((x & 1) ^ (y & 1)) self_collision_indices.push_back(x + y * (5 * w + 1));

    for (int y = 0; y < 5 * h_2 + 1; y++)
        for (int x = 0; x < 5 * w_2 + 1; x++)
            if ((x & 1) ^ (y & 1))
                self_collision_indices.push_back(first_particle_index_cloth_2 + x + y * (5 * w_2 + 1));

    nv::cloth::Range<uint32_t> self_collision_indices_range(&self_collision_indices[0],
                                                            &self_collision_indices[0] + self_collision_indices.size());
    cloth_actor_[index].cloth->setSelfCollisionIndices(self_collision_indices_range);

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
    TrackClothActor(&cloth_actor_[index]);

    // Add the cloth to the solver for simulation
    AddClothToSolver(&cloth_actor_[index], solver_);
}

void SelfCollisionApp::LoadScene() {
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
    physx::PxMat44 pos_trans(physx::PxIdentity);
    pos_trans.setPosition(physx::PxVec3(0.0f, 0.f, -1.0f));
    InitializeCloth(cloth_entity, 0, pos_trans);
}

}  // namespace vox::cloth
