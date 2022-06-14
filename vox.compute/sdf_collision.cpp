//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "sdf_collision.h"

#include <utility>

namespace vox::compute {
SdfCollision::SdfCollision(Device& p_device,
                           std::shared_ptr<Mesh> p_coll_mesh,
                           const char* model_name,
                           int num_cells_in_x,
                           float collision_margin)
    : m_collision_margin_(collision_margin),
      m_num_cells_in_x_axis_(num_cells_in_x),
      m_grid_allocation_multiplier_(1.4f),
      m_num_total_cells_(INT_MAX) {
    m_p_input_collision_mesh_ = std::move(p_coll_mesh);

    // initialize SDF grid using the associated model's bounding box
    auto lower_corner = m_p_input_collision_mesh_->bounds_.lower_corner;
    auto upper_corner = m_p_input_collision_mesh_->bounds_.upper_corner;
    m_cell_size_ = (upper_corner.x - lower_corner.x) / (float)m_num_cells_in_x_axis_;
    int num_extra_padding_cells = (int)(0.8f * (float)m_num_cells_in_x_axis_);
    m_padding_boundary_ = {(float)num_extra_padding_cells * m_cell_size_, (float)num_extra_padding_cells * m_cell_size_,
                           (float)num_extra_padding_cells * m_cell_size_};

    UpdateSdfGrid(lower_corner, upper_corner);

    lower_corner -= m_padding_boundary_;
    upper_corner += m_padding_boundary_;

    m_num_cells_x_ = (int)((upper_corner.x - lower_corner.x) / m_cell_size_);
    m_num_cells_y_ = (int)((upper_corner.y - lower_corner.y) / m_cell_size_);
    m_num_cells_z_ = (int)((upper_corner.z - lower_corner.z) / m_cell_size_);
    m_num_total_cells_ = std::min(m_num_total_cells_, (int)((int)m_grid_allocation_multiplier_ * m_num_cells_x_ *
                                                            m_num_cells_y_ * m_num_cells_z_));
}

void SdfCollision::Update(CommandBuffer& command_buffer, SdfCollisionSystem& system) {}

void SdfCollision::UpdateSdfGrid(const Point3F& tight_bbox_min, const Point3F& tight_bbox_max) {
    m_origin_ = tight_bbox_min - m_padding_boundary_;
}

}  // namespace vox::compute
