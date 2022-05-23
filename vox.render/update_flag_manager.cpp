//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/update_flag_manager.h"

namespace vox {
std::unique_ptr<UpdateFlag> UpdateFlagManager::Registration() { return std::make_unique<UpdateFlag>(this); }

void UpdateFlagManager::Distribute() {
    for (auto &update_flag : update_flags_) {
        update_flag->flag_ = true;
    }
}

}  // namespace vox
