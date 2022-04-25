//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "shadow_subpass.h"

namespace vox {
ShadowSubpass::ShadowSubpass(RenderContext &render_context, Scene *scene, Camera *camera) :
Subpass{render_context, scene, camera} {
}

}
