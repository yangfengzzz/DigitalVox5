//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "sprite_debug.h"
#include "shader/shader_manager.h"

namespace vox {
SpriteDebugMaterial::SpriteDebugMaterial(Device &device, bool is_spot_light) :
BaseMaterial(device) {
    set_is_transparent(true);
    set_blend_mode(BlendMode::ADDITIVE);
    
    vertex_source_ = ShaderManager::get_singleton().load_shader("base/spotlight_sprite_debug.vert");
    if (is_spot_light) {
        fragment_source_ = ShaderManager::get_singleton().load_shader("base/spotlight_sprite_debug.frag");
    } else {
        fragment_source_ = ShaderManager::get_singleton().load_shader("base/pointlight_sprite_debug.frag");
    }
}

}
