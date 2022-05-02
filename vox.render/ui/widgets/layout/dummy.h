//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vector2.h"

#include "ui/widgets/widget.h"

namespace vox::ui {
/**
 * Dummy widget that takes the given size as space in the panel
 */
class Dummy : public Widget {
public:
    explicit Dummy(const Vector2F &size = {0.0f, 0.0f});
    
protected:
    void draw_impl() override;
    
public:
    Vector2F size_;
};

}
