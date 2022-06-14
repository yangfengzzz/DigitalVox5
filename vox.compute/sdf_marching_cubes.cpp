//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.compute/sdf_marching_cubes.h"

#include "vox.compute/marching_cubes_tables.h"

namespace vox::compute {
SdfMarchingCubes::SdfMarchingCubes() : m_max_marching_cubes_vertices_(128 * 1024) {}

void SdfMarchingCubes::Draw() {}

void SdfMarchingCubes::DrawGrid() {}

void SdfMarchingCubes::Update(CommandBuffer& command_buffer) {}

}  // namespace vox::compute
