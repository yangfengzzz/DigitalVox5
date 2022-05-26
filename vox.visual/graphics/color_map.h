//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <memory>

#include "vox.math/color.h"

namespace vox::visualization {

class ColorMap {
public:
    enum class ColorMapOption {
        GRAY = 0,
        JET = 1,
        SUMMER = 2,
        WINTER = 3,
        HOT = 4,
    };

public:
    ColorMap() = default;
    virtual ~ColorMap() = default;

public:
    /// Function to get a color from a value in [0..1]
    [[nodiscard]] virtual Color GetColor(float value) const = 0;

protected:
    [[nodiscard]] static float Interpolate(float value, float y_0, float x_0, float y_1, float x_1) {
        if (value < x_0) return y_0;
        if (value > x_1) return y_1;
        return (value - x_0) * (y_1 - y_0) / (x_1 - x_0) + y_0;
    }
    [[nodiscard]] static Color Interpolate(float value, const Color &y_0, float x_0, const Color &y_1, float x_1) {
        if (value < x_0) return y_0;
        if (value > x_1) return y_1;
        return (value - x_0) * (y_1 - y_0) / (x_1 - x_0) + y_0;
    }
};

class ColorMapGray final : public ColorMap {
public:
    [[nodiscard]] Color GetColor(float value) const final;
};

/// See Matlab's Jet colormap
class ColorMapJet final : public ColorMap {
public:
    [[nodiscard]] Color GetColor(float value) const final;

protected:
    [[nodiscard]] static float JetBase(float value) {
        if (value <= -0.75) {
            return 0.0;
        } else if (value <= -0.25) {
            return Interpolate(value, 0.0, -0.75, 1.0, -0.25);
        } else if (value <= 0.25) {
            return 1.0;
        } else if (value <= 0.75) {
            return Interpolate(value, 1.0, 0.25, 0.0, 0.75);
        } else {
            return 0.0;
        }
    }
};

/// See Matlab's Summer colormap
class ColorMapSummer final : public ColorMap {
public:
    [[nodiscard]] Color GetColor(float value) const final;
};

/// See Matlab's Winter colormap
class ColorMapWinter final : public ColorMap {
public:
    [[nodiscard]] Color GetColor(float value) const final;
};

class ColorMapHot final : public ColorMap {
public:
    [[nodiscard]] Color GetColor(float value) const final;
};

/// Interface functions
std::shared_ptr<const ColorMap> GetGlobalColorMap();
void SetGlobalColorMap(ColorMap::ColorMapOption option);

}  // namespace vox::visualization
