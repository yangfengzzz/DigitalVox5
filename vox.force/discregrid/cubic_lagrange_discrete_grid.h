//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.force/discregrid/discrete_grid.h"

namespace vox::force::discregrid {

class CubicLagrangeDiscreteGrid : public DiscreteGrid {
public:
    explicit CubicLagrangeDiscreteGrid(std::string const& filename);
    CubicLagrangeDiscreteGrid(Eigen::AlignedBox3d const& domain, std::array<unsigned int, 3> const& resolution);

    void Save(std::string const& filename) const override;
    void Load(std::string const& filename) override;

    unsigned int AddFunction(ContinuousFunction const& func,
                             bool verbose = false,
                             SamplePredicate const& pred = nullptr) override;

    [[nodiscard]] std::size_t NCells() const { return m_n_cells_; };
    double Interpolate(unsigned int field_id,
                       Eigen::Vector3d const& xi,
                       Eigen::Vector3d* gradient = nullptr) const override;

    /**
     * @brief Determines the shape functions for the discretization with ID field_id at point xi.
     *
     * @param field_id Discretization ID
     * @param x Location where the shape functions should be determined
     * @param cell cell of x
     * @param c0 vector required for the interpolation
     * @param N	shape functions for the cell of x
     * @param dN (Optional) derivatives of the shape functions, required to compute the gradient
     * @return Success of the function.
     */
    bool DetermineShapeFunctions(unsigned int field_id,
                                 Eigen::Vector3d const& x,
                                 std::array<unsigned int, 32>& cell,
                                 Eigen::Vector3d& c0,
                                 Eigen::Matrix<double, 32, 1>& N,
                                 Eigen::Matrix<double, 32, 3>* dN = nullptr) const override;

    /**
     * @brief Evaluates the given discretization with ID field_id at point xi.
     *
     * @param field_id Discretization ID
     * @param xi Location where the discrete function is evaluated
     * @param cell cell of xi
     * @param c0 vector required for the interpolation
     * @param N	shape functions for the cell of xi
     * @param gradient (Optional) if a pointer to a vector is passed the gradient of the discrete function will be
     * evaluated
     * @param dN (Optional) derivatives of the shape functions, required to compute the gradient
     * @return double Results of the evaluation of the discrete function at point xi
     */
    double Interpolate(unsigned int field_id,
                       Eigen::Vector3d const& xi,
                       const std::array<unsigned int, 32>& cell,
                       const Eigen::Vector3d& c0,
                       const Eigen::Matrix<double, 32, 1>& N,
                       Eigen::Vector3d* gradient = nullptr,
                       Eigen::Matrix<double, 32, 3>* dN = nullptr) const override;

    void ReduceField(unsigned int field_id, Predicate pred) override;

    void ForEachCell(unsigned int field_id,
                     std::function<void(unsigned int, Eigen::AlignedBox3d const&, unsigned int)> const& cb) const;

private:
    [[nodiscard]] Eigen::Vector3d IndexToNodePosition(unsigned int l) const;

private:
    std::vector<std::vector<double>> m_nodes_;
    std::vector<std::vector<std::array<unsigned int, 32>>> m_cells_;
    std::vector<std::vector<unsigned int>> m_cell_map_;
};

}  // namespace vox::force::discregrid