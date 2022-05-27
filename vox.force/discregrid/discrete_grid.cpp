//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.force/discregrid/discrete_grid.h"

using namespace Eigen;

namespace vox::force::discregrid {

DiscreteGrid::MultiIndex DiscreteGrid::singleToMultiIndex(unsigned int l) const {
    auto n01 = m_resolution[0] * m_resolution[1];
    auto k = l / n01;
    auto temp = l % n01;
    auto j = temp / m_resolution[0];
    auto i = temp % m_resolution[0];
    return {{i, j, k}};
}

unsigned int DiscreteGrid::multiToSingleIndex(MultiIndex const& ijk) const {
    return m_resolution[1] * m_resolution[0] * ijk[2] + m_resolution[0] * ijk[1] + ijk[0];
}

AlignedBox3d DiscreteGrid::subdomain(MultiIndex const& ijk) const {
    auto origin =
            m_domain.min() + Map<Matrix<unsigned int, 3, 1> const>(ijk.data()).cast<double>().cwiseProduct(m_cell_size);
    return {origin, origin + m_cell_size};
}

AlignedBox3d DiscreteGrid::subdomain(unsigned int l) const { return subdomain(singleToMultiIndex(l)); }

}  // namespace vox::force::discregrid