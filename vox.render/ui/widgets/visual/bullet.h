//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef DIGITALVOX_VOX_RENDER_UI_WIDGETS_VISUAL_BULLET_H_
#define DIGITALVOX_VOX_RENDER_UI_WIDGETS_VISUAL_BULLET_H_

#include "ui/widgets/widget.h"

namespace vox::ui {
/**
 * Simple widget that display a bullet point
 */
class Bullet : public Widget {
protected:
    void draw_impl() override;
};

}
#endif /* DIGITALVOX_VOX_RENDER_UI_WIDGETS_VISUAL_BULLET_H_ */
