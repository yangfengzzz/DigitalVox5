//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.force/cubic_sdf_collision_detection.h"

#include <Eigen/Dense>
#include <utility>

#include "vox.force/id_factory.h"

namespace vox::force {

int CubicSDFCollisionDetection::CubicSDFCollisionObject::type_id = IDFactory::GetId();

CubicSDFCollisionDetection::CubicSDFCollisionDetection() : DistanceFieldCollisionDetection() {}

CubicSDFCollisionDetection::~CubicSDFCollisionDetection() = default;

bool CubicSDFCollisionDetection::IsDistanceFieldCollisionObject(CollisionObject *co) const {
    return DistanceFieldCollisionDetection::IsDistanceFieldCollisionObject(co) ||
           (co->GetTypeId() == CubicSDFCollisionDetection::CubicSDFCollisionObject::type_id);
}

void CubicSDFCollisionDetection::AddCubicSdfCollisionObject(unsigned int body_index,
                                                            unsigned int body_type,
                                                            const Vector3r *vertices,
                                                            unsigned int num_vertices,
                                                            const std::string &sdf_file,
                                                            const Vector3r &scale,
                                                            bool test_mesh,
                                                            bool invert_sdf) {
    auto *co = new CubicSDFCollisionDetection::CubicSDFCollisionObject();
    co->m_body_index = body_index;
    co->m_body_type = body_type;
    co->m_sdf_file = sdf_file;
    co->m_scale = scale;
    co->m_sdf = std::make_shared<Grid>(co->m_sdf_file);
    co->m_bvh.Init(vertices, num_vertices);
    co->m_bvh.Construct();
    co->m_test_mesh = test_mesh;
    co->m_invert_sdf = 1.0;
    if (invert_sdf) co->m_invert_sdf = -1.0;
    m_collision_objects_.push_back(co);
}

void CubicSDFCollisionDetection::AddCubicSdfCollisionObject(unsigned int body_index,
                                                            unsigned int body_type,
                                                            const Vector3r *vertices,
                                                            unsigned int num_vertices,
                                                            CubicSDFCollisionDetection::GridPtr sdf,
                                                            const Vector3r &scale,
                                                            bool test_mesh /*= true*/,
                                                            bool invert_sdf /*= false*/) {
    auto *co = new CubicSDFCollisionDetection::CubicSDFCollisionObject();
    co->m_body_index = body_index;
    co->m_body_type = body_type;
    co->m_sdf_file = "";
    co->m_scale = scale;
    co->m_sdf = std::move(sdf);
    co->m_bvh.Init(vertices, num_vertices);
    co->m_bvh.Construct();
    co->m_test_mesh = test_mesh;
    co->m_invert_sdf = 1.0;
    if (invert_sdf) co->m_invert_sdf = -1.0;
    m_collision_objects_.push_back(co);
}

CubicSDFCollisionDetection::CubicSDFCollisionObject::CubicSDFCollisionObject() = default;

CubicSDFCollisionDetection::CubicSDFCollisionObject::~CubicSDFCollisionObject() = default;

double CubicSDFCollisionDetection::CubicSDFCollisionObject::Distance(const Eigen::Vector3d &x, const Real tolerance) {
    const Eigen::Vector3d kScaledX = x.cwiseProduct(m_scale.template cast<double>().cwiseInverse());
    const double kDist = m_sdf->Interpolate(0, kScaledX);
    if (kDist == std::numeric_limits<double>::max()) return kDist;
    return m_invert_sdf * m_scale[0] * kDist - tolerance;
}

bool CubicSDFCollisionDetection::CubicSDFCollisionObject::CollisionTest(
        const Vector3r &x, Real tolerance, Vector3r &cp, Vector3r &n, Real &dist, Real max_dist) {
    const Vector3r kScaledX = x.cwiseProduct(m_scale.cwiseInverse());

    Eigen::Vector3d normal;
    double d = m_sdf->Interpolate(0, kScaledX.template cast<double>(), &normal);
    if (d == std::numeric_limits<Real>::max()) return false;
    dist = static_cast<Real>(m_invert_sdf * d - tolerance);

    normal = m_invert_sdf * normal;
    if (dist < max_dist) {
        normal.normalize();
        n = normal.template cast<Real>();

        cp = (kScaledX - dist * n);
        cp = cp.cwiseProduct(m_scale);

        return true;
    }
    return false;
}

}  // namespace vox::force