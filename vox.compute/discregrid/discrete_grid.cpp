//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.compute/discregrid/discrete_grid.h"

using namespace Eigen;

namespace vox::compute::discregrid {

DiscreteGrid::MultiIndex DiscreteGrid::SingleToMultiIndex(unsigned int l) const {
    auto n01 = m_resolution_[0] * m_resolution_[1];
    auto k = l / n01;
    auto temp = l % n01;
    auto j = temp / m_resolution_[0];
    auto i = temp % m_resolution_[0];
    return {{i, j, k}};
}

unsigned int DiscreteGrid::MultiToSingleIndex(MultiIndex const& ijk) const {
    return m_resolution_[1] * m_resolution_[0] * ijk[2] + m_resolution_[0] * ijk[1] + ijk[0];
}

AlignedBox3d DiscreteGrid::Subdomain(MultiIndex const& ijk) const {
    auto origin = m_domain_.min() +
                  Map<Matrix<unsigned int, 3, 1> const>(ijk.data()).cast<double>().cwiseProduct(m_cell_size_);
    return {origin, origin + m_cell_size_};
}

AlignedBox3d DiscreteGrid::Subdomain(unsigned int l) const { return Subdomain(SingleToMultiIndex(l)); }

}  // namespace vox::compute::discregrid
