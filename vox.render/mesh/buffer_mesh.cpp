//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "buffer_mesh.h"

#include <utility>
#include "logging.h"

namespace vox {
void BufferMesh::vertex_input_state(std::vector<VkVertexInputBindingDescription> vertex_input_bindings,
                                    std::vector<VkVertexInputAttributeDescription> vertex_input_attributes) {
    vertex_input_bindings_ = std::move(vertex_input_bindings);
    vertex_input_attributes_ = std::move(vertex_input_attributes);
    
    VkPipelineVertexInputStateCreateInfo vertex_input_state = initializers::pipeline_vertex_input_state_create_info();
    vertex_input_state.vertexBindingDescriptionCount = static_cast<uint32_t>(vertex_input_bindings_.size());
    vertex_input_state.pVertexBindingDescriptions = vertex_input_bindings_.data();
    vertex_input_state.vertexAttributeDescriptionCount = static_cast<uint32_t>(vertex_input_attributes_.size());
    vertex_input_state.pVertexAttributeDescriptions = vertex_input_attributes_.data();
    set_vertex_input_state(vertex_input_state);
}

}
