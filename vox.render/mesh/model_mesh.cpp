//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "model_mesh.h"
#include "../shader/shader_common.h"
#include "../core/device.h"

namespace vox {
bool ModelMesh::accessible() const {
    return accessible_;
}

size_t ModelMesh::vertex_count() const {
    return vertex_count_;
}

ModelMesh::ModelMesh(Device &device) :
device_(device) {
}

void ModelMesh::set_positions(const std::vector<Vector3F> &positions) {
    if (!accessible_) {
        assert(false && "Not allowed to access data while accessible is false.");
    }
    
    auto count = positions.size();
    positions_ = positions;
    vertex_change_flag_ |= ValueChanged::POSITION;
    
    if (vertex_count_ != count) {
        vertex_count_ = count;
    }
}

const std::vector<Vector3F> &ModelMesh::positions() {
    if (!accessible_) {
        assert(false && "Not allowed to access data while accessible is false.");
    }
    
    return positions_;
}

void ModelMesh::set_normals(const std::vector<Vector3F> &normals) {
    if (!accessible_) {
        assert(false && "Not allowed to access data while accessible is false.");
    }
    
    if (normals.size() != vertex_count_) {
        assert(false && "The array provided needs to be the same size as vertex count.");
    }
    
    vertex_change_flag_ |= ValueChanged::NORMAL;
    normals_ = normals;
}

const std::vector<Vector3F> &ModelMesh::normals() {
    if (!accessible_) {
        assert(false && "Not allowed to access data while accessible is false.");
    }
    return normals_;
}

void ModelMesh::set_colors(const std::vector<Color> &colors) {
    if (!accessible_) {
        assert(false && "Not allowed to access data while accessible is false.");
    }
    
    if (colors.size() != vertex_count_) {
        assert(false && "The array provided needs to be the same size as vertex count.");
    }
    
    vertex_change_flag_ |= ValueChanged::COLOR;
    colors_ = colors;
}

const std::vector<Color> &ModelMesh::colors() {
    if (!accessible_) {
        assert(false && "Not allowed to access data while accessible is false.");
    }
    return colors_;
}

void ModelMesh::set_tangents(const std::vector<Vector4F> &tangents) {
    if (!accessible_) {
        assert(false && "Not allowed to access data while accessible is false.");
    }
    
    if (tangents.size() != vertex_count_) {
        assert(false && "The array provided needs to be the same size as vertex count.");
    }
    
    vertex_change_flag_ |= ValueChanged::TANGENT;
    tangents_ = tangents;
}

const std::vector<Vector4F> &ModelMesh::tangents() {
    if (!accessible_) {
        assert(false && "Not allowed to access data while accessible is false.");
    }
    return tangents_;
}

void ModelMesh::set_u_vs(const std::vector<Vector2F> &uv, int channel_index) {
    if (!accessible_) {
        assert(false && "Not allowed to access data while accessible is false.");
    }
    
    if (uv.size() != vertex_count_) {
        assert(false && "The array provided needs to be the same size as vertex count.");
    }
    
    switch (channel_index) {
        case 0:vertex_change_flag_ |= ValueChanged::UV;
            uv_ = uv;
            break;
        case 1:vertex_change_flag_ |= ValueChanged::UV_1;
            uv_1_ = uv;
            break;
        case 2:vertex_change_flag_ |= ValueChanged::UV_2;
            uv_2_ = uv;
            break;
        case 3:vertex_change_flag_ |= ValueChanged::UV_3;
            uv_3_ = uv;
            break;
        case 4:vertex_change_flag_ |= ValueChanged::UV_4;
            uv_4_ = uv;
            break;
        case 5:vertex_change_flag_ |= ValueChanged::UV_5;
            uv_5_ = uv;
            break;
        case 6:vertex_change_flag_ |= ValueChanged::UV_6;
            uv_6_ = uv;
            break;
        case 7:vertex_change_flag_ |= ValueChanged::UV_7;
            uv_7_ = uv;
            break;
        default:assert(false && "The index of channel needs to be in range [0 - 7].");
    }
}

const std::vector<Vector2F> &ModelMesh::uvs(int channel_index) {
    if (!accessible_) {
        assert(false && "Not allowed to access data while accessible is false.");
    }
    switch (channel_index) {
        case 0:return uv_;
        case 1:return uv_1_;
        case 2:return uv_2_;
        case 3:return uv_3_;
        case 4:return uv_4_;
        case 5:return uv_5_;
        case 6:return uv_6_;
        case 7:return uv_7_;
        default:assert(false && "The index of channel needs to be in range [0 - 7].");
            throw std::exception();
    }
}

void ModelMesh::set_indices(const std::vector<uint32_t> &indices) {
    if (!accessible_) {
        assert(false && "Not allowed to access data while accessible is false.");
    }
    indices_type_ = VkIndexType::VK_INDEX_TYPE_UINT32;
    indices_32_ = indices;
}

void ModelMesh::set_indices(const std::vector<uint16_t> &indices) {
    if (!accessible_) {
        assert(false && "Not allowed to access data while accessible is false.");
    }
    indices_type_ = VkIndexType::VK_INDEX_TYPE_UINT16;
    indices_16_ = indices;
}

void ModelMesh::upload_data(bool no_longer_accessible) {
    if (!accessible_) {
        assert(false && "Not allowed to access data while accessible is false.");
    }
    
    auto vertex_input_state = update_vertex_layouts();
    set_vertex_input_state(vertex_input_state);
    vertex_change_flag_ = ValueChanged::ALL;
    
    auto vertex_float_count = element_count_ * vertex_count_;
    auto vertices = std::vector<float>(vertex_float_count);
    update_vertices(vertices);
    
    auto &queue = device_.get_queue_by_flags(VK_QUEUE_GRAPHICS_BIT, 0);
    
    auto &command_buffer = device_.request_command_buffer();
    
    command_buffer.begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
    
    core::Buffer stage_buffer{device_,
        vertices.size() * sizeof(float),
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VMA_MEMORY_USAGE_CPU_ONLY};
    
    stage_buffer.update(vertices.data(), vertices.size() * sizeof(float));
    
    auto new_vertex_buffer = std::make_unique<core::Buffer>(device_,
                                                            vertices.size() * sizeof(float),
                                                            VK_BUFFER_USAGE_TRANSFER_DST_BIT
                                                            | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                                                            VMA_MEMORY_USAGE_GPU_ONLY);
    
    command_buffer.copy_buffer(stage_buffer, *new_vertex_buffer, vertices.size() * sizeof(float));
    set_vertex_buffer_binding(0, std::move(new_vertex_buffer));
    
    if (indices_type_ == VkIndexType::VK_INDEX_TYPE_UINT16) {
        core::Buffer stage_buffer{device_,
            indices_16_.size() * sizeof(uint16_t),
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VMA_MEMORY_USAGE_CPU_ONLY};
        
        stage_buffer.update(indices_16_.data(), indices_16_.size() * sizeof(uint16_t));
        
        core::Buffer new_index_buffer{device_,
            indices_16_.size() * sizeof(uint16_t),
            VK_BUFFER_USAGE_TRANSFER_DST_BIT |
            VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
            VMA_MEMORY_USAGE_GPU_ONLY};
        
        command_buffer.copy_buffer(stage_buffer, new_index_buffer, indices_16_.size() * sizeof(uint16_t));
        set_index_buffer_binding(std::make_unique<IndexBufferBinding>(std::move(new_index_buffer), indices_type_));
    } else if (indices_type_ == VkIndexType::VK_INDEX_TYPE_UINT32) {
        core::Buffer stage_buffer{device_,
            indices_32_.size() * sizeof(uint32_t),
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VMA_MEMORY_USAGE_CPU_ONLY};
        
        stage_buffer.update(indices_32_.data(), indices_32_.size() * sizeof(uint32_t));
        
        core::Buffer new_index_buffer{device_,
            indices_32_.size() * sizeof(uint32_t),
            VK_BUFFER_USAGE_TRANSFER_DST_BIT |
            VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
            VMA_MEMORY_USAGE_GPU_ONLY};
        
        command_buffer.copy_buffer(stage_buffer, new_index_buffer, indices_32_.size() * sizeof(uint32_t));
        set_index_buffer_binding(std::make_unique<IndexBufferBinding>(std::move(new_index_buffer), indices_type_));
    }
    
    command_buffer.end();
    
    queue.submit(command_buffer, device_.request_fence());
    
    device_.get_fence_pool().wait();
    device_.get_fence_pool().reset();
    device_.get_command_pool().reset_pool();
    
    if (no_longer_accessible) {
        accessible_ = false;
        release_cache();
    }
}

VkPipelineVertexInputStateCreateInfo ModelMesh::update_vertex_layouts() {
    vertex_input_attributes_.resize(1);
    vertex_input_attributes_[0] = initializers::vertex_input_attribute_description(0, 0, VK_FORMAT_R32G32B32_SFLOAT, 0);
    
    uint32_t offset = 12;
    uint32_t element_count = 3;
    if (!normals_.empty()) {
        vertex_input_attributes_
            .push_back(initializers::vertex_input_attribute_description(0, (uint32_t)Attributes::NORMAL,
                                                                        VK_FORMAT_R32G32B32_SFLOAT, offset));
        offset += 12;
        element_count += 3;
    }
    if (!colors_.empty()) {
        vertex_input_attributes_
            .push_back(initializers::vertex_input_attribute_description(0,
                                                                        (uint32_t)Attributes::COLOR_0,
                                                                        VK_FORMAT_R32G32B32A32_SFLOAT,
                                                                        offset));
        offset += 16;
        element_count += 4;
    }
    if (!tangents_.empty()) {
        vertex_input_attributes_
            .push_back(initializers::vertex_input_attribute_description(0,
                                                                        (uint32_t)Attributes::TANGENT,
                                                                        VK_FORMAT_R32G32B32A32_SFLOAT,
                                                                        offset));
        offset += 16;
        element_count += 4;
    }
    if (!uv_.empty()) {
        vertex_input_attributes_
            .push_back(initializers::vertex_input_attribute_description(0, (uint32_t)Attributes::UV_0,
                                                                        VK_FORMAT_R32G32_SFLOAT, offset));
        offset += 8;
        element_count += 2;
    }
    if (!uv_1_.empty()) {
        vertex_input_attributes_
            .push_back(initializers::vertex_input_attribute_description(0, (uint32_t)Attributes::UV_1,
                                                                        VK_FORMAT_R32G32_SFLOAT, offset));
        offset += 8;
        element_count += 2;
    }
    if (!uv_2_.empty()) {
        vertex_input_attributes_
            .push_back(initializers::vertex_input_attribute_description(0, (uint32_t)Attributes::UV_2,
                                                                        VK_FORMAT_R32G32_SFLOAT, offset));
        offset += 8;
        element_count += 2;
    }
    if (!uv_3_.empty()) {
        vertex_input_attributes_
            .push_back(initializers::vertex_input_attribute_description(0, (uint32_t)Attributes::UV_3,
                                                                        VK_FORMAT_R32G32_SFLOAT, offset));
        offset += 8;
        element_count += 2;
    }
    if (!uv_4_.empty()) {
        vertex_input_attributes_
            .push_back(initializers::vertex_input_attribute_description(0, (uint32_t)Attributes::UV_4,
                                                                        VK_FORMAT_R32G32_SFLOAT, offset));
        offset += 8;
        element_count += 2;
    }
    if (!uv_5_.empty()) {
        vertex_input_attributes_
            .push_back(initializers::vertex_input_attribute_description(0, (uint32_t)Attributes::UV_5,
                                                                        VK_FORMAT_R32G32_SFLOAT, offset));
        offset += 8;
        element_count += 2;
    }
    if (!uv_6_.empty()) {
        vertex_input_attributes_
            .push_back(initializers::vertex_input_attribute_description(0, (uint32_t)Attributes::UV_6,
                                                                        VK_FORMAT_R32G32_SFLOAT, offset));
        offset += 8;
        element_count += 2;
    }
    if (!uv_7_.empty()) {
        vertex_input_attributes_
            .push_back(initializers::vertex_input_attribute_description(0, (uint32_t)Attributes::UV_7,
                                                                        VK_FORMAT_R32G32_SFLOAT, offset));
        offset += 8;
        element_count += 2;
    }
    
    vertex_input_bindings_.resize(1);
    vertex_input_bindings_[0] =
    vox::initializers::vertex_input_binding_description(0, element_count * 4, VK_VERTEX_INPUT_RATE_VERTEX);
    
    VkPipelineVertexInputStateCreateInfo
    vertex_input_state = vox::initializers::pipeline_vertex_input_state_create_info();
    vertex_input_state.vertexBindingDescriptionCount = static_cast<uint32_t>(vertex_input_bindings_.size());
    vertex_input_state.pVertexBindingDescriptions = vertex_input_bindings_.data();
    vertex_input_state.vertexAttributeDescriptionCount = static_cast<uint32_t>(vertex_input_attributes_.size());
    vertex_input_state.pVertexAttributeDescriptions = vertex_input_attributes_.data();
    
    element_count_ = element_count;
    return vertex_input_state;
}

void ModelMesh::update_vertices(std::vector<float> &vertices) {
    if ((vertex_change_flag_ & ValueChanged::POSITION) != 0) {
        for (size_t i = 0; i < vertex_count_; i++) {
            auto start = element_count_ * i;
            const auto &position = positions_[i];
            vertices[start] = position.x;
            vertices[start + 1] = position.y;
            vertices[start + 2] = position.z;
        }
    }
    
    size_t offset = 3;
    
    if (!normals_.empty()) {
        if ((vertex_change_flag_ & ValueChanged::NORMAL) != 0) {
            for (size_t i = 0; i < vertex_count_; i++) {
                auto start = element_count_ * i + offset;
                const auto &normal = normals_[i];
                vertices[start] = normal.x;
                vertices[start + 1] = normal.y;
                vertices[start + 2] = normal.z;
            }
        }
        offset += 3;
    }
    
    if (!colors_.empty()) {
        if ((vertex_change_flag_ & ValueChanged::COLOR) != 0) {
            for (size_t i = 0; i < vertex_count_; i++) {
                auto start = element_count_ * i + offset;
                const auto &color = colors_[i];
                vertices[start] = color.r;
                vertices[start + 1] = color.g;
                vertices[start + 2] = color.b;
                vertices[start + 3] = color.a;
            }
        }
        offset += 4;
    }
    
    if (!tangents_.empty()) {
        if ((vertex_change_flag_ & ValueChanged::TANGENT) != 0) {
            for (size_t i = 0; i < vertex_count_; i++) {
                auto start = element_count_ * i + offset;
                const auto &tangent = tangents_[i];
                vertices[start] = tangent.x;
                vertices[start + 1] = tangent.y;
                vertices[start + 2] = tangent.z;
            }
        }
        offset += 4;
    }
    if (!uv_.empty()) {
        if ((vertex_change_flag_ & ValueChanged::UV) != 0) {
            for (size_t i = 0; i < vertex_count_; i++) {
                auto start = element_count_ * i + offset;
                const auto &uv = uv_[i];
                vertices[start] = uv.x;
                vertices[start + 1] = uv.y;
            }
        }
        offset += 2;
    }
    if (!uv_1_.empty()) {
        if ((vertex_change_flag_ & ValueChanged::UV_1) != 0) {
            for (size_t i = 0; i < vertex_count_; i++) {
                auto start = element_count_ * i + offset;
                const auto &uv = uv_1_[i];
                vertices[start] = uv.x;
                vertices[start + 1] = uv.y;
            }
        }
        offset += 2;
    }
    if (!uv_2_.empty()) {
        if ((vertex_change_flag_ & ValueChanged::UV_2) != 0) {
            for (size_t i = 0; i < vertex_count_; i++) {
                auto start = element_count_ * i + offset;
                const auto &uv = uv_2_[i];
                vertices[start] = uv.x;
                vertices[start + 1] = uv.y;
                
            }
        }
        offset += 2;
    }
    if (!uv_3_.empty()) {
        if ((vertex_change_flag_ & ValueChanged::UV_3) != 0) {
            for (size_t i = 0; i < vertex_count_; i++) {
                auto start = element_count_ * i + offset;
                const auto &uv = uv_3_[i];
                vertices[start] = uv.x;
                vertices[start + 1] = uv.y;
            }
        }
        offset += 2;
    }
    if (!uv_4_.empty()) {
        if ((vertex_change_flag_ & ValueChanged::UV_4) != 0) {
            for (size_t i = 0; i < vertex_count_; i++) {
                auto start = element_count_ * i + offset;
                const auto &uv = uv_4_[i];
                vertices[start] = uv.x;
                vertices[start + 1] = uv.y;
            }
        }
        offset += 2;
    }
    if (!uv_5_.empty()) {
        if ((vertex_change_flag_ & ValueChanged::UV_5) != 0) {
            for (size_t i = 0; i < vertex_count_; i++) {
                auto start = element_count_ * i + offset;
                const auto &uv = uv_5_[i];
                vertices[start] = uv.x;
                vertices[start + 1] = uv.y;
            }
        }
        offset += 2;
    }
    if (!uv_6_.empty()) {
        if ((vertex_change_flag_ & ValueChanged::UV_6) != 0) {
            for (size_t i = 0; i < vertex_count_; i++) {
                auto start = element_count_ * i + offset;
                const auto &uv = uv_6_[i];
                vertices[start] = uv.x;
                vertices[start + 1] = uv.y;
            }
        }
        offset += 2;
    }
    if (!uv_7_.empty()) {
        if ((vertex_change_flag_ & ValueChanged::UV_7) != 0) {
            for (size_t i = 0; i < vertex_count_; i++) {
                auto start = element_count_ * i + offset;
                const auto &uv = uv_7_[i];
                vertices[start] = uv.x;
                vertices[start + 1] = uv.y;
            }
        }
        offset += 2;
    }
    
    vertex_change_flag_ = 0;
}

void ModelMesh::release_cache() {
    vertices_.clear();
    positions_.clear();
    tangents_.clear();
    normals_.clear();
    colors_.clear();
    uv_.clear();
    uv_1_.clear();
    uv_2_.clear();
    uv_3_.clear();
    uv_4_.clear();
    uv_5_.clear();
    uv_6_.clear();
    uv_7_.clear();
}

}