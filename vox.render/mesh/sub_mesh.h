//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef DIGITALVOX_VOX_RENDER_MESH_SUB_MESH_H_
#define DIGITALVOX_VOX_RENDER_MESH_SUB_MESH_H_

#include <volk.h>

namespace vox {
/**
 * Sub-mesh, mainly contains drawing information.
 */
class SubMesh {
public:
    /**
     * Create a sub-mesh.
     * @param start - Start drawing offset
     * @param count - Drawing count
     */
    explicit SubMesh(uint32_t start = 0, uint32_t count = 0);
    
    [[nodiscard]] uint32_t start() const;
    
    [[nodiscard]] uint32_t count() const;
    
private:
    /** Start drawing offset. */
    uint32_t start_;
    /** Drawing count. */
    uint32_t count_;
};

}

#endif /* DIGITALVOX_VOX_RENDER_MESH_SUB_MESH_H_ */
