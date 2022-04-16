//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef DIGITALVOX_VOX_RENDER_MESH_BUFFER_MESH_H_
#define DIGITALVOX_VOX_RENDER_MESH_BUFFER_MESH_H_

#include "mesh.h"

namespace vox {
class BufferMesh : public Mesh {
public:
    /**
     * Set vertex layouts.
     * @param vertex_input_bindings - stride step size
     * @param vertex_input_attributes - Vertex attributes collection
     */
    void vertex_input_state(std::vector<VkVertexInputBindingDescription> vertex_input_bindings,
                            std::vector<VkVertexInputAttributeDescription> vertex_input_attributes);
    
private:
    std::vector<VkVertexInputBindingDescription> vertex_input_bindings_{};
    std::vector<VkVertexInputAttributeDescription> vertex_input_attributes_{};
};

}

#endif /* DIGITALVOX_VOX_RENDER_MESH_BUFFER_MESH_H_ */
