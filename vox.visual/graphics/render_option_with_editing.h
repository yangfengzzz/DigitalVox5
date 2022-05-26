//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.visual/graphics/render_option.h"

namespace vox::visualization {

class RenderOptionWithEditing : public RenderOption {
public:
    static const double picker_sphere_size_min_;
    static const double picker_sphere_size_max_;
    static const double picker_sphere_size_default_;

public:
    RenderOptionWithEditing() = default;
    ~RenderOptionWithEditing() override = default;

public:
    bool ConvertToJsonValue(Json::Value &value) const override;
    bool ConvertFromJsonValue(const Json::Value &value) override;
    void IncreaseSphereSize() {
        pointcloud_picker_sphere_size_ = std::min(pointcloud_picker_sphere_size_ * 2.0, picker_sphere_size_max_);
    }
    void DecreaseSphereSize() {
        pointcloud_picker_sphere_size_ = std::max(pointcloud_picker_sphere_size_ * 0.5, picker_sphere_size_min_);
    }

public:
    // Selection polygon
    Color selection_polygon_boundary_color_ = Color(0.3, 0.3, 0.3);
    Color selection_polygon_mask_color_ = Color(0.3, 0.3, 0.3);
    double selection_polygon_mask_alpha_ = 0.5;

    // PointCloud Picker
    double pointcloud_picker_sphere_size_ = picker_sphere_size_default_;
};

}  // namespace vox::visualization
