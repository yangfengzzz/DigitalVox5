//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef DIGITALVOX_VOX_RENDER_UI_WIDGETS_VISUAL_PROGRESS_BAR_H_
#define DIGITALVOX_VOX_RENDER_UI_WIDGETS_VISUAL_PROGRESS_BAR_H_

#include "ui/widgets/widget.h"
#include "vector2.h"

namespace vox::ui {
/**
 * Simple widget that display a progress bar
 */
class ProgressBar : public Widget {
public:
    /**
     * Constructor
     * @param p_fraction p_fraction
     * @param p_size p_size
     * @param p_overlay p_overlay
     */
    explicit ProgressBar(float p_fraction = 0.0f, const Vector2F &p_size = {0.0f, 0.0f},
                         std::string p_overlay = "");
    
protected:
    void draw_impl() override;
    
public:
    float fraction_;
    Vector2F size_;
    std::string overlay_;
};

}
#endif /* DIGITALVOX_VOX_RENDER_UI_WIDGETS_VISUAL_PROGRESS_BAR_H_ */
