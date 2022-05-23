//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/mesh/sub_mesh.h"

namespace vox {
SubMesh::SubMesh(uint32_t start, uint32_t count) : start_(start), count_(count) {}

uint32_t SubMesh::Start() const { return start_; }

uint32_t SubMesh::Count() const { return count_; }

}  // namespace vox
