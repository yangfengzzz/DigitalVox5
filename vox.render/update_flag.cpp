//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/update_flag.h"

#include "vox.render/update_flag_manager.h"

namespace vox {
UpdateFlag::UpdateFlag(UpdateFlagManager *flags) : flags_(flags) { flags->update_flags_.push_back(this); }

UpdateFlag::~UpdateFlag() {
    flags_->update_flags_.erase(std::remove(flags_->update_flags_.begin(), flags_->update_flags_.end(), this),
                                flags_->update_flags_.end());
}

}  // namespace vox
