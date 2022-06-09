//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/wireframe/wireframe_manager.h"

namespace vox {

//-----------------------------------------------------------------------
WireframeManager *WireframeManager::GetSingletonPtr() { return ms_singleton; }

WireframeManager &WireframeManager::GetSingleton() {
    assert(ms_singleton);
    return (*ms_singleton);
}

}  // namespace vox
