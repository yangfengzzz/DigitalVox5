//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <unordered_map>
#include <vector>

#include "vox.force/common.h"

typedef Eigen::Vector3i NeighborhoodSearchCellPos;

namespace std {
template <>
struct hash<NeighborhoodSearchCellPos *> {
    std::size_t operator()(NeighborhoodSearchCellPos *const &key) const {
        const int kP1 = 73856093 * (*key)[0];
        const int kP2 = 19349663 * (*key)[1];
        const int kP3 = 83492791 * (*key)[2];
        return kP1 + kP2 + kP3;
    }
};
}  // namespace std

namespace vox::force {

class NeighborhoodSearchSpatialHashing {
public:
    explicit NeighborhoodSearchSpatialHashing(unsigned int num_particles = 0,
                                              Real radius = 0.1,
                                              unsigned int max_neighbors = 60u,
                                              unsigned int max_particles_per_cell = 50u);
    ~NeighborhoodSearchSpatialHashing();

    // Spatial hashing
    struct HashEntry {
        HashEntry() = default;
        ;
        unsigned long timestamp{};
        std::vector<unsigned int> particle_indices;
    };

    FORCE_INLINE static int Floor(const Real v) {
        return (int)(v + 32768.f) - 32768;  // Shift to get positive values
    }

    void CleanUp();
    void NeighborhoodSearch(Vector3r *x);
    void NeighborhoodSearch(Vector3r *x, unsigned int num_boundary_particles, Vector3r *boundary_x);
    void Update();
    [[nodiscard]] unsigned int **GetNeighbors() const;
    [[nodiscard]] unsigned int *GetNumNeighbors() const;
    [[nodiscard]] unsigned int GetMaxNeighbors() const { return m_max_neighbors_; }

    [[nodiscard]] unsigned int GetNumParticles() const;
    void SetRadius(Real radius);
    [[nodiscard]] Real GetRadius() const;

    [[nodiscard]] FORCE_INLINE unsigned int NumOfNeighbors(unsigned int i) const { return m_num_neighbors_[i]; }
    [[nodiscard]] FORCE_INLINE unsigned int GetNeighbor(unsigned int i, unsigned int k) const {
        return m_neighbors_[i][k];
    }

private:
    unsigned int m_num_particles_;
    unsigned int m_max_neighbors_;
    unsigned int m_max_particles_per_cell_;
    unsigned int **m_neighbors_;
    unsigned int *m_num_neighbors_;
    Real m_cell_grid_size_;
    Real m_radius_2_;
    unsigned int m_current_timestamp_;
    std::unordered_map<NeighborhoodSearchCellPos *, HashEntry *> m_grid_map_;
};

}  // namespace vox::force
