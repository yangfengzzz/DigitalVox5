//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.force/collision_detection.h"

#include "vox.force/id_factory.h"

namespace vox::force {

int CollisionDetection::CollisionObjectWithoutGeometry::type_id = IDFactory::GetId();
const unsigned int CollisionDetection::rigid_body_contact_type_ = 0;
const unsigned int CollisionDetection::particle_contact_type_ = 1;
const unsigned int CollisionDetection::particle_rigid_body_contact_type_ = 2;
const unsigned int CollisionDetection::particle_solid_contact_type_ = 3;

const unsigned int CollisionDetection::CollisionObject::rigid_body_collision_object_type = 0;
const unsigned int CollisionDetection::CollisionObject::triangle_model_collision_object_type = 1;
const unsigned int CollisionDetection::CollisionObject::tet_model_collision_object_type = 2;

CollisionDetection::CollisionDetection() : m_collision_objects_() {
    m_collision_objects_.reserve(1000);
    m_contact_cb_ = nullptr;
    m_solid_contact_cb_ = nullptr;
    m_tolerance_ = static_cast<Real>(0.01);
}

CollisionDetection::~CollisionDetection() { Cleanup(); }

void CollisionDetection::Cleanup() {
    for (auto &m_collision_object : m_collision_objects_) delete m_collision_object;
    m_collision_objects_.clear();
}

void CollisionDetection::AddRigidBodyContact(unsigned int rb_index_1,
                                             unsigned int rb_index_2,
                                             const Vector3r &cp_1,
                                             const Vector3r &cp_2,
                                             const Vector3r &normal,
                                             Real dist,
                                             Real restitution_coeff,
                                             Real friction_coeff) {
    if (m_contact_cb_)
        m_contact_cb_(rigid_body_contact_type_, rb_index_1, rb_index_2, cp_1, cp_2, normal, dist, restitution_coeff,
                      friction_coeff, m_contact_cb_user_data_);
}

void CollisionDetection::AddParticleRigidBodyContact(unsigned int particle_index,
                                                     unsigned int rb_index,
                                                     const Vector3r &cp_1,
                                                     const Vector3r &cp_2,
                                                     const Vector3r &normal,
                                                     Real dist,
                                                     Real restitution_coeff,
                                                     Real friction_coeff) {
    if (m_contact_cb_)
        m_contact_cb_(particle_rigid_body_contact_type_, particle_index, rb_index, cp_1, cp_2, normal, dist,
                      restitution_coeff, friction_coeff, m_contact_cb_user_data_);
}

void CollisionDetection::AddParticleSolidContact(unsigned int particle_index,
                                                 unsigned int solid_index,
                                                 unsigned int tet_index,
                                                 const Vector3r &bary,
                                                 const Vector3r &cp_1,
                                                 const Vector3r &cp_2,
                                                 const Vector3r &normal,
                                                 Real dist,
                                                 Real restitution_coeff,
                                                 Real friction_coeff) {
    if (m_solid_contact_cb_)
        m_solid_contact_cb_(particle_solid_contact_type_, particle_index, solid_index, tet_index, bary, cp_1, cp_2,
                            normal, dist, restitution_coeff, friction_coeff, m_contact_cb_user_data_);
}

void CollisionDetection::AddCollisionObject(const unsigned int body_index, const unsigned int body_type) {
    auto *co = new CollisionObjectWithoutGeometry();
    co->m_body_index = body_index;
    co->m_body_type = body_type;
    m_collision_objects_.push_back(co);
}

void CollisionDetection::SetContactCallback(CollisionDetection::ContactCallbackFunction val, void *user_data) {
    m_contact_cb_ = val;
    m_contact_cb_user_data_ = user_data;
}

void CollisionDetection::SetSolidContactCallback(CollisionDetection::SolidContactCallbackFunction val,
                                                 void *user_data) {
    m_solid_contact_cb_ = val;
    m_solid_contact_cb_user_data_ = user_data;
}

void CollisionDetection::UpdateAabbs(SimulationModel &model) {
    const SimulationModel::RigidBodyVector &rigid_bodies = model.getRigidBodies();
    const SimulationModel::TriangleModelVector &tri_models = model.getTriangleModels();
    const SimulationModel::TetModelVector &tet_models = model.getTetModels();
    const ParticleData &pd = model.getParticles();

    for (auto co : m_collision_objects_) {
        UpdateAabb(model, co);
    }
}

void CollisionDetection::UpdateAabb(SimulationModel &model, CollisionDetection::CollisionObject *co) const {
    const SimulationModel::RigidBodyVector &rigid_bodies = model.getRigidBodies();
    const SimulationModel::TriangleModelVector &tri_models = model.getTriangleModels();
    const SimulationModel::TetModelVector &tet_models = model.getTetModels();
    const ParticleData &pd = model.getParticles();
    if (co->m_body_type == CollisionDetection::CollisionObject::rigid_body_collision_object_type) {
        const unsigned int kRbIndex = co->m_body_index;
        RigidBody *rb = rigid_bodies[kRbIndex];
        const VertexData &vd = rb->getGeometry().getVertexData();

        co->m_aabb.m_p_[0] = vd.GetPosition(0);
        co->m_aabb.m_p_[1] = vd.GetPosition(0);
        for (unsigned int j = 1; j < vd.Size(); j++) {
            UpdateAabb(vd.GetPosition(j), co->m_aabb);
        }
    } else if (co->m_body_type == CollisionDetection::CollisionObject::triangle_model_collision_object_type) {
        const unsigned int kModelIndex = co->m_body_index;
        TriangleModel *tm = tri_models[kModelIndex];
        const unsigned int kOffset = tm->getIndexOffset();
        const IndexedFaceMesh &mesh = tm->getParticleMesh();
        const unsigned int kNumVert = mesh.NumVertices();

        co->m_aabb.m_p_[0] = pd.GetPosition(kOffset);
        co->m_aabb.m_p_[1] = pd.GetPosition(kOffset);
        for (unsigned int j = kOffset + 1; j < kOffset + kNumVert; j++) {
            UpdateAabb(pd.GetPosition(j), co->m_aabb);
        }
    } else if (co->m_body_type == CollisionDetection::CollisionObject::tet_model_collision_object_type) {
        const unsigned int kModelIndex = co->m_body_index;
        TetModel *tm = tet_models[kModelIndex];
        const unsigned int kOffset = tm->getIndexOffset();
        const IndexedTetMesh &mesh = tm->getParticleMesh();
        const unsigned int kNumVert = mesh.NumVertices();

        co->m_aabb.m_p_[0] = pd.GetPosition(kOffset);
        co->m_aabb.m_p_[1] = pd.GetPosition(kOffset);
        for (unsigned int j = kOffset + 1; j < kOffset + kNumVert; j++) {
            UpdateAabb(pd.GetPosition(j), co->m_aabb);
        }
    }

    // Extend AABB by tolerance
    co->m_aabb.m_p_[0][0] -= m_tolerance_;
    co->m_aabb.m_p_[0][1] -= m_tolerance_;
    co->m_aabb.m_p_[0][2] -= m_tolerance_;
    co->m_aabb.m_p_[1][0] += m_tolerance_;
    co->m_aabb.m_p_[1][1] += m_tolerance_;
    co->m_aabb.m_p_[1][2] += m_tolerance_;
}

void CollisionDetection::UpdateAabb(const Vector3r &p, AABB &aabb) {
    if (aabb.m_p_[0][0] > p[0]) aabb.m_p_[0][0] = p[0];
    if (aabb.m_p_[0][1] > p[1]) aabb.m_p_[0][1] = p[1];
    if (aabb.m_p_[0][2] > p[2]) aabb.m_p_[0][2] = p[2];
    if (aabb.m_p_[1][0] < p[0]) aabb.m_p_[1][0] = p[0];
    if (aabb.m_p_[1][1] < p[1]) aabb.m_p_[1][1] = p[1];
    if (aabb.m_p_[1][2] < p[2]) aabb.m_p_[1][2] = p[2];
}

}  // namespace vox::force