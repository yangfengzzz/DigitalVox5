//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "button.h"
#include "vector2.h"
#include "color.h"
#include <volk.h>

namespace vox::ui {
/**
 * Button widget with an image
 */
class ButtonImage : public Button {
public:
    ButtonImage(VkDescriptorSet texture_id, const Vector2F &size);
    
protected:
    void draw_impl() override;
    
public:
    bool disabled_ = false;
    
    Color background_ = {0, 0, 0, 0};
    Color tint_ = {1, 1, 1, 1};
    
    VkDescriptorSet texture_id_;
    Vector2F size_;
};

}
