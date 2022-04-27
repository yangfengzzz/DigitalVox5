//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "mesh.h"

namespace vox {
class BufferMesh : public Mesh {
public:
    void set_vertex_buffer_binding(size_t index, core::Buffer *binding);
    
    [[nodiscard]] size_t vertex_buffer_count() const override;
    
    [[nodiscard]] const core::Buffer *vertex_buffer(size_t index) const override;
    
private:
    std::vector<core::Buffer *> vertex_buffer_bindings_{};
};

}
