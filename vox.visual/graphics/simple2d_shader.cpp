//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "simple2d_shader.h"

namespace vox::visualization {
bool Simple2DShader::BindSelectionPolygon(const visualization::SelectionPolygon &polygon,
                                          Camera *camera,
                                          const RenderOptionWithEditing &option) {
    if (polygon.IsEmpty()) {
        LOGW("Binding failed with empty SelectionPolygon.")
    }
    size_t segment_num = polygon.polygon_.size() - 1;
    if (polygon.is_closed_) {
        segment_num++;
    }
    points_.resize(segment_num * 2);
    colors_.resize(segment_num * 2);
    for (size_t i = 0; i < segment_num; i++) {
    }
    if (polygon.is_closed_) {
        points_.resize(polygon.polygon_.size() * 2);
        colors_.resize(polygon.polygon_.size() * 2);
    } else {
        points_.resize(polygon.polygon_.size() * 2 - 2);
        colors_.resize(polygon.polygon_.size() * 2 - 2);
    }
    auto width = (float)camera->FramebufferWidth();
    auto height = (float)camera->FramebufferHeight();
    for (size_t i = 0; i < segment_num; i++) {
        size_t j = (i + 1) % polygon.polygon_.size();
        const auto &vi = polygon.polygon_[i];
        const auto &vj = polygon.polygon_[j];
        points_[i * 2] = Vector3F((float)(vi(0) / width * 2.0 - 1.0), (float)(vi(1) / height * 2.0 - 1.0), 0.0f);
        points_[i * 2 + 1] = Vector3F((float)(vj(0) / width * 2.0 - 1.0), (float)(vj(1) / height * 2.0 - 1.0), 0.0f);
        colors_[i * 2] = colors_[i * 2 + 1] = option.selection_polygon_boundary_color_;
    }

    return true;
}

}  // namespace vox::visualization