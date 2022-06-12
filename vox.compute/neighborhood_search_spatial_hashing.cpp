//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.compute/neighborhood_search_spatial_hashing.h"

namespace vox::compute {

NeighborhoodSearchSpatialHashing::NeighborhoodSearchSpatialHashing(const unsigned int num_particles,
                                                                   const Real radius,
                                                                   const unsigned int max_neighbors,
                                                                   const unsigned int max_particles_per_cell)
    : m_grid_map_(num_particles * 2) {
    m_cell_grid_size_ = radius;
    m_radius_2_ = radius * radius;
    m_num_particles_ = num_particles;
    m_max_particles_per_cell_ = max_particles_per_cell;
    m_max_neighbors_ = max_neighbors;

    m_num_neighbors_ = nullptr;
    m_neighbors_ = nullptr;

    if (num_particles != 0) {
        m_num_neighbors_ = new unsigned int[m_num_particles_];
        m_neighbors_ = new unsigned int *[m_num_particles_];
        for (unsigned int i = 0; i < m_num_particles_; i++) m_neighbors_[i] = new unsigned int[m_max_neighbors_];
    }

    m_current_timestamp_ = 0;
}

NeighborhoodSearchSpatialHashing::~NeighborhoodSearchSpatialHashing() { CleanUp(); }

void NeighborhoodSearchSpatialHashing::CleanUp() {
    for (unsigned int i = 0; i < m_num_particles_; i++) delete[] m_neighbors_[i];
    delete[] m_neighbors_;
    delete[] m_num_neighbors_;
    m_num_particles_ = 0;

    for (auto &iter : m_grid_map_) {
        NeighborhoodSearchSpatialHashing::HashEntry *entry = iter.second;
        delete entry;
        iter.second = nullptr;
    }
}

unsigned int **NeighborhoodSearchSpatialHashing::GetNeighbors() const { return m_neighbors_; }

unsigned int *NeighborhoodSearchSpatialHashing::GetNumNeighbors() const { return m_num_neighbors_; }

unsigned int NeighborhoodSearchSpatialHashing::GetNumParticles() const { return m_num_particles_; }

void NeighborhoodSearchSpatialHashing::SetRadius(Real radius) {
    m_cell_grid_size_ = radius;
    m_radius_2_ = radius * radius;
}

Real NeighborhoodSearchSpatialHashing::GetRadius() const { return sqrt(m_radius_2_); }

void NeighborhoodSearchSpatialHashing::Update() { m_current_timestamp_++; }

void NeighborhoodSearchSpatialHashing::NeighborhoodSearch(Vector3r *x) {
    const Real kFactor = static_cast<Real>(1.0) / m_cell_grid_size_;
    for (int i = 0; i < (int)m_num_particles_; i++) {
        const int kCellPos1 = NeighborhoodSearchSpatialHashing::Floor(x[i][0] * kFactor) + 1;
        const int kCellPos2 = NeighborhoodSearchSpatialHashing::Floor(x[i][1] * kFactor) + 1;
        const int kCellPos3 = NeighborhoodSearchSpatialHashing::Floor(x[i][2] * kFactor) + 1;
        NeighborhoodSearchCellPos cell_pos(kCellPos1, kCellPos2, kCellPos3);
        HashEntry *&entry = m_grid_map_[&cell_pos];

        if (entry != nullptr) {
            if (entry->timestamp != m_current_timestamp_) {
                entry->timestamp = m_current_timestamp_;
                entry->particle_indices.clear();
            }
        } else {
            auto *new_entry = new HashEntry();
            new_entry->particle_indices.reserve(m_max_particles_per_cell_);
            new_entry->timestamp = m_current_timestamp_;
            entry = new_entry;
        }
        entry->particle_indices.push_back(i);
    }

// loop over all 27 neighboring cells
#pragma omp parallel shared(x, kFactor) default(none)
    {
#pragma omp for schedule(static)
        for (int i = 0; i < (int)m_num_particles_; i++) {
            m_num_neighbors_[i] = 0;
            const int kCellPos1 = NeighborhoodSearchSpatialHashing::Floor(x[i][0] * kFactor);
            const int kCellPos2 = NeighborhoodSearchSpatialHashing::Floor(x[i][1] * kFactor);
            const int kCellPos3 = NeighborhoodSearchSpatialHashing::Floor(x[i][2] * kFactor);
            for (unsigned char j = 0; j < 3; j++) {
                for (unsigned char k = 0; k < 3; k++) {
                    for (unsigned char l = 0; l < 3; l++) {
                        NeighborhoodSearchCellPos cell_pos(kCellPos1 + j, kCellPos2 + k, kCellPos3 + l);
                        auto entry = m_grid_map_.find(&cell_pos);

                        if ((entry != m_grid_map_.end()) && (entry->second->timestamp == m_current_timestamp_)) {
                            for (unsigned int pi : entry->second->particle_indices) {
                                if (pi != i) {
                                    const Real kDist2 = (x[i] - x[pi]).squaredNorm();
                                    if (kDist2 < m_radius_2_) {
                                        if (m_num_neighbors_[i] < m_max_neighbors_)
                                            m_neighbors_[i][m_num_neighbors_[i]++] = pi;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

void NeighborhoodSearchSpatialHashing::NeighborhoodSearch(Vector3r *x,
                                                          unsigned int num_boundary_particles,
                                                          Vector3r *boundary_x) {
    const Real kFactor = static_cast<Real>(1.0) / m_cell_grid_size_;
    for (int i = 0; i < (int)m_num_particles_; i++) {
        const int kCellPos1 = NeighborhoodSearchSpatialHashing::Floor(x[i][0] * kFactor) + 1;
        const int kCellPos2 = NeighborhoodSearchSpatialHashing::Floor(x[i][1] * kFactor) + 1;
        const int kCellPos3 = NeighborhoodSearchSpatialHashing::Floor(x[i][2] * kFactor) + 1;
        NeighborhoodSearchCellPos cell_pos(kCellPos1, kCellPos2, kCellPos3);
        HashEntry *&entry = m_grid_map_[&cell_pos];

        if (entry != nullptr) {
            if (entry->timestamp != m_current_timestamp_) {
                entry->timestamp = m_current_timestamp_;
                entry->particle_indices.clear();
            }
        } else {
            auto *new_entry = new HashEntry();
            new_entry->particle_indices.reserve(m_max_particles_per_cell_);
            new_entry->timestamp = m_current_timestamp_;
            entry = new_entry;
        }
        entry->particle_indices.push_back(i);
    }

    for (int i = 0; i < (int)num_boundary_particles; i++) {
        const int kCellPos1 = NeighborhoodSearchSpatialHashing::Floor(boundary_x[i][0] * kFactor) + 1;
        const int kCellPos2 = NeighborhoodSearchSpatialHashing::Floor(boundary_x[i][1] * kFactor) + 1;
        const int kCellPos3 = NeighborhoodSearchSpatialHashing::Floor(boundary_x[i][2] * kFactor) + 1;
        NeighborhoodSearchCellPos cell_pos(kCellPos1, kCellPos2, kCellPos3);
        HashEntry *&entry = m_grid_map_[&cell_pos];

        if (entry != nullptr) {
            if (entry->timestamp != m_current_timestamp_) {
                entry->timestamp = m_current_timestamp_;
                entry->particle_indices.clear();
            }
        } else {
            auto *new_entry = new HashEntry();
            new_entry->particle_indices.reserve(m_max_particles_per_cell_);
            new_entry->timestamp = m_current_timestamp_;
            entry = new_entry;
        }
        entry->particle_indices.push_back(m_num_particles_ + i);
    }

// loop over all 27 neighboring cells
#pragma omp parallel shared(x, kFactor, boundary_x) default(none)
    {
#pragma omp for schedule(static)
        for (int i = 0; i < (int)m_num_particles_; i++) {
            m_num_neighbors_[i] = 0;
            const int kCellPos1 = NeighborhoodSearchSpatialHashing::Floor(x[i][0] * kFactor);
            const int kCellPos2 = NeighborhoodSearchSpatialHashing::Floor(x[i][1] * kFactor);
            const int kCellPos3 = NeighborhoodSearchSpatialHashing::Floor(x[i][2] * kFactor);
            for (unsigned char j = 0; j < 3; j++) {
                for (unsigned char k = 0; k < 3; k++) {
                    for (unsigned char l = 0; l < 3; l++) {
                        NeighborhoodSearchCellPos cell_pos(kCellPos1 + j, kCellPos2 + k, kCellPos3 + l);
                        auto entry = m_grid_map_.find(&cell_pos);

                        if ((entry != m_grid_map_.end()) && (entry->second->timestamp == m_current_timestamp_)) {
                            for (unsigned int pi : entry->second->particle_indices) {
                                if (pi != i) {
                                    Real dist_2;
                                    if (pi < m_num_particles_)
                                        dist_2 = (x[i] - x[pi]).squaredNorm();
                                    else
                                        dist_2 = (x[i] - boundary_x[pi - m_num_particles_]).squaredNorm();

                                    if (dist_2 < m_radius_2_) {
                                        if (m_num_neighbors_[i] < m_max_neighbors_)
                                            m_neighbors_[i][m_num_neighbors_[i]++] = pi;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

}  // namespace vox::compute
