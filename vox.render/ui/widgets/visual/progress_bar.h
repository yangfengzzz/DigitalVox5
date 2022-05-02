//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "ui/widgets/widget.h"
#include "vector2.h"

namespace vox::ui {
/**
 * Simple widget that display a progress bar
 */
class ProgressBar : public Widget {
public:
    explicit ProgressBar(float fraction = 0.0f, const Vector2F &size = {0.0f, 0.0f},
                         std::string overlay = "");
    
protected:
    void draw_impl() override;
    
public:
    float fraction_;
    Vector2F size_;
    std::string overlay_;
};

}
