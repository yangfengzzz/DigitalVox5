//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "cluster_debug_material.h"
#include "shader/shader_manager.h"

namespace vox {
ClusterDebugMaterial::ClusterDebugMaterial(Device &device) :
BaseMaterial(device) {
    vertex_source_ = ShaderManager::get_singleton().load_shader("");
    fragment_source_ = ShaderManager::get_singleton().load_shader("");
}

}
