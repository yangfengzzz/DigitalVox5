//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <string>

#include "vox.math/bounding_box3.h"
#include "vox.render/mesh/index_buffer_binding.h"
#include "vox.render/mesh/sub_mesh.h"
#include "vox.render/rendering/pipeline_state.h"
#include "vox.render/update_flag_manager.h"
#include "vox.render/vk_initializers.h"

namespace vox {
class Mesh {
public:
    /** Name. */
    std::string name_;
    /** The bounding volume of the mesh. */
    BoundingBox3F bounds_ = BoundingBox3F();

    /**
     * Instanced count, disable instanced drawing when set zero.
     */
    [[nodiscard]] uint32_t InstanceCount() const;

    void SetInstanceCount(uint32_t value);

    /**
     * First sub-mesh. Rendered using the first material.
     */
    [[nodiscard]] const SubMesh *FirstSubMesh() const;

    /**
     * A collection of sub-mesh, each sub-mesh can be rendered with an independent material.
     */
    [[nodiscard]] const std::vector<SubMesh> &SubMeshes() const;

    /**
     * Add sub-mesh, each sub-mesh can correspond to an independent material.
     * @param sub_mesh - Start drawing offset, if the index buffer is set,
     * it means the offset in the index buffer, if not set, it means the offset in the vertex buffer
     */
    void AddSubMesh(SubMesh sub_mesh);

    /**
     * Add sub-mesh, each sub-mesh can correspond to an independent material.
     * @param start - Start drawing offset, if the index buffer is set,
     * it means the offset in the index buffer, if not set,
     * it means the offset in the vertex buffer
     * @param count - Drawing count, if the index buffer is set,
     * it means the count in the index buffer, if not set,
     * it means the count in the vertex buffer
     */
    void AddSubMesh(uint32_t start = 0, uint32_t count = 0);

    /**
     * Clear all sub-mesh.
     */
    void ClearSubMesh();

    /**
     * Register update flag, update flag will be true if the vertex element changes.
     * @returns Update flag
     */
    std::unique_ptr<UpdateFlag> RegisterUpdateFlag();

public:
    /**
     * Set vertex state.
     * @param vertex_input_bindings - stride step size
     * @param vertex_input_attributes - Vertex attributes collection
     */
    void SetVertexInputState(const std::vector<VkVertexInputBindingDescription> &vertex_input_bindings,
                             const std::vector<VkVertexInputAttributeDescription> &vertex_input_attributes);

    [[nodiscard]] const VertexInputState &VertexInputState() const;

    /**
     * Index buffer binding.
     */
    [[nodiscard]] const IndexBufferBinding *IndexBufferBinding() const;

    void SetIndexBufferBinding(std::unique_ptr<vox::IndexBufferBinding> &&binding);

    [[nodiscard]] virtual size_t VertexBufferCount() const = 0;

    [[nodiscard]] virtual const core::Buffer *VertexBuffer(size_t index) const = 0;

protected:
    uint32_t instance_count_ = 1;
    std::unique_ptr<vox::IndexBufferBinding> index_buffer_binding_{nullptr};
    vox::VertexInputState vertex_input_state_;

    std::vector<SubMesh> sub_meshes_{};
    UpdateFlagManager update_flag_manager_;
};

}  // namespace vox
