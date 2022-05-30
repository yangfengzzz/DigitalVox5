//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.force/line_model.h"
#include "vox.force/position_based_rigid_body_dynamics.h"

namespace vox::force {

LineModel::LineModel() {
    m_restitution_coeff_ = static_cast<Real>(0.6);
    m_friction_coeff_ = static_cast<Real>(0.2);
}

LineModel::~LineModel() = default;

LineModel::Edges& LineModel::GetEdges() { return m_edges_; }

void LineModel::InitMesh(const unsigned int n_points,
                         const unsigned int n_quaternions,
                         const unsigned int index_offset,
                         const unsigned int index_offset_quaternions,
                         unsigned int* indices,
                         unsigned int* indices_quaternions) {
    m_n_points_ = n_points;
    m_n_quaternions_ = n_quaternions;
    m_index_offset_ = index_offset;
    m_index_offset_quaternions_ = index_offset_quaternions;

    m_edges_.resize(n_points - 1);

    for (unsigned int i = 0; i < n_points - 1; i++) {
        m_edges_[i] = OrientedEdge(indices[2 * i], indices[2 * i + 1], indices_quaternions[i]);
    }
}

unsigned int LineModel::GetIndexOffset() const { return m_index_offset_; }

unsigned LineModel::GetIndexOffsetQuaternions() const { return m_index_offset_quaternions_; }

}  // namespace vox::force