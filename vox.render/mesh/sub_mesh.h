//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

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

    [[nodiscard]] uint32_t Start() const;

    [[nodiscard]] uint32_t Count() const;

private:
    /** Start drawing offset. */
    uint32_t start_;
    /** Drawing count. */
    uint32_t count_;
};

}  // namespace vox
