//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "plot.h"

#include <utility>

namespace vox::ui {
Plot::Plot(std::vector<float> p_data,
           float p_min_scale,
           float p_max_scale,
           const Vector2F &p_size,
           std::string p_overlay,
           std::string p_label) :
DataWidget(data_), data_(std::move(p_data)), min_scale_(p_min_scale), max_scale_(p_max_scale),
size_(p_size), overlay_(std::move(p_overlay)), label_(std::move(p_label)) {
}

}
