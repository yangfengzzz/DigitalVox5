//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <stdio.h>

namespace vox {
class AssimpParser {
public:
    /**
     * Simply load meshes from a file using assimp
     * Return true on success
     */
    static bool load_model();
};

}
