//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef DIGITALVOX_VOX_RENDER_UPDATE_FLAG_H_
#define DIGITALVOX_VOX_RENDER_UPDATE_FLAG_H_

#include <cstdio>

namespace vox {
class UpdateFlagManager;

/**
 * Used to update tags.
 */
class UpdateFlag {
public:
    bool flag_ = true;
    
    explicit UpdateFlag(UpdateFlagManager *flags);
    
    ~UpdateFlag();
    
private:
    UpdateFlagManager *flags_{nullptr};
};

}        // namespace vox

#endif /* DIGITALVOX_VOX_RENDER_UPDATE_FLAG_H_ */
