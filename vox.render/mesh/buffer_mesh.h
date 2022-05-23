//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.render/mesh/mesh.h"

namespace vox {
class BufferMesh : public Mesh {
public:
    void SetVertexBufferBinding(size_t index, core::Buffer *binding);

    [[nodiscard]] size_t VertexBufferCount() const override;

    [[nodiscard]] const core::Buffer *VertexBuffer(size_t index) const override;

private:
    std::vector<core::Buffer *> vertex_buffer_bindings_{};
};

}  // namespace vox
