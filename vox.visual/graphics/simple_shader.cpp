//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "simple_shader.h"

#include "vox.base/logging.h"
#include "vox.visual/graphics/color_map.h"
#include "vox.geometry/bounding_volume.h"

namespace vox::visualization {
bool SimpleShader::BindPointCloud(const geometry::PointCloud &pointcloud, const RenderOption &option) {
    if (!pointcloud.HasPoints()) {
        LOGW("Binding failed with empty pointcloud.")
        return false;
    }

    const ColorMap &global_color_map = *GetGlobalColorMap();
    points_.resize(pointcloud.points_.size());
    colors_.resize(pointcloud.points_.size());
    for (size_t i = 0; i < pointcloud.points_.size(); i++) {
        const auto &point = pointcloud.points_[i].cast<float>();
        points_[i] = Vector3F(point.x(), point.y(), point.z());

        Color color;
        switch (option.point_color_option_) {
            case RenderOption::PointColorOption::XCoordinate:
                color = global_color_map.GetColor((float)pointcloud.GetAxisAlignedBoundingBox().GetXPercentage(point(0)));
                break;
            case RenderOption::PointColorOption::YCoordinate:
                color = global_color_map.GetColor((float)pointcloud.GetAxisAlignedBoundingBox().GetYPercentage(point(0)));
                break;
            case RenderOption::PointColorOption::ZCoordinate:
                color = global_color_map.GetColor((float)pointcloud.GetAxisAlignedBoundingBox().GetZPercentage(point(0)));
                break;
            case RenderOption::PointColorOption::Color:
            case RenderOption::PointColorOption::Default:
            default:
                if (pointcloud.HasColors()) {
                    auto float_color = pointcloud.colors_[i].cast<float>();
                    color = Color(float_color.x(), float_color.y(), float_color.z());
                } else {
                    color = global_color_map.GetColor((float)pointcloud.GetAxisAlignedBoundingBox().GetZPercentage(point(0)));
                }
                break;
        }
        color.a = 1.0;
        colors_[i] = color;
    }
    return true;
}

}  // namespace vox::visualization