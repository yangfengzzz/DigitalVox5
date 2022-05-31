//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <array>
#include <Eigen/Dense>
#include <fstream>
#include <vector>

namespace vox::force::discregrid {

class DiscreteGrid {
public:
    using CoefficientVector = Eigen::Matrix<double, 32, 1>;
    using ContinuousFunction = std::function<double(Eigen::Vector3d const&)>;
    using MultiIndex = std::array<unsigned int, 3>;
    using Predicate = std::function<bool(Eigen::Vector3d const&, double)>;
    using SamplePredicate = std::function<bool(Eigen::Vector3d const&)>;

    DiscreteGrid() = default;
    DiscreteGrid(Eigen::AlignedBox3d const& domain, std::array<unsigned int, 3> const& resolution)
        : m_domain_(domain), m_resolution_(resolution), m_n_fields_(0u) {
        auto n = Eigen::Matrix<unsigned int, 3, 1>::Map(resolution.data());
        m_cell_size_ = domain.diagonal().cwiseQuotient(n.cast<double>());
        m_inv_cell_size_ = m_cell_size_.cwiseInverse();
        m_n_cells_ = n.prod();
    }
    virtual ~DiscreteGrid() = default;

    virtual void Save(std::string const& filename) const = 0;
    virtual void Load(std::string const& filename) = 0;

    virtual unsigned int AddFunction(ContinuousFunction const& func,
                                     bool verbose = false,
                                     SamplePredicate const& pred = nullptr) = 0;

    double Interpolate(Eigen::Vector3d const& xi, Eigen::Vector3d* gradient = nullptr) const {
        return Interpolate(0u, xi, gradient);
    }

    virtual double Interpolate(unsigned int field_id,
                               Eigen::Vector3d const& xi,
                               Eigen::Vector3d* gradient = nullptr) const = 0;

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
    virtual bool DetermineShapeFunctions(unsigned int field_id,
                                         Eigen::Vector3d const& x,
                                         std::array<unsigned int, 32>& cell,
                                         Eigen::Vector3d& c0,
                                         Eigen::Matrix<double, 32, 1>& N,
                                         Eigen::Matrix<double, 32, 3>* dN = nullptr) const = 0;

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
    virtual double Interpolate(unsigned int field_id,
                               Eigen::Vector3d const& xi,
                               const std::array<unsigned int, 32>& cell,
                               const Eigen::Vector3d& c0,
                               const Eigen::Matrix<double, 32, 1>& N,
                               Eigen::Vector3d* gradient = nullptr,
                               Eigen::Matrix<double, 32, 3>* dN = nullptr) const = 0;

    virtual void ReduceField(unsigned int field_id, Predicate pred) {}

    [[nodiscard]] MultiIndex SingleToMultiIndex(unsigned int i) const;
    [[nodiscard]] unsigned int MultiToSingleIndex(MultiIndex const& ijk) const;

    [[nodiscard]] Eigen::AlignedBox3d Subdomain(MultiIndex const& ijk) const;
    [[nodiscard]] Eigen::AlignedBox3d Subdomain(unsigned int l) const;

    [[nodiscard]] Eigen::AlignedBox3d const& Domain() const { return m_domain_; }
    [[nodiscard]] std::array<unsigned int, 3> const& Resolution() const { return m_resolution_; };
    [[nodiscard]] Eigen::Vector3d const& CellSize() const { return m_cell_size_; }
    [[nodiscard]] Eigen::Vector3d const& InvCellSize() const { return m_inv_cell_size_; }

protected:
    Eigen::AlignedBox3d m_domain_;
    std::array<unsigned int, 3> m_resolution_{};
    Eigen::Vector3d m_cell_size_;
    Eigen::Vector3d m_inv_cell_size_;
    std::size_t m_n_cells_{};
    std::size_t m_n_fields_{};
};
}  // namespace vox::force::discregrid