//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef DIGITALVOX_VOX_RENDER_UI_WIDGETS_VISUAL_IMAGE_H_
#define DIGITALVOX_VOX_RENDER_UI_WIDGETS_VISUAL_IMAGE_H_

#include "ui/widgets/widget.h"
#include "vector2.h"
#include <volk.h>

namespace vox::ui {
/**
 * Simple widget that display an image
 */
class Image : public Widget {
public:
    /**
     * Constructor
     * @param p_texture_id p_textureID
     * @parma p_size p_size
     */
    Image(VkDescriptorSet p_texture_id, const Vector2F &p_size);
    
    void set_texture_view(VkDescriptorSet p_texture_id);
    
protected:
    void draw_impl() override;
    
public:
    VkDescriptorSet texture_id_;
    Vector2F size_;
};

}
#endif /* DIGITALVOX_VOX_RENDER_UI_WIDGETS_VISUAL_IMAGE_H_ */
