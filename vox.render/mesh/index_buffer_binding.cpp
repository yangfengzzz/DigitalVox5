//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/mesh/index_buffer_binding.h"

namespace vox {
IndexBufferBinding::IndexBufferBinding(core::Buffer&& buffer, VkIndexType index_type)
    : buffer_(std::move(buffer)), index_type_(index_type) {}

const core::Buffer& IndexBufferBinding::Buffer() const { return buffer_; }

VkIndexType IndexBufferBinding::IndexType() const { return index_type_; }

}  // namespace vox
