//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <Eigen/Core>

#include "geometry.h"

namespace arc::geometry {

/// \class Geometry2D
///
/// \brief The base geometry class for 2D geometries.
///
/// Main class for 2D geometries, Derives all data from Geometry Base class.
class Geometry2D : public Geometry {
public:
    ~Geometry2D() override = default;

protected:
    /// \brief Parameterized Constructor.
    ///
    /// \param type  type of object based on GeometryType
    explicit Geometry2D(GeometryType type) : Geometry(type, 2) {}

public:
    Geometry& Clear() override = 0;
    [[nodiscard]] bool IsEmpty() const override = 0;
    /// Returns min bounds for geometry coordinates.
    [[nodiscard]] virtual Eigen::Vector2d GetMinBound() const = 0;
    /// Returns max bounds for geometry coordinates.
    [[nodiscard]] virtual Eigen::Vector2d GetMaxBound() const = 0;
};

}  // namespace arc::geometry
