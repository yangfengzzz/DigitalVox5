//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.math/vector3.h"
#include "vox.math/color.h"
#include "vox.render/script.h"
#include "vox.geometry/point_cloud.h"
#include "vox.visual/graphics/render_option.h"

namespace vox::visualization {

class SimpleShader : public Script {
public:
    bool BindPointCloud(const geometry::PointCloud & pointcloud,
                        const RenderOption &option);


private:
    std::vector<Vector3F> points_;
    std::vector<Color> colors_;
};

}  // namespace vox::visualization