//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "plot_lines.h"
#include "ui/widgets/converter.h"

namespace vox::ui {
PlotLines::PlotLines(const std::vector<float> &p_data,
                     float p_min_scale,
                     float p_max_scale,
                     const Vector2F &p_size,
                     const std::string &p_overlay,
                     const std::string &p_label) :
Plot(p_data, p_min_scale, p_max_scale, p_size,
     p_overlay, p_label) {
}

void PlotLines::draw_impl() {
    ImGui::PlotLines((label_ + widget_id_).c_str(), data_.data(), static_cast<int>(data_.size()), 0,
                     overlay_.c_str(), min_scale_, max_scale_, Converter::to_imVec2(size_), sizeof(float));
}

}
