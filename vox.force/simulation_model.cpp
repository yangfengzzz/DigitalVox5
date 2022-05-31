//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.force/simulation_model.h"

#include "vox.force/constraints.h"
#include "vox.force/position_based_rigid_body_dynamics.h"

namespace vox::force {

SimulationModel::SimulationModel() {
    m_contact_stiffness_rigid_body_ = 1.0;
    m_contact_stiffness_particle_rigid_body_ = 100.0;

    m_groups_initialized_ = false;

    m_rigid_body_contact_constraints_.reserve(10000);
    m_particle_rigid_body_contact_constraints_.reserve(10000);
    m_particle_solid_contact_constraints_.reserve(10000);
}

SimulationModel::~SimulationModel() { Cleanup(); }

void SimulationModel::Init() {}

void SimulationModel::Cleanup() {
    ResetContacts();
    for (auto &m_rigidBodie : m_rigid_bodies_) delete m_rigidBodie;
    m_rigid_bodies_.clear();
    for (auto &m_triangleModel : m_triangle_models_) delete m_triangleModel;
    m_triangle_models_.clear();
    for (auto &m_tetModel : m_tet_models_) delete m_tetModel;
    m_tet_models_.clear();
    for (auto &m_lineModel : m_line_models_) delete m_lineModel;
    m_line_models_.clear();
    for (auto &m_constraint : m_constraints_) delete m_constraint;
    m_constraints_.clear();
    m_particles_.Release();
    m_orientations_.Release();
    m_groups_initialized_ = false;
}

void SimulationModel::Reset() {
    ResetContacts();

    // rigid bodies
    for (auto &m_rigidBodie : m_rigid_bodies_) {
        m_rigidBodie->Reset();
        m_rigidBodie->GetGeometry().UpdateMeshTransformation(m_rigidBodie->GetPosition(),
                                                             m_rigidBodie->GetRotationMatrix());
    }

    // particles
    for (unsigned int i = 0; i < m_particles_.Size(); i++) {
        const Vector3r &x0 = m_particles_.GetPosition0(i);
        m_particles_.GetPosition(i) = x0;
        m_particles_.GetLastPosition(i) = m_particles_.GetPosition(i);
        m_particles_.GetOldPosition(i) = m_particles_.GetPosition(i);
        m_particles_.GetVelocity(i).setZero();
        m_particles_.GetAcceleration(i).setZero();
    }

    // orientations
    for (unsigned int i = 0; i < m_orientations_.Size(); i++) {
        const Quaternionr &q0 = m_orientations_.GetQuaternion0(i);
        m_orientations_.GetQuaternion(i) = q0;
        m_orientations_.GetLastQuaternion(i) = q0;
        m_orientations_.GetOldQuaternion(i) = q0;
        m_orientations_.GetVelocity(i).setZero();
        m_orientations_.GetAcceleration(i).setZero();
    }

    UpdateConstraints();
}

SimulationModel::RigidBodyVector &SimulationModel::GetRigidBodies() { return m_rigid_bodies_; }

ParticleData &SimulationModel::GetParticles() { return m_particles_; }

OrientationData &SimulationModel::GetOrientations() { return m_orientations_; }

SimulationModel::TriangleModelVector &SimulationModel::GetTriangleModels() { return m_triangle_models_; }

SimulationModel::TetModelVector &SimulationModel::GetTetModels() { return m_tet_models_; }

SimulationModel::LineModelVector &SimulationModel::GetLineModels() { return m_line_models_; }

SimulationModel::ConstraintVector &SimulationModel::GetConstraints() { return m_constraints_; }

SimulationModel::RigidBodyContactConstraintVector &SimulationModel::GetRigidBodyContactConstraints() {
    return m_rigid_body_contact_constraints_;
}

SimulationModel::ParticleRigidBodyContactConstraintVector &SimulationModel::GetParticleRigidBodyContactConstraints() {
    return m_particle_rigid_body_contact_constraints_;
}

SimulationModel::ParticleSolidContactConstraintVector &SimulationModel::GetParticleSolidContactConstraints() {
    return m_particle_solid_contact_constraints_;
}

SimulationModel::ConstraintGroupVector &SimulationModel::GetConstraintGroups() { return m_constraint_groups_; }

void SimulationModel::UpdateConstraints() {
    for (auto &m_constraint : m_constraints_) m_constraint->UpdateConstraint(*this);
}

bool SimulationModel::AddBallJoint(unsigned int rbIndex1, unsigned int rbIndex2, const Vector3r &pos) {
    auto *bj = new BallJoint();
    const bool res = bj->InitConstraint(*this, rbIndex1, rbIndex2, pos);
    if (res) {
        m_constraints_.push_back(bj);
        m_groups_initialized_ = false;
    }
    return res;
}

bool SimulationModel::AddBallOnLineJoint(unsigned int rbIndex1,
                                         unsigned int rbIndex2,
                                         const Vector3r &pos,
                                         const Vector3r &dir) {
    auto *bj = new BallOnLineJoint();
    const bool res = bj->InitConstraint(*this, rbIndex1, rbIndex2, pos, dir);
    if (res) {
        m_constraints_.push_back(bj);
        m_groups_initialized_ = false;
    }
    return res;
}

bool SimulationModel::AddHingeJoint(unsigned int rb_index_1,
                                    unsigned int rb_index_2,
                                    const Vector3r &pos,
                                    const Vector3r &axis) {
    auto *hj = new HingeJoint();
    const bool res = hj->InitConstraint(*this, rb_index_1, rb_index_2, pos, axis);
    if (res) {
        m_constraints_.push_back(hj);
        m_groups_initialized_ = false;
    }
    return res;
}

bool SimulationModel::AddUniversalJoint(unsigned int rb_index_1,
                                        unsigned int rb_index_2,
                                        const Vector3r &pos,
                                        const Vector3r &axis1,
                                        const Vector3r &axis2) {
    auto *uj = new UniversalJoint();
    const bool res = uj->InitConstraint(*this, rb_index_1, rb_index_2, pos, axis1, axis2);
    if (res) {
        m_constraints_.push_back(uj);
        m_groups_initialized_ = false;
    }
    return res;
}

bool SimulationModel::AddSliderJoint(unsigned int rb_index_1, unsigned int rb_index_2, const Vector3r &axis) {
    auto *joint = new SliderJoint();
    const bool res = joint->InitConstraint(*this, rb_index_1, rb_index_2, axis);
    if (res) {
        m_constraints_.push_back(joint);
        m_groups_initialized_ = false;
    }
    return res;
}

bool SimulationModel::AddTargetPositionMotorSliderJoint(unsigned int rb_index_1,
                                                        unsigned int rb_index_2,
                                                        const Vector3r &axis) {
    auto *joint = new TargetPositionMotorSliderJoint();
    const bool res = joint->InitConstraint(*this, rb_index_1, rb_index_2, axis);
    if (res) {
        m_constraints_.push_back(joint);
        m_groups_initialized_ = false;
    }
    return res;
}

bool SimulationModel::AddTargetVelocityMotorSliderJoint(unsigned int rb_index_1,
                                                        unsigned int rb_index_2,
                                                        const Vector3r &axis) {
    auto *joint = new TargetVelocityMotorSliderJoint();
    const bool res = joint->InitConstraint(*this, rb_index_1, rb_index_2, axis);
    if (res) {
        m_constraints_.push_back(joint);
        m_groups_initialized_ = false;
    }
    return res;
}

bool SimulationModel::AddTargetAngleMotorHingeJoint(unsigned int rb_index_1,
                                                    unsigned int rb_index_2,
                                                    const Vector3r &pos,
                                                    const Vector3r &axis) {
    auto *hj = new TargetAngleMotorHingeJoint();
    const bool res = hj->InitConstraint(*this, rb_index_1, rb_index_2, pos, axis);
    if (res) {
        m_constraints_.push_back(hj);
        m_groups_initialized_ = false;
    }
    return res;
}

bool SimulationModel::AddTargetVelocityMotorHingeJoint(unsigned int rb_index_1,
                                                       unsigned int rb_index_2,
                                                       const Vector3r &pos,
                                                       const Vector3r &axis) {
    auto *hj = new TargetVelocityMotorHingeJoint();
    const bool res = hj->InitConstraint(*this, rb_index_1, rb_index_2, pos, axis);
    if (res) {
        m_constraints_.push_back(hj);
        m_groups_initialized_ = false;
    }
    return res;
}

bool SimulationModel::AddDamperJoint(unsigned int rb_index_1,
                                     unsigned int rb_index_2,
                                     const Vector3r &axis,
                                     Real stiffness) {
    auto *joint = new DamperJoint();
    const bool res = joint->InitConstraint(*this, rb_index_1, rb_index_2, axis, stiffness);
    if (res) {
        m_constraints_.push_back(joint);
        m_groups_initialized_ = false;
    }
    return res;
}

bool SimulationModel::AddRigidBodyParticleBallJoint(const unsigned int rb_index, const unsigned int particle_index) {
    auto *bj = new RigidBodyParticleBallJoint();
    const bool res = bj->InitConstraint(*this, rb_index, particle_index);
    if (res) {
        m_constraints_.push_back(bj);
        m_groups_initialized_ = false;
    }
    return res;
}

bool SimulationModel::AddRigidBodySpring(
        unsigned int rb_index_1, unsigned int rb_index_2, const Vector3r &pos1, const Vector3r &pos2, Real stiffness) {
    auto *s = new RigidBodySpring();
    const bool res = s->InitConstraint(*this, rb_index_1, rb_index_2, pos1, pos2, stiffness);
    if (res) {
        m_constraints_.push_back(s);
        m_groups_initialized_ = false;
    }
    return res;
}

bool SimulationModel::AddDistanceJoint(unsigned int rb_index_1,
                                       unsigned int rb_index_2,
                                       const Vector3r &pos1,
                                       const Vector3r &pos2) {
    auto *j = new DistanceJoint();
    const bool res = j->InitConstraint(*this, rb_index_1, rb_index_2, pos1, pos2);
    if (res) {
        m_constraints_.push_back(j);
        m_groups_initialized_ = false;
    }
    return res;
}

bool SimulationModel::AddRigidBodyContactConstraint(unsigned int rb_index_1,
                                                    unsigned int rb_index_2,
                                                    const Vector3r &cp1,
                                                    const Vector3r &cp2,
                                                    const Vector3r &normal,
                                                    Real dist,
                                                    Real restitution_coeff,
                                                    Real friction_coeff) {
    m_rigid_body_contact_constraints_.emplace_back(RigidBodyContactConstraint());
    RigidBodyContactConstraint &cc = m_rigid_body_contact_constraints_.back();
    const bool res = cc.InitConstraint(*this, rb_index_1, rb_index_2, cp1, cp2, normal, dist, restitution_coeff,
                                       m_contact_stiffness_rigid_body_, friction_coeff);
    if (!res) m_rigid_body_contact_constraints_.pop_back();
    return res;
}

bool SimulationModel::AddParticleRigidBodyContactConstraint(unsigned int particle_index,
                                                            unsigned int rb_index,
                                                            const Vector3r &cp1,
                                                            const Vector3r &cp2,
                                                            const Vector3r &normal,
                                                            Real dist,
                                                            Real restitution_coeff,
                                                            Real friction_coeff) {
    m_particle_rigid_body_contact_constraints_.emplace_back(ParticleRigidBodyContactConstraint());
    ParticleRigidBodyContactConstraint &cc = m_particle_rigid_body_contact_constraints_.back();
    const bool res = cc.InitConstraint(*this, particle_index, rb_index, cp1, cp2, normal, dist, restitution_coeff,
                                       m_contact_stiffness_particle_rigid_body_, friction_coeff);
    if (!res) m_particle_rigid_body_contact_constraints_.pop_back();
    return res;
}

bool SimulationModel::AddParticleSolidContactConstraint(unsigned int particle_index,
                                                        unsigned int solid_index,
                                                        unsigned int tet_index,
                                                        const Vector3r &bary,
                                                        const Vector3r &cp1,
                                                        const Vector3r &cp2,
                                                        const Vector3r &normal,
                                                        Real dist,
                                                        Real restitution_coeff,
                                                        Real friction_coeff) {
    m_particle_solid_contact_constraints_.emplace_back(ParticleTetContactConstraint());
    ParticleTetContactConstraint &cc = m_particle_solid_contact_constraints_.back();
    const bool res = cc.InitConstraint(*this, particle_index, solid_index, tet_index, bary, cp1, cp2, normal, dist,
                                       friction_coeff);
    if (!res) m_particle_solid_contact_constraints_.pop_back();
    return res;
}

bool SimulationModel::AddDistanceConstraint(unsigned int particle1, unsigned int particle2, Real stiffness) {
    auto *c = new DistanceConstraint();
    const bool res = c->InitConstraint(*this, particle1, particle2, stiffness);
    if (res) {
        m_constraints_.push_back(c);
        m_groups_initialized_ = false;
    }
    return res;
}

bool SimulationModel::AddDistanceConstraintXPBD(unsigned int particle1, unsigned int particle2, Real stiffness) {
    auto *c = new DistanceConstraint_XPBD();
    const bool res = c->InitConstraint(*this, particle1, particle2, stiffness);
    if (res) {
        m_constraints_.push_back(c);
        m_groups_initialized_ = false;
    }
    return res;
}

bool SimulationModel::AddDihedralConstraint(unsigned int particle1,
                                            unsigned int particle2,
                                            unsigned int particle3,
                                            unsigned int particle4,
                                            Real stiffness) {
    auto *c = new DihedralConstraint();
    const bool res = c->InitConstraint(*this, particle1, particle2, particle3, particle4, stiffness);
    if (res) {
        m_constraints_.push_back(c);
        m_groups_initialized_ = false;
    }
    return res;
}

bool SimulationModel::AddIsometricBendingConstraint(unsigned int particle1,
                                                    unsigned int particle2,
                                                    unsigned int particle3,
                                                    unsigned int particle4,
                                                    Real stiffness) {
    auto *c = new IsometricBendingConstraint();
    const bool res = c->InitConstraint(*this, particle1, particle2, particle3, particle4, stiffness);
    if (res) {
        m_constraints_.push_back(c);
        m_groups_initialized_ = false;
    }
    return res;
}

bool SimulationModel::AddIsometricBendingConstraintXpbd(unsigned int particle1,
                                                        unsigned int particle2,
                                                        unsigned int particle3,
                                                        unsigned int particle4,
                                                        Real stiffness) {
    auto *c = new IsometricBendingConstraint_XPBD();
    const bool res = c->InitConstraint(*this, particle1, particle2, particle3, particle4, stiffness);
    if (res) {
        m_constraints_.push_back(c);
        m_groups_initialized_ = false;
    }
    return res;
}

bool SimulationModel::AddFemTriangleConstraint(unsigned int particle1,
                                               unsigned int particle2,
                                               unsigned int particle3,
                                               Real xx_stiffness,
                                               Real yy_stiffness,
                                               Real xy_stiffness,
                                               Real xy_poisson_ratio,
                                               Real yx_poisson_ratio) {
    auto *c = new FEMTriangleConstraint();
    const bool res = c->InitConstraint(*this, particle1, particle2, particle3, xx_stiffness, yy_stiffness, xy_stiffness,
                                       xy_poisson_ratio, yx_poisson_ratio);
    if (res) {
        m_constraints_.push_back(c);
        m_groups_initialized_ = false;
    }
    return res;
}

bool SimulationModel::AddStrainTriangleConstraint(unsigned int particle1,
                                                  unsigned int particle2,
                                                  unsigned int particle3,
                                                  Real xx_stiffness,
                                                  Real yy_stiffness,
                                                  Real xy_stiffness,
                                                  bool normalize_stretch,
                                                  bool normalize_shear) {
    auto *c = new StrainTriangleConstraint();
    const bool res = c->InitConstraint(*this, particle1, particle2, particle3, xx_stiffness, yy_stiffness, xy_stiffness,
                                       normalize_stretch, normalize_shear);
    if (res) {
        m_constraints_.push_back(c);
        m_groups_initialized_ = false;
    }
    return res;
}

bool SimulationModel::AddVolumeConstraint(unsigned int particle1,
                                          unsigned int particle2,
                                          unsigned int particle3,
                                          unsigned int particle4,
                                          Real stiffness) {
    auto *c = new VolumeConstraint();
    const bool res = c->InitConstraint(*this, particle1, particle2, particle3, particle4, stiffness);
    if (res) {
        m_constraints_.push_back(c);
        m_groups_initialized_ = false;
    }
    return res;
}

bool SimulationModel::AddVolumeConstraintXPBD(unsigned int particle1,
                                              unsigned int particle2,
                                              unsigned int particle3,
                                              unsigned int particle4,
                                              Real stiffness) {
    auto *c = new VolumeConstraint_XPBD();
    const bool res = c->InitConstraint(*this, particle1, particle2, particle3, particle4, stiffness);
    if (res) {
        m_constraints_.push_back(c);
        m_groups_initialized_ = false;
    }
    return res;
}

bool SimulationModel::AddFemTetConstraint(unsigned int particle1,
                                          unsigned int particle2,
                                          unsigned int particle3,
                                          unsigned int particle4,
                                          Real stiffness,
                                          Real poisson_ratio) {
    auto *c = new FEMTetConstraint();
    const bool res = c->InitConstraint(*this, particle1, particle2, particle3, particle4, stiffness, poisson_ratio);
    if (res) {
        m_constraints_.push_back(c);
        m_groups_initialized_ = false;
    }
    return res;
}

bool SimulationModel::AddStrainTetConstraint(unsigned int particle1,
                                             unsigned int particle2,
                                             unsigned int particle3,
                                             unsigned int particle4,
                                             Real stretch_stiffness,
                                             Real shear_stiffness,
                                             bool normalize_stretch,
                                             bool normalize_shear) {
    auto *c = new StrainTetConstraint();
    const bool res = c->InitConstraint(*this, particle1, particle2, particle3, particle4, stretch_stiffness,
                                       shear_stiffness, normalize_stretch, normalize_shear);
    if (res) {
        m_constraints_.push_back(c);
        m_groups_initialized_ = false;
    }
    return res;
}

bool SimulationModel::AddShapeMatchingConstraint(unsigned int number_of_particles,
                                                 const unsigned int particle_indices[],
                                                 const unsigned int num_clusters[],
                                                 Real stiffness) {
    auto *c = new ShapeMatchingConstraint(number_of_particles);
    const bool res = c->InitConstraint(*this, particle_indices, num_clusters, stiffness);
    if (res) {
        m_constraints_.push_back(c);
        m_groups_initialized_ = false;
    }
    return res;
}

bool SimulationModel::AddStretchShearConstraint(unsigned int particle1,
                                                unsigned int particle2,
                                                unsigned int quaternion1,
                                                Real stretching_stiffness,
                                                Real shearing_stiffness_1,
                                                Real shearing_stiffness_2) {
    auto *c = new StretchShearConstraint();
    const bool res = c->InitConstraint(*this, particle1, particle2, quaternion1, stretching_stiffness,
                                       shearing_stiffness_1, shearing_stiffness_2);
    if (res) {
        m_constraints_.push_back(c);
        m_groups_initialized_ = false;
    }
    return res;
}

bool SimulationModel::AddBendTwistConstraint(unsigned int quaternion1,
                                             unsigned int quaternion2,
                                             Real twisting_stiffness,
                                             Real bending_stiffness_1,
                                             Real bending_stiffness_2) {
    auto *c = new BendTwistConstraint();
    const bool res = c->InitConstraint(*this, quaternion1, quaternion2, twisting_stiffness, bending_stiffness_1,
                                       bending_stiffness_2);
    if (res) {
        m_constraints_.push_back(c);
        m_groups_initialized_ = false;
    }
    return res;
}

bool SimulationModel::AddStretchBendingTwistingConstraint(unsigned int rb_index_1,
                                                          unsigned int rb_index_2,
                                                          const Vector3r &pos,
                                                          Real average_radius,
                                                          Real average_segment_length,
                                                          Real youngs_modulus,
                                                          Real torsion_modulus) {
    auto *c = new StretchBendingTwistingConstraint();
    const bool res = c->InitConstraint(*this, rb_index_1, rb_index_2, pos, average_radius, average_segment_length,
                                       youngs_modulus, torsion_modulus);
    if (res) {
        m_constraints_.push_back(c);
        m_groups_initialized_ = false;
    }
    return res;
}

bool SimulationModel::AddDirectPositionBasedSolverForStiffRodsConstraint(
        const std::vector<std::pair<unsigned int, unsigned int>> &joint_segment_indices,
        const std::vector<Vector3r> &joint_positions,
        const std::vector<Real> &average_radii,
        const std::vector<Real> &average_segment_lengths,
        const std::vector<Real> &youngs_moduli,
        const std::vector<Real> &torsion_moduli) {
    auto *c = new DirectPositionBasedSolverForStiffRodsConstraint();
    const bool res = c->InitConstraint(*this, joint_segment_indices, joint_positions, average_radii,
                                       average_segment_lengths, youngs_moduli, torsion_moduli);
    if (res) {
        m_constraints_.push_back(c);
        m_groups_initialized_ = false;
    }
    return res;
}

void SimulationModel::AddTriangleModel(unsigned int n_points,
                                       unsigned int n_faces,
                                       Vector3r *points,
                                       unsigned int *indices,
                                       const TriangleModel::ParticleMesh::UVIndices &uv_indices,
                                       const TriangleModel::ParticleMesh::UVs &uvs) {
    auto *triModel = new TriangleModel();
    m_triangle_models_.push_back(triModel);

    unsigned int startIndex = m_particles_.Size();
    m_particles_.Reserve(startIndex + n_points);

    for (unsigned int i = 0; i < n_points; i++) m_particles_.AddVertex(points[i]);

    triModel->InitMesh(n_points, n_faces, startIndex, indices, uv_indices, uvs);

    // Update normals
    triModel->UpdateMeshNormals(m_particles_);
}

void SimulationModel::AddRegularTriangleModel(
        int width, int height, const Vector3r &translation, const Matrix3r &rotation, const Vector2r &scale) {
    TriangleModel::ParticleMesh::UVs uvs;
    uvs.resize(width * height);

    const Real dy = scale[1] / (Real)(height - 1);
    const Real dx = scale[0] / (Real)(width - 1);

    std::vector<Vector3r> points;
    points.resize(width * height);
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            const Real y = (Real)dy * i;
            const Real x = (Real)dx * j;
            points[i * width + j] = rotation * Vector3r(x, y, 0.0) + translation;

            uvs[i * width + j][0] = x / scale[0];
            uvs[i * width + j][1] = y / scale[1];
        }
    }
    const int nIndices = 6 * (height - 1) * (width - 1);

    TriangleModel::ParticleMesh::UVIndices uvIndices;
    uvIndices.resize(nIndices);

    std::vector<unsigned int> indices;
    indices.resize(nIndices);
    int index = 0;
    for (int i = 0; i < height - 1; i++) {
        for (int j = 0; j < width - 1; j++) {
            int helper = 0;
            if (i % 2 == j % 2) helper = 1;

            indices[index] = i * width + j;
            indices[index + 1] = i * width + j + 1;
            indices[index + 2] = (i + 1) * width + j + helper;

            uvIndices[index] = i * width + j;
            uvIndices[index + 1] = i * width + j + 1;
            uvIndices[index + 2] = (i + 1) * width + j + helper;
            index += 3;

            indices[index] = (i + 1) * width + j + 1;
            indices[index + 1] = (i + 1) * width + j;
            indices[index + 2] = i * width + j + 1 - helper;

            uvIndices[index] = (i + 1) * width + j + 1;
            uvIndices[index + 1] = (i + 1) * width + j;
            uvIndices[index + 2] = i * width + j + 1 - helper;
            index += 3;
        }
    }

    const unsigned int nPoints = height * width;
    const unsigned int nFaces = nIndices / 3;
    const auto modelIndex = m_triangle_models_.size();
    AddTriangleModel(nPoints, nFaces, points.data(), indices.data(), uvIndices, uvs);
    const auto offset = m_triangle_models_[modelIndex]->GetIndexOffset();

    ParticleData &pd = GetParticles();
    for (unsigned int i = offset; i < offset + m_triangle_models_[modelIndex]->GetParticleMesh().NumVertices(); i++)
        pd.SetMass(i, 1.0);
}

void SimulationModel::AddTetModel(unsigned int n_points, unsigned int n_tets, Vector3r *points, unsigned int *indices) {
    auto *tetModel = new TetModel();
    m_tet_models_.push_back(tetModel);

    unsigned int startIndex = m_particles_.Size();
    m_particles_.Reserve(startIndex + n_points);

    for (unsigned int i = 0; i < n_points; i++) m_particles_.AddVertex(points[i]);

    tetModel->InitMesh(n_points, n_tets, startIndex, indices);
}

void SimulationModel::AddRegularTetModel(int width,
                                         int height,
                                         int depth,
                                         const Vector3r &translation,
                                         const Matrix3r &rotation,
                                         const Vector3r &scale) {
    std::vector<Vector3r> points;
    points.resize(width * height * depth);

    const Real dx = scale[0] / (Real)(width - 1);
    const Real dy = scale[1] / (Real)(height - 1);
    const Real dz = scale[2] / (Real)(depth - 1);

    // center in origin
    const Vector3r t = translation - 0.5 * scale;
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            for (int k = 0; k < depth; k++) {
                const Real x = (Real)dx * i;
                const Real y = (Real)dy * j;
                const Real z = (Real)dz * k;

                points[i * height * depth + j * depth + k] = rotation * Vector3r(x, y, z) + t;
            }
        }
    }

    std::vector<unsigned int> indices;
    indices.reserve(width * height * depth * 5);
    for (int i = 0; i < width - 1; i++) {
        for (int j = 0; j < height - 1; j++) {
            for (int k = 0; k < depth - 1; k++) {
                // For each block, the 8 corners are numerated as:
                //     4*-----*7
                //     /|    /|
                //    / |   / |
                //  5*-----*6 |
                //   | 0*--|--*3
                //   | /   | /
                //   |/    |/
                //  1*-----*2
                unsigned int p0 = i * height * depth + j * depth + k;
                unsigned int p1 = p0 + 1;
                unsigned int p3 = (i + 1) * height * depth + j * depth + k;
                unsigned int p2 = p3 + 1;
                unsigned int p7 = (i + 1) * height * depth + (j + 1) * depth + k;
                unsigned int p6 = p7 + 1;
                unsigned int p4 = i * height * depth + (j + 1) * depth + k;
                unsigned int p5 = p4 + 1;

                // Ensure that neighboring tetras are sharing faces
                if ((i + j + k) % 2 == 1) {
                    indices.push_back(p2);
                    indices.push_back(p1);
                    indices.push_back(p6);
                    indices.push_back(p3);
                    indices.push_back(p6);
                    indices.push_back(p3);
                    indices.push_back(p4);
                    indices.push_back(p7);
                    indices.push_back(p4);
                    indices.push_back(p1);
                    indices.push_back(p6);
                    indices.push_back(p5);
                    indices.push_back(p3);
                    indices.push_back(p1);
                    indices.push_back(p4);
                    indices.push_back(p0);
                    indices.push_back(p6);
                    indices.push_back(p1);
                    indices.push_back(p4);
                    indices.push_back(p3);
                } else {
                    indices.push_back(p0);
                    indices.push_back(p2);
                    indices.push_back(p5);
                    indices.push_back(p1);
                    indices.push_back(p7);
                    indices.push_back(p2);
                    indices.push_back(p0);
                    indices.push_back(p3);
                    indices.push_back(p5);
                    indices.push_back(p2);
                    indices.push_back(p7);
                    indices.push_back(p6);
                    indices.push_back(p7);
                    indices.push_back(p0);
                    indices.push_back(p5);
                    indices.push_back(p4);
                    indices.push_back(p0);
                    indices.push_back(p2);
                    indices.push_back(p7);
                    indices.push_back(p5);
                }
            }
        }
    }
    const auto modelIndex = m_tet_models_.size();
    AddTetModel(width * height * depth, (unsigned int)indices.size() / 4u, points.data(), indices.data());
    const auto offset = m_tet_models_[modelIndex]->GetIndexOffset();

    ParticleData &pd = GetParticles();
    for (unsigned int i = offset; i < offset + m_tet_models_[modelIndex]->GetParticleMesh().NumVertices(); i++) {
        pd.SetMass(i, 1.0);
    }
}

void SimulationModel::AddLineModel(unsigned int n_points,
                                   unsigned int n_quaternions,
                                   Vector3r *points,
                                   Quaternionr *quaternions,
                                   unsigned int *indices,
                                   unsigned int *indices_quaternions) {
    auto *lineModel = new LineModel();
    m_line_models_.push_back(lineModel);

    unsigned int startIndex = m_particles_.Size();
    m_particles_.Reserve(startIndex + n_points);

    for (unsigned int i = 0; i < n_points; i++) m_particles_.AddVertex(points[i]);

    unsigned int startIndexOrientations = m_orientations_.Size();
    m_orientations_.Reserve(startIndexOrientations + n_quaternions);

    for (unsigned int i = 0; i < n_quaternions; i++) m_orientations_.AddQuaternion(quaternions[i]);

    lineModel->InitMesh(n_points, n_quaternions, startIndex, startIndexOrientations, indices, indices_quaternions);
}

void SimulationModel::InitConstraintGroups() {
    if (m_groups_initialized_) return;

    const auto numConstraints = (unsigned int)m_constraints_.size();
    const auto numParticles = (unsigned int)m_particles_.Size();
    const auto numRigidBodies = (unsigned int)m_rigid_bodies_.size();
    const unsigned int numBodies = numParticles + numRigidBodies;
    m_constraint_groups_.clear();

    // Maps in which group a particle is or 0 if not yet mapped
    std::vector<unsigned char *> mapping;

    for (unsigned int i = 0; i < numConstraints; i++) {
        Constraint *constraint = m_constraints_[i];

        bool addToNewGroup = true;
        for (unsigned int j = 0; j < m_constraint_groups_.size(); j++) {
            bool addToThisGroup = true;

            for (unsigned int k = 0; k < constraint->NumberOfBodies(); k++) {
                if (mapping[j][constraint->m_bodies[k]] != 0) {
                    addToThisGroup = false;
                    break;
                }
            }

            if (addToThisGroup) {
                m_constraint_groups_[j].push_back(i);

                for (unsigned int k = 0; k < constraint->NumberOfBodies(); k++) mapping[j][constraint->m_bodies[k]] = 1;

                addToNewGroup = false;
                break;
            }
        }
        if (addToNewGroup) {
            mapping.push_back(new unsigned char[numBodies]);
            memset(mapping[mapping.size() - 1], 0, sizeof(unsigned char) * numBodies);
            m_constraint_groups_.resize(m_constraint_groups_.size() + 1);
            m_constraint_groups_[m_constraint_groups_.size() - 1].push_back(i);
            for (unsigned int k = 0; k < constraint->NumberOfBodies(); k++)
                mapping[m_constraint_groups_.size() - 1][constraint->m_bodies[k]] = 1;
        }
    }

    for (auto &i : mapping) {
        delete[] i;
    }
    mapping.clear();

    m_groups_initialized_ = true;
}

void SimulationModel::ResetContacts() {
    m_rigid_body_contact_constraints_.clear();
    m_particle_rigid_body_contact_constraints_.clear();
    m_particle_solid_contact_constraints_.clear();
}

void SimulationModel::AddClothConstraints(const TriangleModel *tm,
                                          unsigned int cloth_method,
                                          Real distance_stiffness,
                                          Real xx_stiffness,
                                          Real yy_stiffness,
                                          Real xy_stiffness,
                                          Real xy_poisson_ratio,
                                          Real yx_poisson_ratio,
                                          bool normalize_stretch,
                                          bool normalize_shear) {
    if (cloth_method == 1) {
        const unsigned int offset = tm->GetIndexOffset();
        const unsigned int nEdges = tm->GetParticleMesh().NumEdges();
        const IndexedFaceMesh::Edge *edges = tm->GetParticleMesh().GetEdges().data();
        for (unsigned int i = 0; i < nEdges; i++) {
            const unsigned int v1 = edges[i].m_vert[0] + offset;
            const unsigned int v2 = edges[i].m_vert[1] + offset;

            AddDistanceConstraint(v1, v2, distance_stiffness);
        }
    } else if (cloth_method == 2) {
        const unsigned int offset = tm->GetIndexOffset();
        const TriangleModel::ParticleMesh &mesh = tm->GetParticleMesh();
        const unsigned int *tris = mesh.GetFaces().data();
        const unsigned int nFaces = mesh.NumFaces();
        for (unsigned int i = 0; i < nFaces; i++) {
            const unsigned int v1 = tris[3 * i] + offset;
            const unsigned int v2 = tris[3 * i + 1] + offset;
            const unsigned int v3 = tris[3 * i + 2] + offset;
            AddFemTriangleConstraint(v1, v2, v3, xx_stiffness, yy_stiffness, xy_stiffness, xy_poisson_ratio,
                                     yx_poisson_ratio);
        }
    } else if (cloth_method == 3) {
        const unsigned int offset = tm->GetIndexOffset();
        const TriangleModel::ParticleMesh &mesh = tm->GetParticleMesh();
        const unsigned int *tris = mesh.GetFaces().data();
        const unsigned int nFaces = mesh.NumFaces();
        for (unsigned int i = 0; i < nFaces; i++) {
            const unsigned int v1 = tris[3 * i] + offset;
            const unsigned int v2 = tris[3 * i + 1] + offset;
            const unsigned int v3 = tris[3 * i + 2] + offset;
            AddStrainTriangleConstraint(v1, v2, v3, xx_stiffness, yy_stiffness, xy_stiffness, normalize_stretch,
                                        normalize_shear);
        }
    } else if (cloth_method == 4) {
        const unsigned int offset = tm->GetIndexOffset();
        const unsigned int nEdges = tm->GetParticleMesh().NumEdges();
        const IndexedFaceMesh::Edge *edges = tm->GetParticleMesh().GetEdges().data();
        for (unsigned int i = 0; i < nEdges; i++) {
            const unsigned int v1 = edges[i].m_vert[0] + offset;
            const unsigned int v2 = edges[i].m_vert[1] + offset;

            AddDistanceConstraintXPBD(v1, v2, distance_stiffness);
        }
    }
}

void SimulationModel::AddBendingConstraints(const TriangleModel *tm, unsigned int bending_method, Real stiffness) {
    if ((bending_method < 1) || (bending_method > 3)) return;

    const unsigned int offset = tm->GetIndexOffset();
    const TriangleModel::ParticleMesh &mesh = tm->GetParticleMesh();
    unsigned int nEdges = mesh.NumEdges();
    const TriangleModel::ParticleMesh::Edge *edges = mesh.GetEdges().data();
    const unsigned int *tris = mesh.GetFaces().data();
    for (unsigned int i = 0; i < nEdges; i++) {
        const int tri1 = edges[i].m_face[0];
        const int tri2 = edges[i].m_face[1];
        if ((tri1 != 0xffffffff) && (tri2 != 0xffffffff)) {
            // Find the triangle points which do not lie on the axis
            const int axisPoint1 = edges[i].m_vert[0];
            const int axisPoint2 = edges[i].m_vert[1];
            int point1 = -1;
            int point2 = -1;
            for (int j = 0; j < 3; j++) {
                if ((tris[3 * tri1 + j] != axisPoint1) && (tris[3 * tri1 + j] != axisPoint2)) {
                    point1 = tris[3 * tri1 + j];
                    break;
                }
            }
            for (int j = 0; j < 3; j++) {
                if ((tris[3 * tri2 + j] != axisPoint1) && (tris[3 * tri2 + j] != axisPoint2)) {
                    point2 = tris[3 * tri2 + j];
                    break;
                }
            }
            if ((point1 != -1) && (point2 != -1)) {
                const unsigned int vertex1 = point1 + offset;
                const unsigned int vertex2 = point2 + offset;
                const unsigned int vertex3 = edges[i].m_vert[0] + offset;
                const unsigned int vertex4 = edges[i].m_vert[1] + offset;
                if (bending_method == 1)
                    AddDihedralConstraint(vertex1, vertex2, vertex3, vertex4, stiffness);
                else if (bending_method == 2)
                    AddIsometricBendingConstraint(vertex1, vertex2, vertex3, vertex4, stiffness);
                else if (bending_method == 3) {
                    AddIsometricBendingConstraintXpbd(vertex1, vertex2, vertex3, vertex4, stiffness);
                }
            }
        }
    }
}

void SimulationModel::AddSolidConstraints(const TetModel *tm,
                                          unsigned int solid_method,
                                          Real stiffness,
                                          Real poisson_ratio,
                                          Real volume_stiffness,
                                          bool normalize_stretch,
                                          bool normalize_shear) {
    const unsigned int nTets = tm->GetParticleMesh().NumTets();
    const unsigned int *tets = tm->GetParticleMesh().GetTets().data();
    const IndexedTetMesh::VerticesTets &vTets = tm->GetParticleMesh().GetVertexTets();
    const unsigned int offset = tm->GetIndexOffset();
    if (solid_method == 1) {
        const unsigned int nEdges = tm->GetParticleMesh().NumEdges();
        const IndexedTetMesh::Edge *edges = tm->GetParticleMesh().GetEdges().data();
        for (unsigned int i = 0; i < nEdges; i++) {
            const unsigned int v1 = edges[i].m_vert[0] + offset;
            const unsigned int v2 = edges[i].m_vert[1] + offset;

            AddDistanceConstraint(v1, v2, stiffness);
        }

        for (unsigned int i = 0; i < nTets; i++) {
            const unsigned int v1 = tets[4 * i] + offset;
            const unsigned int v2 = tets[4 * i + 1] + offset;
            const unsigned int v3 = tets[4 * i + 2] + offset;
            const unsigned int v4 = tets[4 * i + 3] + offset;

            AddVolumeConstraint(v1, v2, v3, v4, volume_stiffness);
        }
    } else if (solid_method == 2) {
        const TetModel::ParticleMesh &mesh = tm->GetParticleMesh();
        for (unsigned int i = 0; i < nTets; i++) {
            const unsigned int v1 = tets[4 * i] + offset;
            const unsigned int v2 = tets[4 * i + 1] + offset;
            const unsigned int v3 = tets[4 * i + 2] + offset;
            const unsigned int v4 = tets[4 * i + 3] + offset;

            AddFemTetConstraint(v1, v2, v3, v4, stiffness, poisson_ratio);
        }
    } else if (solid_method == 3) {
        const TetModel::ParticleMesh &mesh = tm->GetParticleMesh();
        for (unsigned int i = 0; i < nTets; i++) {
            const unsigned int v1 = tets[4 * i] + offset;
            const unsigned int v2 = tets[4 * i + 1] + offset;
            const unsigned int v3 = tets[4 * i + 2] + offset;
            const unsigned int v4 = tets[4 * i + 3] + offset;

            AddStrainTetConstraint(v1, v2, v3, v4, stiffness, stiffness, normalize_stretch, normalize_stretch);
        }
    } else if (solid_method == 4) {
        const TetModel::ParticleMesh &mesh = tm->GetParticleMesh();
        for (unsigned int i = 0; i < nTets; i++) {
            const unsigned int v[4] = {tets[4 * i] + offset, tets[4 * i + 1] + offset, tets[4 * i + 2] + offset,
                                       tets[4 * i + 3] + offset};
            // Important: Divide position correction by the number of clusters
            // which contain the vertex.
            const unsigned int nc[4] = {
                    (unsigned int)vTets[v[0] - offset].size(), (unsigned int)vTets[v[1] - offset].size(),
                    (unsigned int)vTets[v[2] - offset].size(), (unsigned int)vTets[v[3] - offset].size()};
            AddShapeMatchingConstraint(4, v, nc, stiffness);

            if (v[0] == 1005) std::cout << v[0] << ", " << v[1] << ", " << v[2] << ", " << v[3] << "chk\n";
            if (v[1] == 1005) std::cout << v[0] << ", " << v[1] << ", " << v[2] << ", " << v[3] << "chk\n";
            if (v[2] == 1005) std::cout << v[0] << ", " << v[1] << ", " << v[2] << ", " << v[3] << "chk\n";
            if (v[3] == 1005) std::cout << v[0] << ", " << v[1] << ", " << v[2] << ", " << v[3] << "chk\n";
        }
    } else if (solid_method == 5) {
        const unsigned int offset = tm->GetIndexOffset();
        const unsigned int nEdges = tm->GetParticleMesh().NumEdges();
        const IndexedTetMesh::Edge *edges = tm->GetParticleMesh().GetEdges().data();
        for (unsigned int i = 0; i < nEdges; i++) {
            const unsigned int v1 = edges[i].m_vert[0] + offset;
            const unsigned int v2 = edges[i].m_vert[1] + offset;

            AddDistanceConstraintXPBD(v1, v2, stiffness);
        }

        for (unsigned int i = 0; i < nTets; i++) {
            const unsigned int v1 = tets[4 * i] + offset;
            const unsigned int v2 = tets[4 * i + 1] + offset;
            const unsigned int v3 = tets[4 * i + 2] + offset;
            const unsigned int v4 = tets[4 * i + 3] + offset;

            AddVolumeConstraintXPBD(v1, v2, v3, v4, volume_stiffness);
        }
    }
}

}  // namespace vox::force