//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <memory>

#include "Discregrid/All"
#include "vox.force/common.h"
#include "vox.force/distance_field_collision_detection.h"

namespace vox::force {
/** Collision detection based on cubic signed distance fields.
 */
class CubicSDFCollisionDetection : public DistanceFieldCollisionDetection {
public:
    using Grid = vox::force::discregrid::CubicLagrangeDiscreteGrid;
    using GridPtr = std::shared_ptr<vox::force::discregrid::CubicLagrangeDiscreteGrid>;

    struct CubicSDFCollisionObject : public DistanceFieldCollisionDetection::DistanceFieldCollisionObject {
        std::string m_sdfFile;
        Vector3r m_scale;
        GridPtr m_sdf;
        static int TYPE_ID;

        CubicSDFCollisionObject();
        virtual ~CubicSDFCollisionObject();
        virtual int &getTypeId() const { return TYPE_ID; }
        virtual bool collisionTest(const Vector3r &x,
                                   const Real tolerance,
                                   Vector3r &cp,
                                   Vector3r &n,
                                   Real &dist,
                                   const Real maxDist = 0.0);
        virtual double distance(const Eigen::Vector3d &x, const Real tolerance);
    };

public:
    CubicSDFCollisionDetection();
    virtual ~CubicSDFCollisionDetection();

    virtual bool isDistanceFieldCollisionObject(CollisionObject *co) const;

    void addCubicSDFCollisionObject(const unsigned int bodyIndex,
                                    const unsigned int bodyType,
                                    const Vector3r *vertices,
                                    const unsigned int numVertices,
                                    const std::string &sdfFile,
                                    const Vector3r &scale,
                                    const bool testMesh = true,
                                    const bool invertSDF = false);
    void addCubicSDFCollisionObject(const unsigned int bodyIndex,
                                    const unsigned int bodyType,
                                    const Vector3r *vertices,
                                    const unsigned int numVertices,
                                    GridPtr sdf,
                                    const Vector3r &scale,
                                    const bool testMesh = true,
                                    const bool invertSDF = false);
};
}  // namespace vox::force