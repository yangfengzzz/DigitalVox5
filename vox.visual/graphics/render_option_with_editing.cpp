//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.visual/graphics/render_option_with_editing.h"

#include <json/json.h>

namespace vox::visualization {

const double RenderOptionWithEditing::picker_sphere_size_min_ = 0.000625;
const double RenderOptionWithEditing::picker_sphere_size_max_ = 0.08;
const double RenderOptionWithEditing::picker_sphere_size_default_ = 0.01;

bool RenderOptionWithEditing::ConvertToJsonValue(Json::Value &value) const {
    if (!RenderOption::ConvertToJsonValue(value)) {
        return false;
    }
    value["selection_polygon_boundary_color"].clear();
    value["selection_polygon_boundary_color"].append(selection_polygon_boundary_color_.r);
    value["selection_polygon_boundary_color"].append(selection_polygon_boundary_color_.g);
    value["selection_polygon_boundary_color"].append(selection_polygon_boundary_color_.b);

    value["selection_polygon_mask_color"].clear();
    value["selection_polygon_mask_color"].append(selection_polygon_boundary_color_.r);
    value["selection_polygon_mask_color"].append(selection_polygon_boundary_color_.g);
    value["selection_polygon_mask_color"].append(selection_polygon_boundary_color_.b);

    value["selection_polygon_mask_alpha"] = selection_polygon_mask_alpha_;
    value["pointcloud_picker_sphere_size"] = pointcloud_picker_sphere_size_;
    return true;
}

bool RenderOptionWithEditing::ConvertFromJsonValue(const Json::Value &value) {
    if (!RenderOption::ConvertFromJsonValue(value)) {
        return false;
    }

    selection_polygon_boundary_color_.r = value["selection_polygon_boundary_color"][0].asFloat();
    selection_polygon_boundary_color_.g = value["selection_polygon_boundary_color"][1].asFloat();
    selection_polygon_boundary_color_.b = value["selection_polygon_boundary_color"][2].asFloat();

    selection_polygon_mask_color_.r = value["selection_polygon_mask_color"][0].asFloat();
    selection_polygon_mask_color_.g = value["selection_polygon_mask_color"][1].asFloat();
    selection_polygon_mask_color_.b = value["selection_polygon_mask_color"][2].asFloat();

    selection_polygon_mask_alpha_ = value.get("selection_polygon_mask_alpha", selection_polygon_mask_alpha_).asDouble();
    pointcloud_picker_sphere_size_ =
            value.get("pointcloud_picker_sphere_size", selection_polygon_mask_alpha_).asDouble();
    return true;
}

}  // namespace vox::visualization
