//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "group.h"
#include "event.h"

namespace vox::ui {
/**
 * Widget that can contains other widgets and is collapsable
 */
class GroupCollapsable : public Group {
public:
    explicit GroupCollapsable(std::string name = "");
    
protected:
    void draw_impl() override;
    
public:
    std::string name_;
    bool closable_ = false;
    bool opened_ = true;
    Event<> close_event_;
    Event<> open_event_;
};

}
