//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef DIGITALVOX_VOX_RENDER_UPDATE_FLAG_MANAGER_H_
#define DIGITALVOX_VOX_RENDER_UPDATE_FLAG_MANAGER_H_

#include <memory>
#include <vector>
#include "update_flag.h"

namespace vox {
class UpdateFlagManager {
public:
    std::unique_ptr<UpdateFlag> registration();
    
    void distribute();
    
private:
    friend class UpdateFlag;
    
    std::vector<UpdateFlag *> update_flags_;
};

}        // namespace vox
#endif /* DIGITALVOX_VOX_RENDER_UPDATE_FLAG_MANAGER_H_ */
