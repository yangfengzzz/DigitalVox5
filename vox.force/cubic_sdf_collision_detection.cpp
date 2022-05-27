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

int CubicSDFCollisionDetection::CubicSDFCollisionObject::TYPE_ID = IDFactory::GetId();

CubicSDFCollisionDetection::CubicSDFCollisionDetection() : DistanceFieldCollisionDetection() {}

CubicSDFCollisionDetection::~CubicSDFCollisionDetection() = default;

bool CubicSDFCollisionDetection::isDistanceFieldCollisionObject(CollisionObject *co) const {
    return DistanceFieldCollisionDetection::isDistanceFieldCollisionObject(co) ||
           (co->getTypeId() == CubicSDFCollisionDetection::CubicSDFCollisionObject::TYPE_ID);
}

void CubicSDFCollisionDetection::addCubicSDFCollisionObject(const unsigned int bodyIndex,
                                                            const unsigned int bodyType,
                                                            const Vector3r *vertices,
                                                            const unsigned int numVertices,
                                                            const std::string &sdfFile,
                                                            const Vector3r &scale,
                                                            const bool testMesh,
                                                            const bool invertSDF) {
    auto *co = new CubicSDFCollisionDetection::CubicSDFCollisionObject();
    co->m_bodyIndex = bodyIndex;
    co->m_bodyType = bodyType;
    co->m_sdfFile = sdfFile;
    co->m_scale = scale;
    co->m_sdf = std::make_shared<Grid>(co->m_sdfFile);
    co->m_bvh.Init(vertices, numVertices);
    co->m_bvh.Construct();
    co->m_testMesh = testMesh;
    co->m_invertSDF = 1.0;
    if (invertSDF) co->m_invertSDF = -1.0;
    m_collisionObjects.push_back(co);
}

void CubicSDFCollisionDetection::addCubicSDFCollisionObject(const unsigned int bodyIndex,
                                                            const unsigned int bodyType,
                                                            const Vector3r *vertices,
                                                            const unsigned int numVertices,
                                                            CubicSDFCollisionDetection::GridPtr sdf,
                                                            const Vector3r &scale,
                                                            const bool testMesh /*= true*/,
                                                            const bool invertSDF /*= false*/) {
    auto *co = new CubicSDFCollisionDetection::CubicSDFCollisionObject();
    co->m_bodyIndex = bodyIndex;
    co->m_bodyType = bodyType;
    co->m_sdfFile = "";
    co->m_scale = scale;
    co->m_sdf = std::move(sdf);
    co->m_bvh.Init(vertices, numVertices);
    co->m_bvh.Construct();
    co->m_testMesh = testMesh;
    co->m_invertSDF = 1.0;
    if (invertSDF) co->m_invertSDF = -1.0;
    m_collisionObjects.push_back(co);
}

CubicSDFCollisionDetection::CubicSDFCollisionObject::CubicSDFCollisionObject() = default;

CubicSDFCollisionDetection::CubicSDFCollisionObject::~CubicSDFCollisionObject() = default;

double CubicSDFCollisionDetection::CubicSDFCollisionObject::distance(const Eigen::Vector3d &x, const Real tolerance) {
    const Eigen::Vector3d scaled_x = x.cwiseProduct(m_scale.template cast<double>().cwiseInverse());
    const double dist = m_sdf->interpolate(0, scaled_x);
    if (dist == std::numeric_limits<double>::max()) return dist;
    return m_invertSDF * m_scale[0] * dist - tolerance;
}

bool CubicSDFCollisionDetection::CubicSDFCollisionObject::collisionTest(
        const Vector3r &x, const Real tolerance, Vector3r &cp, Vector3r &n, Real &dist, const Real maxDist) {
    const Vector3r scaled_x = x.cwiseProduct(m_scale.cwiseInverse());

    Eigen::Vector3d normal;
    double d = m_sdf->interpolate(0, scaled_x.template cast<double>(), &normal);
    if (d == std::numeric_limits<Real>::max()) return false;
    dist = static_cast<Real>(m_invertSDF * d - tolerance);

    normal = m_invertSDF * normal;
    if (dist < maxDist) {
        normal.normalize();
        n = normal.template cast<Real>();

        cp = (scaled_x - dist * n);
        cp = cp.cwiseProduct(m_scale);

        return true;
    }
    return false;
}

}  // namespace vox::force