//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef DIGITALVOX_VOX_RENDER_MESH_INDEX_BUFFER_BINDING_H_
#define DIGITALVOX_VOX_RENDER_MESH_INDEX_BUFFER_BINDING_H_

#include "core/buffer.h"

namespace vox {
/**
 * Index buffer binding.
 */
class IndexBufferBinding {
public:
    /**
     * Create index buffer binding.
     * @param buffer - Index buffer
     * @param index_type - Index buffer format
     */
    IndexBufferBinding(core::Buffer&& buffer, VkIndexType index_type);
    
    /**
     * Index buffer.
     */
    [[nodiscard]] const core::Buffer& buffer() const;

    /**
     * Index buffer format.
     */
    [[nodiscard]] VkIndexType index_type() const;
    
private:
    core::Buffer buffer_;
    VkIndexType index_type_;
};

}

#endif /* DIGITALVOX_VOX_RENDER_MESH_INDEX_BUFFER_BINDING_H_ */
