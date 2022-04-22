//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "mesh_manager.h"

namespace vox {
MeshManager *MeshManager::get_singleton_ptr() {
    return ms_singleton_;
}

MeshManager &MeshManager::get_singleton() {
    assert(ms_singleton_);
    return (*ms_singleton_);
}

}
