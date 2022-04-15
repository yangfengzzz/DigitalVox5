//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef DIGITALVOX_VOX_RENDER_UI_WIDGETS_BUTTONS_BUTTON_IMAGE_H_
#define DIGITALVOX_VOX_RENDER_UI_WIDGETS_BUTTONS_BUTTON_IMAGE_H_

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
    /**
     * Constructor
     * @param p_texture_id p_textureID
     * @param p_size p_size
     */
    ButtonImage(VkDescriptorSet p_texture_id, const Vector2F &p_size);
    
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
#endif /* DIGITALVOX_VOX_RENDER_UI_WIDGETS_BUTTONS_BUTTON_IMAGE_H_ */
