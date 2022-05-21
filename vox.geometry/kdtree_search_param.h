//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

namespace vox::geometry {

/// \class KDTreeSearchParam
///
/// \brief Base class for KDTree search parameters.
class KDTreeSearchParam {
public:
    /// \enum SearchType
    ///
    /// \brief Specifies the search type for the search.
    enum class SearchType {
        Knn = 0,
        Radius = 1,
        Hybrid = 2,
    };

public:
    virtual ~KDTreeSearchParam() = default;

protected:
    explicit KDTreeSearchParam(SearchType type) : search_type_(type) {}

public:
    /// Get the search type (KNN, Radius, Hybrid) for the search parameter.
    [[nodiscard]] SearchType GetSearchType() const { return search_type_; }

private:
    SearchType search_type_;
};

/// \class KDTreeSearchParamKNN
///
/// \brief KDTree search parameters for pure KNN search.
class KDTreeSearchParamKNN : public KDTreeSearchParam {
public:
    /// \brief Default Constructor.
    ///
    /// \param knn Specifies the knn neighbors that will searched. Default
    /// is 30.
    explicit KDTreeSearchParamKNN(int knn = 30) : KDTreeSearchParam(SearchType::Knn), knn_(knn) {}

public:
    /// Number of the neighbors that will be searched.
    int knn_;
};

/// \class KDTreeSearchParamRadius
///
/// \brief KDTree search parameters for pure radius search.
class KDTreeSearchParamRadius : public KDTreeSearchParam {
public:
    /// \brief Default Constructor.
    ///
    /// \param radius Specifies the radius of the search.
    explicit KDTreeSearchParamRadius(double radius) : KDTreeSearchParam(SearchType::Radius), radius_(radius) {}

public:
    /// Search radius.
    double radius_;
};

/// \class KDTreeSearchParamHybrid
///
/// \brief KDTree search parameters for hybrid KNN and radius search.
class KDTreeSearchParamHybrid : public KDTreeSearchParam {
public:
    /// \brief Default Constructor.
    ///
    /// \param radius Specifies the radius of the search.
    /// \param max_nn Specifies the max neighbors to be searched.
    KDTreeSearchParamHybrid(double radius, int max_nn)
        : KDTreeSearchParam(SearchType::Hybrid), radius_(radius), max_nn_(max_nn) {}

public:
    /// Search radius.
    double radius_;
    /// At maximum, max_nn neighbors will be searched.
    int max_nn_;
};

}  // namespace vox::geometry
