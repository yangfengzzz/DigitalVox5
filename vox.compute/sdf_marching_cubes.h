//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.math/vector4.h"
#include "vox.math/matrix4x4.h"
#include "vox.render/core/command_buffer.h"

namespace vox::compute {
struct MarchingCubesUniformBuffer {
    Matrix4x4F mMW;
    Matrix4x4F mMWP;
    Vector4F cColor;
    Vector4F vLightDir;
    Vector4F g_Origin;
    
    float g_CellSize;
    int32_t g_NumCellsX;
    int32_t g_NumCellsY;
    int32_t g_NumCellsZ;
    
    int32_t g_MaxMarchingCubesVertices;
    float g_MarchingCubesIsolevel;
};

class SDFMarchingCubes {
public:
    // Draw the SDF using marching cubes for debug purpose
    void Draw();

    // Draw the grid
    void DrawGrid();

    // Update mesh by running marching cubes
    void Update(CommandBuffer& command_buffer);
};

}
