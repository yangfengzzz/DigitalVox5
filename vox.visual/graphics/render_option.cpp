//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.visual/graphics/render_option.h"

#include <GL/glew.h>
#include <json/json.h>

#include <algorithm>

#include "vox.base/logging.h"

namespace vox::visualization {

bool RenderOption::ConvertToJsonValue(Json::Value &value) const {
    value["class_name"] = "RenderOption";
    value["version_major"] = 1;
    value["version_minor"] = 0;

    value["point_size"] = point_size_;
    value["point_color_option"] = (int)point_color_option_;
    value["point_show_normal"] = point_show_normal_;

    value["mesh_shade_option"] = (int)mesh_shade_option_;
    value["mesh_color_option"] = (int)mesh_color_option_;
    value["mesh_show_back_face"] = mesh_show_back_face_;
    value["mesh_show_wireframe"] = mesh_show_wireframe_;
    value["default_mesh_color"].clear();
    value["default_mesh_color"].append(default_mesh_color_.r);
    value["default_mesh_color"].append(default_mesh_color_.g);
    value["default_mesh_color"].append(default_mesh_color_.b);

    value["line_width"] = line_width_;

    value["image_stretch_option"] = (int)image_stretch_option_;
    value["image_max_depth"] = image_max_depth_;

    value["show_coordinate_frame"] = show_coordinate_frame_;
    return true;
}

bool RenderOption::ConvertFromJsonValue(const Json::Value &value) {
    if (!value.isObject()) {
        LOGW("ViewTrajectory read JSON failed: unsupported json format.")
        return false;
    }
    if (value.get("class_name", "").asString() != "RenderOption" || value.get("version_major", 1).asInt() != 1 ||
        value.get("version_minor", 0).asInt() != 0) {
        LOGW("ViewTrajectory read JSON failed: unsupported json format.")
        return false;
    }

    point_size_ = value.get("point_size", point_size_).asDouble();
    point_color_option_ = (PointColorOption)value.get("point_color_option", (int)point_color_option_).asInt();
    point_show_normal_ = value.get("point_show_normal", point_show_normal_).asBool();

    mesh_shade_option_ = (MeshShadeOption)value.get("mesh_shade_option", (int)mesh_shade_option_).asInt();
    mesh_color_option_ = (MeshColorOption)value.get("mesh_color_option", (int)mesh_color_option_).asInt();
    mesh_show_back_face_ = value.get("mesh_show_back_face", mesh_show_back_face_).asBool();
    mesh_show_wireframe_ = value.get("mesh_show_wireframe", mesh_show_wireframe_).asBool();
    default_mesh_color_.r = value["default_mesh_color"][0].asFloat();
    default_mesh_color_.g = value["default_mesh_color"][1].asFloat();
    default_mesh_color_.b = value["default_mesh_color"][2].asFloat();

    line_width_ = value.get("line_width", line_width_).asDouble();

    image_stretch_option_ = (ImageStretchOption)value.get("image_stretch_option", (int)image_stretch_option_).asInt();
    image_max_depth_ = value.get("image_max_depth", image_max_depth_).asInt();

    show_coordinate_frame_ = value.get("show_coordinate_frame", show_coordinate_frame_).asBool();
    return true;
}

void RenderOption::ChangePointSize(double change) {
    point_size_ = std::max(std::min(point_size_ + change * POINT_SIZE_STEP, POINT_SIZE_MAX), POINT_SIZE_MIN);
}

void RenderOption::SetPointSize(double size) { point_size_ = std::max(std::min(size, POINT_SIZE_MAX), POINT_SIZE_MIN); }

void RenderOption::ChangeLineWidth(double change) {
    line_width_ = std::max(std::min(line_width_ + change * LINE_WIDTH_STEP, LINE_WIDTH_MAX), LINE_WIDTH_MIN);
}

}  // namespace vox
