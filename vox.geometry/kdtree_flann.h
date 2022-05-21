//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <Eigen/Core>
#include <memory>
#include <vector>

#include "geometry.h"
#include "kdtree_search_param.h"
//#include "open3d/pipelines/registration/Feature.h"

/// @cond
namespace nanoflann {
struct metric_L2;
template <class MatrixType, int DIM, class Distance, bool row_major>
struct KDTreeEigenMatrixAdaptor;
}  // namespace nanoflann
/// @endcond

namespace vox::geometry {

/// \class KDTreeFlann
///
/// \brief KDTree with FLANN for nearest neighbor search.
class KDTreeFlann {
public:
    /// \brief Default Constructor.
    KDTreeFlann();

    /// \brief Parameterized Constructor.
    ///
    /// \param data Provides set of data points for KDTree construction.
    explicit KDTreeFlann(const Eigen::MatrixXd &data);

    /// \brief Parameterized Constructor.
    ///
    /// \param geometry Provides geometry from which KDTree is constructed.
    explicit KDTreeFlann(const Geometry &geometry);

    /// \brief Parameterized Constructor.
    ///
    /// \param feature Provides a set of features from which the KDTree is
    /// constructed.
    //    KDTreeFlann(const pipelines::registration::Feature &feature);

    ~KDTreeFlann();

    KDTreeFlann(const KDTreeFlann &) = delete;

    KDTreeFlann &operator=(const KDTreeFlann &) = delete;

public:
    /// Sets the data for the KDTree from a matrix.
    ///
    /// \param data Data points for KDTree Construction.
    bool SetMatrixData(const Eigen::MatrixXd &data);

    /// Sets the data for the KDTree from geometry.
    ///
    /// \param geometry Geometry for KDTree Construction.
    bool SetGeometry(const Geometry &geometry);

    /// Sets the data for the KDTree from the feature data.
    ///
    /// \param feature Set of features for KDTree construction.
    //    bool SetFeature(const pipelines::registration::Feature &feature);

    template <typename T>
    int Search(const T &query,
               const KDTreeSearchParam &param,
               std::vector<int> &indices,
               std::vector<double> &distance2) const;

    template <typename T>
    int SearchKNN(const T &query, int knn, std::vector<int> &indices, std::vector<double> &distance2) const;

    template <typename T>
    int SearchRadius(const T &query, double radius, std::vector<int> &indices, std::vector<double> &distance2) const;

    template <typename T>
    int SearchHybrid(
            const T &query, double radius, int max_nn, std::vector<int> &indices, std::vector<double> &distance2) const;

private:
    /// \brief Sets the KDTree data from the data provided by the other methods.
    ///
    /// Internal method that sets all the members of KDTree by data provided by
    /// features, geometry, etc.
    bool SetRawData(const Eigen::Map<const Eigen::MatrixXd> &data);

protected:
    using KDTree_t =
            nanoflann::KDTreeEigenMatrixAdaptor<Eigen::Map<const Eigen::MatrixXd>, -1, nanoflann::metric_L2, false>;

    std::vector<double> data_;
    std::unique_ptr<Eigen::Map<const Eigen::MatrixXd>> data_interface_;
    std::unique_ptr<KDTree_t> nanoflann_index_;
    size_t dimension_ = 0;
    size_t dataset_size_ = 0;
};

}  // namespace vox::geometry
