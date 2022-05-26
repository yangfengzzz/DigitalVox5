//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.visual/graphics/color_map.h"

#include <memory>

#include "vox.base/logging.h"

namespace vox {

namespace {
using namespace visualization;

class GlobalColorMapSingleton {
private:
    GlobalColorMapSingleton() : color_map_(new ColorMapJet) { LOGD("Global colormap init.") }

public:
    ~GlobalColorMapSingleton(){LOGD("Global colormap destruct.")}

    GlobalColorMapSingleton(const GlobalColorMapSingleton &) = delete;
    GlobalColorMapSingleton &operator=(const GlobalColorMapSingleton &) = delete;

public:
    static GlobalColorMapSingleton &GetInstance() {
        static GlobalColorMapSingleton singleton;
        return singleton;
    }

public:
    std::shared_ptr<const ColorMap> color_map_;
};

}  // unnamed namespace

namespace visualization {
Color ColorMapGray::GetColor(float value) const { return {value, value, value}; }

Color ColorMapJet::GetColor(float value) const {
    return {JetBase(value * 2.f - 1.5f),   // red
            JetBase(value * 2.f - 1.0f),   // green
            JetBase(value * 2.f - 0.5f)};  // blue
}

Color ColorMapSummer::GetColor(float value) const {
    return {Interpolate(value, 0.0, 0.0, 1.0, 1.0), Interpolate(value, 0.5, 0.0, 1.0, 1.0), 0.4};
}

Color ColorMapWinter::GetColor(float value) const {
    return {0.0, Interpolate(value, 0.0, 0.0, 1.0, 1.0), Interpolate(value, 1.0, 0.0, 0.5, 1.0)};
}

Color ColorMapHot::GetColor(float value) const {
    Color edges[4] = {
            Color(1.0, 1.0, 1.0),
            Color(1.0, 1.0, 0.0),
            Color(1.0, 0.0, 0.0),
            Color(0.0, 0.0, 0.0),
    };
    if (value < 0.0) {
        return edges[0];
    } else if (value < 1.0 / 3.0) {
        return Interpolate(value, edges[0], 0.0, edges[1], 1.0 / 3.0);
    } else if (value < 2.0 / 3.0) {
        return Interpolate(value, edges[1], 1.0 / 3.0, edges[2], 2.0 / 3.0);
    } else if (value < 1.0) {
        return Interpolate(value, edges[2], 2.0 / 3.0, edges[3], 1.0);
    } else {
        return edges[3];
    }
}

std::shared_ptr<const ColorMap> GetGlobalColorMap() { return GlobalColorMapSingleton::GetInstance().color_map_; }

void SetGlobalColorMap(ColorMap::ColorMapOption option) {
    switch (option) {
        case ColorMap::ColorMapOption::GRAY:
            GlobalColorMapSingleton::GetInstance().color_map_ = std::make_shared<ColorMapGray>();
            break;
        case ColorMap::ColorMapOption::SUMMER:
            GlobalColorMapSingleton::GetInstance().color_map_ = std::make_shared<ColorMapSummer>();
            break;
        case ColorMap::ColorMapOption::WINTER:
            GlobalColorMapSingleton::GetInstance().color_map_ = std::make_shared<ColorMapWinter>();
            break;
        case ColorMap::ColorMapOption::HOT:
            GlobalColorMapSingleton::GetInstance().color_map_ = std::make_shared<ColorMapHot>();
            break;
        case ColorMap::ColorMapOption::JET:
        default:
            GlobalColorMapSingleton::GetInstance().color_map_ = std::make_shared<ColorMapJet>();
            break;
    }
}

}  // namespace visualization
}  // namespace vox
