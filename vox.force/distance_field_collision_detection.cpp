//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.force/distance_field_collision_detection.h"

#include "omp.h"
#include "vox.force/id_factory.h"

namespace vox::force {

int DistanceFieldCollisionDetection::DistanceFieldCollisionBox::type_id = IDFactory::GetId();
int DistanceFieldCollisionDetection::DistanceFieldCollisionSphere::type_id = IDFactory::GetId();
int DistanceFieldCollisionDetection::DistanceFieldCollisionTorus::type_id = IDFactory::GetId();
int DistanceFieldCollisionDetection::DistanceFieldCollisionCylinder::type_id = IDFactory::GetId();
int DistanceFieldCollisionDetection::DistanceFieldCollisionHollowSphere::type_id = IDFactory::GetId();
int DistanceFieldCollisionDetection::DistanceFieldCollisionHollowBox::type_id = IDFactory::GetId();
int DistanceFieldCollisionDetection::DistanceFieldCollisionObjectWithoutGeometry::type_id = IDFactory::GetId();

DistanceFieldCollisionDetection::DistanceFieldCollisionDetection() : CollisionDetection() {}

DistanceFieldCollisionDetection::~DistanceFieldCollisionDetection() = default;

void DistanceFieldCollisionDetection::GetCollisionDetection(SimulationModel &model) {
    model.ResetContacts();
    const SimulationModel::RigidBodyVector &rigid_bodies = model.GetRigidBodies();
    const SimulationModel::TriangleModelVector &tri_models = model.GetTriangleModels();
    const SimulationModel::TetModelVector &tet_models = model.GetTetModels();
    const ParticleData &pd = model.GetParticles();

    std::vector<std::pair<unsigned int, unsigned int>> co_pairs;
    for (unsigned int i = 0; i < m_collision_objects_.size(); i++) {
        CollisionDetection::CollisionObject *co1 = m_collision_objects_[i];
        for (unsigned int k = 0; k < m_collision_objects_.size(); k++) {
            CollisionDetection::CollisionObject *co2 = m_collision_objects_[k];
            if ((i != k)) {
                // ToDo: self collisions for deformable
                co_pairs.emplace_back(i, k);
            }
        }
    }

    // omp_set_num_threads(1);
    std::vector<std::vector<ContactData>> contacts_mt;
#ifdef _DEBUG
    const unsigned int maxThreads = 1;
#else
    const unsigned int kMaxThreads = omp_get_max_threads();
#endif
    contacts_mt.resize(kMaxThreads);

#pragma omp parallel shared(model, tri_models, tet_models, pd, co_pairs, rigid_bodies, contacts_mt) default(none)
    {
// Update BVHs
#pragma omp for schedule(static)
        for (auto co : m_collision_objects_) {
            UpdateAabb(model, co);
            if (IsDistanceFieldCollisionObject(co)) {
                if (co->m_body_type == CollisionDetection::CollisionObject::triangle_model_collision_object_type) {
                    auto *sco = (DistanceFieldCollisionObject *)co;

                    TriangleModel *tm = tri_models[co->m_body_index];
                    const unsigned int kOffset = tm->GetIndexOffset();
                    const IndexedFaceMesh &mesh = tm->GetParticleMesh();
                    const unsigned int kNumVert = mesh.NumVertices();
                    sco->m_bvh.Init(&pd.GetPosition(kOffset), kNumVert);
                    sco->m_bvh.Update();
                } else if (co->m_body_type == CollisionDetection::CollisionObject::tet_model_collision_object_type) {
                    TetModel *tm = tet_models[co->m_body_index];
                    const unsigned int kOffset = tm->GetIndexOffset();
                    const IndexedTetMesh &mesh = tm->GetParticleMesh();
                    const unsigned int kNumVert = mesh.NumVertices();

                    auto *sco = (DistanceFieldCollisionObject *)co;
                    sco->m_bvh.Init(&pd.GetPosition(kOffset), kNumVert);
                    sco->m_bvh_tets.UpdateVertices(&pd.GetPosition(kOffset));
                    sco->m_bvh_tets_0.UpdateVertices(&pd.GetPosition(kOffset));

                    sco->m_bvh.Update();
                    sco->m_bvh_tets.Update();
                }
            }
        }

#pragma omp for schedule(static)
        for (auto &co_pair : co_pairs) {
            CollisionDetection::CollisionObject *co1 = m_collision_objects_[co_pair.first];
            CollisionDetection::CollisionObject *co2 = m_collision_objects_[co_pair.second];

            if (((co2->m_body_type != CollisionDetection::CollisionObject::rigid_body_collision_object_type) &&
                 (co2->m_body_type != CollisionDetection::CollisionObject::tet_model_collision_object_type)) ||
                !IsDistanceFieldCollisionObject(co1) || !IsDistanceFieldCollisionObject(co2) ||
                !AABB::Intersection(co1->m_aabb, co2->m_aabb))
                continue;

            if ((co1->m_body_type == CollisionDetection::CollisionObject::rigid_body_collision_object_type) &&
                (co2->m_body_type == CollisionDetection::CollisionObject::rigid_body_collision_object_type) &&
                ((DistanceFieldCollisionObject *)co1)->m_test_mesh) {
                RigidBody *rb1 = rigid_bodies[co1->m_body_index];
                RigidBody *rb2 = rigid_bodies[co2->m_body_index];
                const Real kRestitutionCoeff = rb1->GetRestitutionCoeff() * rb2->GetRestitutionCoeff();
                const Real kFrictionCoeff = rb1->GetFrictionCoeff() + rb2->GetFrictionCoeff();
                CollisionDetectionRigidBodies(rb1, (DistanceFieldCollisionObject *)co1, rb2,
                                              (DistanceFieldCollisionObject *)co2, kRestitutionCoeff, kFrictionCoeff,
                                              contacts_mt);
            } else if ((co1->m_body_type == CollisionDetection::CollisionObject::triangle_model_collision_object_type) &&
                       (co2->m_body_type == CollisionDetection::CollisionObject::rigid_body_collision_object_type) &&
                       ((DistanceFieldCollisionObject *)co1)->m_test_mesh) {
                TriangleModel *tm = tri_models[co1->m_body_index];
                RigidBody *rb2 = rigid_bodies[co2->m_body_index];
                const unsigned int kOffset = tm->GetIndexOffset();
                const IndexedFaceMesh &mesh = tm->GetParticleMesh();
                const unsigned int kNumVert = mesh.NumVertices();
                const Real kRestitutionCoeff = tm->GetRestitutionCoeff() * rb2->GetRestitutionCoeff();
                const Real kFrictionCoeff = tm->GetFrictionCoeff() + rb2->GetFrictionCoeff();
                CollisionDetectionRbSolid(pd, kOffset, kNumVert, (DistanceFieldCollisionObject *)co1, rb2,
                                          (DistanceFieldCollisionObject *)co2, kRestitutionCoeff, kFrictionCoeff,
                                          contacts_mt);
            } else if ((co1->m_body_type == CollisionDetection::CollisionObject::tet_model_collision_object_type) &&
                       (co2->m_body_type == CollisionDetection::CollisionObject::rigid_body_collision_object_type) &&
                       ((DistanceFieldCollisionObject *)co1)->m_test_mesh) {
                TetModel *tm = tet_models[co1->m_body_index];
                RigidBody *rb2 = rigid_bodies[co2->m_body_index];
                const unsigned int kOffset = tm->GetIndexOffset();
                const IndexedTetMesh &mesh = tm->GetParticleMesh();
                const unsigned int kNumVert = mesh.NumVertices();
                const Real kRestitutionCoeff = tm->GetRestitutionCoeff() * rb2->GetRestitutionCoeff();
                const Real kFrictionCoeff = tm->GetFrictionCoeff() + rb2->GetFrictionCoeff();
                CollisionDetectionRbSolid(pd, kOffset, kNumVert, (DistanceFieldCollisionObject *)co1, rb2,
                                          (DistanceFieldCollisionObject *)co2, kRestitutionCoeff, kFrictionCoeff,
                                          contacts_mt);
            } else if ((co1->m_body_type == CollisionDetection::CollisionObject::tet_model_collision_object_type) &&
                       (co2->m_body_type == CollisionDetection::CollisionObject::tet_model_collision_object_type) &&
                       ((DistanceFieldCollisionObject *)co1)->m_test_mesh) {
                TetModel *tm1 = tet_models[co1->m_body_index];
                TetModel *tm2 = tet_models[co2->m_body_index];
                const unsigned int kOffset = tm1->GetIndexOffset();
                const IndexedTetMesh &mesh = tm1->GetParticleMesh();
                const unsigned int kNumVert = mesh.NumVertices();
                const Real kRestitutionCoeff = tm1->GetRestitutionCoeff() * tm2->GetRestitutionCoeff();
                const Real kFrictionCoeff = tm1->GetFrictionCoeff() + tm2->GetFrictionCoeff();
                CollisionDetectionSolidSolid(pd, kOffset, kNumVert, (DistanceFieldCollisionObject *)co1, tm2,
                                             (DistanceFieldCollisionObject *)co2, kRestitutionCoeff, kFrictionCoeff,
                                             contacts_mt);
            }
        }
    }

    for (auto &i : contacts_mt) {
        for (auto &j : i) {
            if (j.m_type == 1)
                AddParticleRigidBodyContact(j.m_index_1, j.m_index_2, j.m_cp1, j.m_cp2, j.m_normal, j.m_dist,
                                            j.m_restitution, j.m_friction);
            else if (j.m_type == 0)
                AddRigidBodyContact(j.m_index_1, j.m_index_2, j.m_cp1, j.m_cp2, j.m_normal, j.m_dist, j.m_restitution,
                                    j.m_friction);
            else if (j.m_type == 2) {
                AddParticleSolidContact(j.m_index_1, j.m_index_2, j.m_element_index_2, j.m_bary2, j.m_cp1, j.m_cp2,
                                        j.m_normal, j.m_dist, j.m_restitution, j.m_friction);
            }
        }
    }
}

void DistanceFieldCollisionDetection::CollisionDetectionRigidBodies(
        RigidBody *rb1,
        DistanceFieldCollisionObject *co1,
        RigidBody *rb2,
        DistanceFieldCollisionObject *co2,
        Real restitution_coeff,
        Real friction_coeff,
        std::vector<std::vector<ContactData>> &contacts_mt) {
    if ((rb1->GetMass() == 0.0) && (rb2->GetMass() == 0.0)) return;

    const VertexData &vd = rb1->GetGeometry().GetVertexData();

    const Vector3r &com2 = rb2->GetPosition();

    // remove the rotation of the main axis transformation that is performed
    // to get a diagonal inertia tensor since the distance function is
    // evaluated in local coordinates
    //
    // transformation world to local:
    // p_local = R_initial^T ( R_MAT R^T (p_world - x) - x_initial + x_MAT)
    //
    // transformation local to:
    // p_world = R R_MAT^T (R_initial p_local + x_initial - x_MAT) + x
    //
    const Matrix3r &R = rb2->GetTransformationR();
    const Vector3r &v1 = rb2->GetTransformationV1();
    const Vector3r &v2 = rb2->GetTransformationV2();

    const PointCloudBSH &bvh = ((DistanceFieldCollisionDetection::DistanceFieldCollisionObject *)co1)->m_bvh;
    std::function<bool(unsigned int, unsigned int)> predicate = [&](unsigned int node_index, unsigned int depth) {
        const BoundingSphere &bs = bvh.GetHull(node_index);
        const Vector3r &sphere_x = bs.X();
        const Vector3r kSphereXw = rb1->GetRotation() * sphere_x + rb1->GetPosition();

        AlignedBox3r box3f;
        box3f.extend(co2->m_aabb.m_p_[0]);
        box3f.extend(co2->m_aabb.m_p_[1]);
        const Real kDist = box3f.exteriorDistance(kSphereXw);

        // Test if center of bounding sphere intersects AABB
        if (kDist < bs.R()) {
            // Test if distance of center of bounding sphere to collision object is smaller than the radius
            const Vector3r kX = R * (kSphereXw - com2) + v1;
            const double kDist2 = co2->Distance(kX.template cast<double>(), m_tolerance_);
            if (kDist2 == std::numeric_limits<double>::max()) return true;
            if (kDist2 < bs.R()) return true;
        }
        return false;
    };
    std::function<void(unsigned int, unsigned int)> cb = [&](unsigned int node_index, unsigned int depth) {
        auto const &node = bvh.GetNode(node_index);
        if (!node.IsLeaf()) return;

        for (auto i = node.begin; i < node.begin + node.n; ++i) {
            unsigned int index = bvh.GetEntity(i);
            const Vector3r &x_w = vd.GetPosition(index);
            const Vector3r kX = R * (x_w - com2) + v1;
            Vector3r cp, n;
            Real dist;
            if (co2->CollisionTest(kX, m_tolerance_, cp, n, dist)) {
                const Vector3r kCpW = R.transpose() * cp + v2;
                const Vector3r kNw = R.transpose() * n;

#ifdef _DEBUG
                int tid = 0;
#else
                int tid = omp_get_thread_num();
#endif

                contacts_mt[tid].push_back(
                        {0, co1->m_body_index, co2->m_body_index, x_w, kCpW, kNw, dist,
                                            restitution_coeff, friction_coeff});
            }
        }
    };
    bvh.TraverseDepthFirst(predicate, cb);
}

void DistanceFieldCollisionDetection::CollisionDetectionRbSolid(const ParticleData &pd,
                                                                unsigned int offset,
                                                                unsigned int num_vert,
                                                                DistanceFieldCollisionObject *co1,
                                                                RigidBody *rb2,
                                                                DistanceFieldCollisionObject *co2,
                                                                Real restitution_coeff,
                                                                Real friction_coeff,
                                                                std::vector<std::vector<ContactData>> &contacts_mt) {
    const Vector3r &com2 = rb2->GetPosition();

    // remove the rotation of the main axis transformation that is performed
    // to get a diagonal inertia tensor since the distance function is
    // evaluated in local coordinates
    //
    // transformation world to local:
    // p_local = R_initial^T ( R_MAT R^T (p_world - x) - x_initial + x_MAT)
    //
    // transformation local to:
    // p_world = R R_MAT^T (R_initial p_local + x_initial - x_MAT) + x
    //
    const Matrix3r &R = rb2->GetTransformationR();
    const Vector3r &v1 = rb2->GetTransformationV1();
    const Vector3r &v2 = rb2->GetTransformationV2();

    const PointCloudBSH &bvh = ((DistanceFieldCollisionDetection::DistanceFieldCollisionObject *)co1)->m_bvh;

    std::function<bool(unsigned int, unsigned int)> predicate = [&](unsigned int node_index, unsigned int depth) {
        const BoundingSphere &bs = bvh.GetHull(node_index);
        const Vector3r &sphere_x_w = bs.X();

        AlignedBox3r box3f;
        box3f.extend(co2->m_aabb.m_p_[0]);
        box3f.extend(co2->m_aabb.m_p_[1]);
        const Real kDist = box3f.exteriorDistance(sphere_x_w);

        // Test if center of bounding sphere intersects AABB
        if (kDist < bs.R()) {
            // Test if distance of center of bounding sphere to collision object is smaller than the radius
            const Vector3r kX = R * (sphere_x_w - com2) + v1;
            const double kDist2 = co2->Distance(kX.template cast<double>(), m_tolerance_);
            if (kDist2 == std::numeric_limits<double>::max()) return true;
            if (kDist2 < bs.R()) return true;
        }
        return false;
    };

    std::function<void(unsigned int, unsigned int)> cb = [&](unsigned int node_index, unsigned int depth) {
        auto const &node = bvh.GetNode(node_index);
        if (!node.IsLeaf()) return;

        for (auto i = node.begin; i < node.begin + node.n; ++i) {
            unsigned int index = bvh.GetEntity(i) + offset;
            const Vector3r &x_w = pd.GetPosition(index);
            const Vector3r kX = R * (x_w - com2) + v1;
            Vector3r cp, n;
            Real dist;
            if (co2->CollisionTest(kX, m_tolerance_, cp, n, dist)) {
                const Vector3r kCpW = R.transpose() * cp + v2;
                const Vector3r kNw = R.transpose() * n;

#ifdef _DEBUG
                int tid = 0;
#else
                int tid = omp_get_thread_num();
#endif
                contacts_mt[tid].push_back(
                        {1, index, co2->m_body_index, x_w, kCpW, kNw, dist, restitution_coeff, friction_coeff});
            }
        }
    };

    bvh.TraverseDepthFirst(predicate, cb);
}

void DistanceFieldCollisionDetection::CollisionDetectionSolidSolid(const ParticleData &pd,
                                                                   unsigned int offset,
                                                                   unsigned int num_vert,
                                                                   DistanceFieldCollisionObject *co1,
                                                                   TetModel *tm2,
                                                                   DistanceFieldCollisionObject *co2,
                                                                   Real restitution_coeff,
                                                                   Real friction_coeff,
                                                                   std::vector<std::vector<ContactData>> &contacts_mt) {
    const PointCloudBSH &bvh1 = ((DistanceFieldCollisionDetection::DistanceFieldCollisionObject *)co1)->m_bvh;
    const TetMeshBSH &bvh2 = ((DistanceFieldCollisionDetection::DistanceFieldCollisionObject *)co2)->m_bvh_tets;
    const unsigned int *indices = tm2->GetParticleMesh().GetTets().data();
    const unsigned int kOffset2 = tm2->GetIndexOffset();

    // callback function for BVH which is called if a leaf node in the point cloud BVH
    // has a collision with a leaf node in the tet BVH
    std::function<void(unsigned int, unsigned int)> cb = [&](unsigned int node_index1, unsigned int node_index2) {
        auto const &node1 = bvh1.GetNode(node_index1);
        auto const &node2 = bvh2.GetNode(node_index2);

        // loop over all primitives (points, tets) in the leaf nodes
        for (auto i = node1.begin; i < node1.begin + node1.n; ++i) {
            for (auto j = node2.begin; j < node2.begin + node2.n; ++j) {
                // Get sample point
                unsigned int index = bvh1.GetEntity(i) + offset;
                const Vector3r &x_w = pd.GetPosition(index);

                // Get tet
                const unsigned int kTetIndex = bvh2.GetEntity(j);
                const Vector3r &x0 = pd.GetPosition(indices[4 * kTetIndex] + kOffset2);
                const Vector3r &x1 = pd.GetPosition(indices[4 * kTetIndex + 1] + kOffset2);
                const Vector3r &x2 = pd.GetPosition(indices[4 * kTetIndex + 2] + kOffset2);
                const Vector3r &x3 = pd.GetPosition(indices[4 * kTetIndex + 3] + kOffset2);

                // Compute barycentric coordinates of point in tet
                Matrix3r A;
                A.col(0) = x1 - x0;
                A.col(1) = x2 - x0;
                A.col(2) = x3 - x0;
                Vector3r bary = A.inverse() * (x_w - x0);

                // check if point lies in tet using barycentric coordinates
                if ((bary[0] >= 0.0) && (bary[1] >= 0.0) && (bary[2] >= 0.0) && (bary[0] + bary[1] + bary[2] <= 1.0)) {
                    // use barycentric coordinates to determine position of the point in the reference space of the tet
                    const Vector3r &X0 = pd.GetPosition0(indices[4 * kTetIndex] + kOffset2);
                    const Vector3r &X1 = pd.GetPosition0(indices[4 * kTetIndex + 1] + kOffset2);
                    const Vector3r &X2 = pd.GetPosition0(indices[4 * kTetIndex + 2] + kOffset2);
                    const Vector3r &X3 = pd.GetPosition0(indices[4 * kTetIndex + 3] + kOffset2);

                    Matrix3r A0;
                    A0.col(0) = X1 - X0;
                    A0.col(1) = X2 - X0;
                    A0.col(2) = X3 - X0;

                    // point in reference space of the tet
                    const Vector3r kX = X0 + A0 * bary;

                    Vector3r cp_l, n_l;
                    Real dist;

                    // apply inverse initial transform to transform the point in the space of the
                    // signed distance field
                    const Vector3r kXl = (tm2->GetInitialR().transpose() * (kX - tm2->GetInitialX()));

                    // perform collision test with distance field to get closest point on surface
                    // if (co2->collisionTest(X_l, m_tolerance, cp_l, n_l, dist))
                    if (co2->CollisionTest(kXl, 0.0, cp_l, n_l, dist)) {
                        unsigned int cp_tet_index;
                        Vector3r cp_bary;

                        // transform the closest point on surface back to the reference space of the tet model
                        const Vector3r kCp0 = (tm2->GetInitialR() * cp_l + tm2->GetInitialX());

                        // find the tet which contains the resulting point
                        if (FindRefTetAt(pd, tm2, co2, kCp0, cp_tet_index, cp_bary)) {
                            // if we are in another tet, update matrix A
                            Vector3r cp_w;
                            if (cp_tet_index != kTetIndex) {
                                const Vector3r &x0 = pd.GetPosition(indices[4 * cp_tet_index] + kOffset2);
                                const Vector3r &x1 = pd.GetPosition(indices[4 * cp_tet_index + 1] + kOffset2);
                                const Vector3r &x2 = pd.GetPosition(indices[4 * cp_tet_index + 2] + kOffset2);
                                const Vector3r &x3 = pd.GetPosition(indices[4 * cp_tet_index + 3] + kOffset2);
                                A.col(0) = x1 - x0;
                                A.col(1) = x2 - x0;
                                A.col(2) = x3 - x0;

                                // compute world space contact point in body 2
                                cp_w = x0 + A * cp_bary;
                            } else
                                // compute world space contact point in body 2
                                cp_w = x0 + A * cp_bary;

#ifdef _DEBUG
                            int tid = 0;
#else
                            int tid = omp_get_thread_num();
#endif

                            Vector3r n_w = cp_w - x_w;

                            // normalize normal vector
                            const Real kDist = (x_w - cp_w).norm();
                            if (kDist > 1.0e-6) n_w /= kDist;

                            contacts_mt[tid].push_back({2, index, co2->m_body_index, x_w, cp_w, n_w, kDist,
                                                        restitution_coeff, friction_coeff, kTetIndex, cp_tet_index, bary,
                                                        cp_bary});
                        }
                    }
                }
            }
        }
    };

    BVHTest::Traverse(bvh1, bvh2, cb);
}

bool DistanceFieldCollisionDetection::IsDistanceFieldCollisionObject(CollisionObject *co) const {
    return (co->GetTypeId() == DistanceFieldCollisionDetection::DistanceFieldCollisionBox::type_id) ||
           (co->GetTypeId() == DistanceFieldCollisionDetection::DistanceFieldCollisionSphere::type_id) ||
           (co->GetTypeId() == DistanceFieldCollisionDetection::DistanceFieldCollisionTorus::type_id) ||
           (co->GetTypeId() == DistanceFieldCollisionDetection::DistanceFieldCollisionCylinder::type_id) ||
           (co->GetTypeId() == DistanceFieldCollisionDetection::DistanceFieldCollisionHollowSphere::type_id) ||
           (co->GetTypeId() == DistanceFieldCollisionDetection::DistanceFieldCollisionHollowBox::type_id) ||
           (co->GetTypeId() == DistanceFieldCollisionDetection::DistanceFieldCollisionObjectWithoutGeometry::type_id);
}

void DistanceFieldCollisionDetection::AddCollisionBox(unsigned int body_index,
                                                      unsigned int body_type,
                                                      const Vector3r *vertices,
                                                      unsigned int num_vertices,
                                                      const Vector3r &box,
                                                      bool test_mesh,
                                                      bool invert_sdf) {
    auto *cf = new DistanceFieldCollisionDetection::DistanceFieldCollisionBox();
    cf->m_body_index = body_index;
    cf->m_body_type = body_type;
    // distance function requires 0.5*box
    cf->m_box = 0.5 * box;
    cf->m_bvh.Init(vertices, num_vertices);
    cf->m_bvh.Construct();
    cf->m_test_mesh = test_mesh;
    if (invert_sdf) cf->m_invert_sdf = -1.0;
    m_collision_objects_.push_back(cf);
}

void DistanceFieldCollisionDetection::AddCollisionSphere(unsigned int body_index,
                                                         unsigned int body_type,
                                                         const Vector3r *vertices,
                                                         unsigned int num_vertices,
                                                         Real radius,
                                                         bool test_mesh,
                                                         bool invert_sdf) {
    auto *cs = new DistanceFieldCollisionDetection::DistanceFieldCollisionSphere();
    cs->m_body_index = body_index;
    cs->m_body_type = body_type;
    cs->m_radius = radius;
    cs->m_bvh.Init(vertices, num_vertices);
    cs->m_bvh.Construct();
    cs->m_test_mesh = test_mesh;
    if (invert_sdf) cs->m_invert_sdf = -1.0;
    m_collision_objects_.push_back(cs);
}

void DistanceFieldCollisionDetection::AddCollisionTorus(unsigned int body_index,
                                                        unsigned int body_type,
                                                        const Vector3r *vertices,
                                                        unsigned int num_vertices,
                                                        const Vector2r &radii,
                                                        bool test_mesh,
                                                        bool invert_sdf) {
    auto *ct = new DistanceFieldCollisionDetection::DistanceFieldCollisionTorus();
    ct->m_body_index = body_index;
    ct->m_body_type = body_type;
    ct->m_radii = radii;
    ct->m_bvh.Init(vertices, num_vertices);
    ct->m_bvh.Construct();
    ct->m_test_mesh = test_mesh;
    if (invert_sdf) ct->m_invert_sdf = -1.0;
    m_collision_objects_.push_back(ct);
}

void DistanceFieldCollisionDetection::AddCollisionCylinder(unsigned int body_index,
                                                           unsigned int body_type,
                                                           const Vector3r *vertices,
                                                           unsigned int num_vertices,
                                                           const Vector2r &dim,
                                                           bool test_mesh,
                                                           bool invert_sdf) {
    auto *ct = new DistanceFieldCollisionDetection::DistanceFieldCollisionCylinder();
    ct->m_body_index = body_index;
    ct->m_body_type = body_type;
    ct->m_dim = dim;
    // distance function uses height/2
    ct->m_dim[1] *= 0.5;
    ct->m_bvh.Init(vertices, num_vertices);
    ct->m_bvh.Construct();
    ct->m_test_mesh = test_mesh;
    if (invert_sdf) ct->m_invert_sdf = -1.0;
    m_collision_objects_.push_back(ct);
}

void DistanceFieldCollisionDetection::AddCollisionHollowSphere(unsigned int body_index,
                                                               unsigned int body_type,
                                                               const Vector3r *vertices,
                                                               unsigned int num_vertices,
                                                               Real radius,
                                                               Real thickness,
                                                               bool test_mesh,
                                                               bool invert_sdf) {
    auto *cs = new DistanceFieldCollisionDetection::DistanceFieldCollisionHollowSphere();
    cs->m_body_index = body_index;
    cs->m_body_type = body_type;
    cs->m_radius = radius;
    cs->m_thickness = thickness;
    cs->m_bvh.Init(vertices, num_vertices);
    cs->m_bvh.Construct();
    cs->m_test_mesh = test_mesh;
    if (invert_sdf) cs->m_invert_sdf = -1.0;
    m_collision_objects_.push_back(cs);
}

void DistanceFieldCollisionDetection::AddCollisionHollowBox(unsigned int body_index,
                                                            unsigned int body_type,
                                                            const Vector3r *vertices,
                                                            unsigned int num_vertices,
                                                            const Vector3r &box,
                                                            Real thickness,
                                                            bool test_mesh,
                                                            bool invert_sdf) {
    auto *cf = new DistanceFieldCollisionDetection::DistanceFieldCollisionHollowBox();
    cf->m_body_index = body_index;
    cf->m_body_type = body_type;
    // distance function requires 0.5*box
    cf->m_box = 0.5 * box;
    cf->m_thickness = thickness;
    cf->m_bvh.Init(vertices, num_vertices);
    cf->m_bvh.Construct();
    cf->m_test_mesh = test_mesh;
    if (invert_sdf) cf->m_invert_sdf = -1.0;
    m_collision_objects_.push_back(cf);
}

void DistanceFieldCollisionDetection::AddCollisionObjectWithoutGeometry(unsigned int body_index,
                                                                        unsigned int body_type,
                                                                        const Vector3r *vertices,
                                                                        unsigned int num_vertices,
                                                                        bool test_mesh) {
    auto *co = new DistanceFieldCollisionObjectWithoutGeometry();
    co->m_body_index = body_index;
    co->m_body_type = body_type;
    co->m_bvh.Init(vertices, num_vertices);
    co->m_bvh.Construct();
    co->m_test_mesh = test_mesh;
    co->m_invert_sdf = 1.0;
    m_collision_objects_.push_back(co);
}

double DistanceFieldCollisionDetection::DistanceFieldCollisionBox::Distance(const Eigen::Vector3d &x,
                                                                            const Real tolerance) {
    const Eigen::Vector3d kBoxD = m_box.template cast<double>();
    const Eigen::Vector3d &x_d = x.template cast<double>();
    const Eigen::Vector3d kD(fabs(x_d.x()) - kBoxD.x(), fabs(x_d.y()) - kBoxD.y(), fabs(x_d.z()) - kBoxD.z());
    const Eigen::Vector3d kMaxD(std::max(kD.x(), 0.0), std::max(kD.y(), 0.0), std::max(kD.z(), 0.0));
    return m_invert_sdf * (std::min(std::max(kD.x(), std::max(kD.y(), kD.z())), 0.0) + kMaxD.norm()) -
           static_cast<double>(tolerance);
}

double DistanceFieldCollisionDetection::DistanceFieldCollisionSphere::Distance(const Eigen::Vector3d &x,
                                                                               const Real tolerance) {
    const Eigen::Vector3d &d = x.template cast<double>();
    const double kDl = d.norm();
    return m_invert_sdf * (kDl - static_cast<double>(m_radius)) - static_cast<double>(tolerance);
}

bool DistanceFieldCollisionDetection::DistanceFieldCollisionSphere::CollisionTest(
        const Vector3r &x, const Real tolerance, Vector3r &cp, Vector3r &n, Real &dist, const Real max_dist) {
    const Vector3r &d = x;
    const Real kDl = d.norm();
    dist = m_invert_sdf * (kDl - m_radius) - tolerance;
    if (dist < max_dist) {
        if (kDl < 1.e-6)
            n.setZero();
        else
            n = m_invert_sdf * d / kDl;

        cp = ((m_radius + tolerance) * n);
        return true;
    }
    return false;
}

double DistanceFieldCollisionDetection::DistanceFieldCollisionTorus::Distance(const Eigen::Vector3d &x,
                                                                              const Real tolerance) {
    const Eigen::Vector2d kRadiiD = m_radii.template cast<double>();
    const Eigen::Vector2d kQ(Vector2r(x.x(), x.z()).norm() - kRadiiD.x(), x.y());
    return m_invert_sdf * (kQ.norm() - kRadiiD.y()) - tolerance;
}

double DistanceFieldCollisionDetection::DistanceFieldCollisionCylinder::Distance(const Eigen::Vector3d &x,
                                                                                 const Real tolerance) {
    const double kL = sqrt(x.x() * x.x() + x.z() * x.z());
    const Eigen::Vector2d kD = Eigen::Vector2d(fabs(kL), fabs(x.y())) - m_dim.template cast<double>();
    const Eigen::Vector2d kMaxD(std::max(kD.x(), 0.0), std::max(kD.y(), 0.0));
    return m_invert_sdf * (std::min(std::max(kD.x(), kD.y()), 0.0) + kMaxD.norm()) - static_cast<double>(tolerance);
}

double DistanceFieldCollisionDetection::DistanceFieldCollisionHollowSphere::Distance(const Eigen::Vector3d &x,
                                                                                     const Real tolerance) {
    const Eigen::Vector3d &d = x.template cast<double>();
    const double kDl = d.norm();
    return m_invert_sdf * (fabs(kDl - static_cast<double>(m_radius)) - static_cast<double>(m_thickness)) -
           static_cast<double>(tolerance);
}

bool DistanceFieldCollisionDetection::DistanceFieldCollisionHollowSphere::CollisionTest(
        const Vector3r &x, const Real tolerance, Vector3r &cp, Vector3r &n, Real &dist, const Real max_dist) {
    const Vector3r &d = x;
    const Real kDl = d.norm();
    dist = m_invert_sdf * (fabs(kDl - m_radius) - m_thickness) - tolerance;
    if (dist < max_dist) {
        if (kDl < 1.e-6)
            n.setZero();
        else if (kDl < m_radius)
            n = -m_invert_sdf * d / kDl;
        else
            n = m_invert_sdf * d / kDl;

        cp = x - dist * n;
        return true;
    }
    return false;
}

double DistanceFieldCollisionDetection::DistanceFieldCollisionHollowBox::Distance(const Eigen::Vector3d &x,
                                                                                  const Real tolerance) {
    const Eigen::Vector3d kBoxD = m_box.template cast<double>();
    const Eigen::Vector3d &x_d = x.template cast<double>();
    const Eigen::Vector3d kD = x_d.cwiseAbs() - kBoxD;
    const Eigen::Vector3d kMaxD = kD.cwiseMax(Eigen::Vector3d(0.0, 0.0, 0.0));
    return m_invert_sdf * (fabs(std::min(kD.maxCoeff(), 0.0) + kMaxD.norm()) - m_thickness) -
           static_cast<double>(tolerance);
}

void DistanceFieldCollisionDetection::DistanceFieldCollisionObject::ApproximateNormal(const Eigen::Vector3d &x,
                                                                                      Real tolerance,
                                                                                      Vector3r &n) {
    // approximate gradient
    double eps = 1.e-6;
    n.setZero();
    Eigen::Vector3d x_tmp = x;
    for (unsigned int j = 0; j < 3; j++) {
        x_tmp[j] += eps;

        double e_p, e_m;
        e_p = Distance(x_tmp, tolerance);
        x_tmp[j] = x[j] - eps;
        e_m = Distance(x_tmp, tolerance);
        x_tmp[j] = x[j];

        double res = (e_p - e_m) * (1.0 / (2.0 * eps));

        n[j] = static_cast<Real>(res);
    }

    const Real kNorm2 = n.squaredNorm();
    if (kNorm2 < 1.e-6)
        n.setZero();
    else
        n = n / sqrt(kNorm2);
}

bool DistanceFieldCollisionDetection::DistanceFieldCollisionObject::CollisionTest(
        const Vector3r &x, const Real tolerance, Vector3r &cp, Vector3r &n, Real &dist, const Real max_dist) {
    const Real kTd = static_cast<Real>(tolerance);
    dist = static_cast<Real>(Distance(x.template cast<double>(), kTd));
    if (dist < max_dist) {
        // approximate gradient
        const Eigen::Vector3d kXd = x.template cast<double>();

        ApproximateNormal(kXd, kTd, n);

        cp = (x - dist * n);
        return true;
    }
    return false;
}

void DistanceFieldCollisionDetection::DistanceFieldCollisionObject::InitTetBvh(const Vector3r *vertices,
                                                                               unsigned int num_vertices,
                                                                               const unsigned int *indices,
                                                                               unsigned int num_tets,
                                                                               Real tolerance) {
    if (m_body_type == CollisionDetection::CollisionObject::tet_model_collision_object_type) {
        m_bvh_tets.Init(vertices, num_vertices, indices, num_tets, tolerance);
        m_bvh_tets.Construct();

        // ToDo: copy constructor
        m_bvh_tets_0.Init(vertices, num_vertices, indices, num_tets, 0.0);
        m_bvh_tets_0.Construct();
    }
}

bool DistanceFieldCollisionDetection::FindRefTetAt(
        const ParticleData &pd,
        TetModel *tm,
        const DistanceFieldCollisionDetection::DistanceFieldCollisionObject *co,
        const Vector3r &X,
        unsigned int &tet_index,
        Vector3r &barycentric_coordinates) {
    const TetMeshBSH &bvh0 = ((DistanceFieldCollisionDetection::DistanceFieldCollisionObject *)co)->m_bvh_tets_0;
    const unsigned int *indices = tm->GetParticleMesh().GetTets().data();
    const unsigned int kOffset = tm->GetIndexOffset();
    std::vector<Vector3r> bary;
    std::vector<unsigned int> tets;
    bary.reserve(100);
    tets.reserve(100);

    std::function<bool(unsigned int, unsigned int)> predicate = [&](unsigned int node_index, unsigned int depth) {
        const BoundingSphere &bs = bvh0.GetHull(node_index);
        return bs.Contains(X);
    };
    std::function<void(unsigned int, unsigned int)> cb = [&](unsigned int node_index, unsigned int depth) {
        auto const &node = bvh0.GetNode(node_index);
        if (!node.IsLeaf()) return;

        for (auto i = node.begin; i < node.begin + node.n; ++i) {
            const unsigned int kTetIndex = bvh0.GetEntity(i);

            // use barycentric coordinates to determine position in reference space
            const Vector3r &X0 = pd.GetPosition0(indices[4 * kTetIndex] + kOffset);
            const Vector3r &X1 = pd.GetPosition0(indices[4 * kTetIndex + 1] + kOffset);
            const Vector3r &X2 = pd.GetPosition0(indices[4 * kTetIndex + 2] + kOffset);
            const Vector3r &X3 = pd.GetPosition0(indices[4 * kTetIndex + 3] + kOffset);

            // Compute barycentric coordinates of point in tet
            Matrix3r A;
            A.col(0) = X1 - X0;
            A.col(1) = X2 - X0;
            A.col(2) = X3 - X0;
            bary.emplace_back(A.inverse() * (X - X0));
            tets.push_back(kTetIndex);
        }
    };

    bvh0.TraverseDepthFirst(predicate, cb);

    if (bary.empty()) return false;

    // find best set of barycentric coordinates
    unsigned int index = 0;
    Real min_error = std::numeric_limits<Real>::max();
    for (unsigned int i = 0; i < bary.size(); i++) {
        // Determine if barycentric coordinates are negative and add distance to 0 as error
        Real error = std::max(static_cast<Real>(0.0), -bary[i][0]);
        error += std::max(static_cast<Real>(0.0), -bary[i][1]);
        error += std::max(static_cast<Real>(0.0), -bary[i][2]);

        // Determine if sum of barycentric coordinates is larger than one and add distance to 1 as error
        error += std::max(static_cast<Real>(0.0), bary[i][0] + bary[i][1] + bary[i][2] - static_cast<Real>(1.0));

        if (error < min_error) {
            min_error = error;
            index = i;
        }
    }
    barycentric_coordinates = bary[index];
    tet_index = tets[index];
    return true;
}

}  // namespace vox::force