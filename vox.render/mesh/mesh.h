//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "sub_mesh.h"
#include "index_buffer_binding.h"
#include "bounding_box3.h"
#include "update_flag_manager.h"
#include "../vk_initializers.h"
#include "rendering/pipeline_state.h"
#include <string>

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
    [[nodiscard]] uint32_t instance_count() const;
    
    void set_instance_count(uint32_t value);
    
    /**
     * First sub-mesh. Rendered using the first material.
     */
    [[nodiscard]] const SubMesh *sub_mesh() const;
    
    /**
     * A collection of sub-mesh, each sub-mesh can be rendered with an independent material.
     */
    [[nodiscard]] const std::vector<SubMesh> &sub_meshes() const;
    
    /**
     * Add sub-mesh, each sub-mesh can correspond to an independent material.
     * @param sub_mesh - Start drawing offset, if the index buffer is set,
     * it means the offset in the index buffer, if not set, it means the offset in the vertex buffer
     */
    void add_sub_mesh(SubMesh sub_mesh);
    
    /**
     * Add sub-mesh, each sub-mesh can correspond to an independent material.
     * @param start - Start drawing offset, if the index buffer is set,
     * it means the offset in the index buffer, if not set,
     * it means the offset in the vertex buffer
     * @param count - Drawing count, if the index buffer is set,
     * it means the count in the index buffer, if not set,
     * it means the count in the vertex buffer
     */
    void add_sub_mesh(uint32_t start = 0, uint32_t count = 0);
    
    /**
     * Clear all sub-mesh.
     */
    void clear_sub_mesh();
    
    /**
     * Register update flag, update flag will be true if the vertex element changes.
     * @returns Update flag
     */
    std::unique_ptr<UpdateFlag> register_update_flag();
    
public:
    /**
     * Set vertex state.
     * @param vertex_input_bindings - stride step size
     * @param vertex_input_attributes - Vertex attributes collection
     */
    void set_vertex_input_state(const std::vector<VkVertexInputBindingDescription> &vertex_input_bindings,
                                const std::vector<VkVertexInputAttributeDescription> &vertex_input_attributes);
    
    const VertexInputState &vertex_input_state() const;
    
    /**
     * Vertex buffer binding collection.
     */
    [[nodiscard]] const std::vector<std::unique_ptr<core::Buffer>> &vertex_buffer_bindings() const;
    
    void set_vertex_buffer_binding(size_t index, std::unique_ptr<core::Buffer> &&binding);
    
    /**
     * Index buffer binding.
     */
    [[nodiscard]] const IndexBufferBinding &index_buffer_binding() const;
    
    void set_index_buffer_binding(std::unique_ptr<IndexBufferBinding> &&binding);
    
protected:
    uint32_t instance_count_ = 1;
    std::vector<std::unique_ptr<core::Buffer>> vertex_buffer_bindings_{};
    std::unique_ptr<IndexBufferBinding> index_buffer_binding_{nullptr};
    VertexInputState vertex_input_state_;
    
    std::vector<SubMesh> sub_meshes_{};
    UpdateFlagManager update_flag_manager_;
};

}
