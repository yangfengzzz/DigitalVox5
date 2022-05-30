//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <vector>

#include "vox.force/common.h"
#include "vox.force/constraints.h"
#include "vox.force/indexed_face_mesh.h"
#include "vox.force/particle_data.h"
#include "vox.force/rigid_body.h"

namespace vox::force {
class LineModel {
    struct OrientedEdge {
        OrientedEdge() = default;
        OrientedEdge(unsigned int p_0, unsigned int p_1, unsigned int q_0) {
            m_vert[0] = p_0;
            m_vert[1] = p_1;
            m_quat = q_0;
        }
        unsigned int m_vert[2]{};
        unsigned int m_quat{};
    };

public:
    typedef std::vector<OrientedEdge> Edges;

    LineModel();
    virtual ~LineModel();

protected:
    /** offset which must be added to get the correct index in the particles array */
    unsigned int m_index_offset_{};
    /** offset which must be added to get the correct index in the quaternions array */
    unsigned int m_index_offset_quaternions_{};
    unsigned int m_n_points_{}, m_n_quaternions_{};
    Edges m_edges_;
    Real m_restitution_coeff_;
    Real m_friction_coeff_;

public:
    void UpdateConstraints();

    Edges& GetEdges();

    [[nodiscard]] unsigned int GetIndexOffset() const;
    [[nodiscard]] unsigned int GetIndexOffsetQuaternions() const;

    void InitMesh(unsigned int n_points,
                  unsigned int n_quaternions,
                  unsigned int index_offset,
                  unsigned int index_offset_quaternions,
                  unsigned int* indices,
                  unsigned int* indices_quaternions);

    [[nodiscard]] FORCE_INLINE Real GetRestitutionCoeff() const { return m_restitution_coeff_; }

    FORCE_INLINE void SetRestitutionCoeff(Real val) { m_restitution_coeff_ = val; }

    [[nodiscard]] FORCE_INLINE Real GetFrictionCoeff() const { return m_friction_coeff_; }

    FORCE_INLINE void SetFrictionCoeff(Real val) { m_friction_coeff_ = val; }
};
}  // namespace vox::force