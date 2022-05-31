//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <memory>

#include "vox.force/common.h"
#include "vox.force/discregrid/cubic_lagrange_discrete_grid.h"
#include "vox.force/discregrid/triangle_mesh.h"
#include "vox.force/discregrid/triangle_mesh_distance.h"
#include "vox.force/distance_field_collision_detection.h"

namespace vox::force {
/**
 * Collision detection based on cubic signed distance fields.
 */
class CubicSDFCollisionDetection : public DistanceFieldCollisionDetection {
public:
    using Grid = vox::force::discregrid::CubicLagrangeDiscreteGrid;
    using GridPtr = std::shared_ptr<vox::force::discregrid::CubicLagrangeDiscreteGrid>;

    //MARK: -
    struct CubicSDFCollisionObject : public DistanceFieldCollisionDetection::DistanceFieldCollisionObject {
        std::string m_sdf_file;
        Vector3r m_scale;
        GridPtr m_sdf;
        static int type_id;

        CubicSDFCollisionObject();
        ~CubicSDFCollisionObject() override;
        [[nodiscard]] int &GetTypeId() const override { return type_id; }
        bool CollisionTest(
                const Vector3r &x, Real tolerance, Vector3r &cp, Vector3r &n, Real &dist, Real max_dist = 0.0) override;
        double Distance(const Eigen::Vector3d &x, Real tolerance) override;
    };

public:
    CubicSDFCollisionDetection();
    ~CubicSDFCollisionDetection() override;

    bool IsDistanceFieldCollisionObject(CollisionObject *co) const override;

    void AddCubicSdfCollisionObject(unsigned int body_index,
                                    unsigned int body_type,
                                    const Vector3r *vertices,
                                    unsigned int num_vertices,
                                    const std::string &sdf_file,
                                    const Vector3r &scale,
                                    bool test_mesh = true,
                                    bool invert_sdf = false);
    void AddCubicSdfCollisionObject(unsigned int body_index,
                                    unsigned int body_type,
                                    const Vector3r *vertices,
                                    unsigned int num_vertices,
                                    GridPtr sdf,
                                    const Vector3r &scale,
                                    bool test_mesh = true,
                                    bool invert_sdf = false);
};

}  // namespace vox::force
