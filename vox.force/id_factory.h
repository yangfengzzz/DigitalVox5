//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

namespace vox::force {
/** Factory for unique ids.
 */
class IDFactory {
public:
    static int GetId() { return id_++; }

private:
    /** Current id */
    static int id_;
};

}  // namespace vox::force
