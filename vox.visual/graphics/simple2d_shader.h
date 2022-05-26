//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.render/camera.h"
#include "vox.render/script.h"
#include "vox.visual/graphics/render_option_with_editing.h"
#include "vox.visual/graphics/selection_polygon.h"

namespace vox::visualization {
class Simple2DShader : public Script {
public:
    bool BindSelectionPolygon(const visualization::SelectionPolygon &polygon,
                              Camera *camera,
                              const RenderOptionWithEditing &option);

private:
    std::vector<Vector3F> points_{};
    std::vector<Color> colors_{};
    MeshRenderer *renderer_{nullptr};
    bool is_dirty_ = false;
};
}  // namespace vox::visualization
