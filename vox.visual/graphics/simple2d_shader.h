//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.render/script.h"
#include "vox.visual/graphics/render_option.h"

namespace vox::visualization {
class Simple2DShader : public Script {
public:
    bool BindSelectionPolygon(const RenderOption &option);
};
}  // namespace vox::visualization
