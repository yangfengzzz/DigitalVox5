//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef DIGITALVOX_VOX_RENDER_UI_WIDGETS_PLOT_PLOT_LINES_H_
#define DIGITALVOX_VOX_RENDER_UI_WIDGETS_PLOT_PLOT_LINES_H_

#include "plot.h"

namespace vox::ui {
/**
 * Plot displayed as lines
 */
class PlotLines : public Plot {
public:
    /**
     * Constructor
     * @param p_data p_data
     * @param p_min_scale p_minScale
     * @param p_max_scale p_maxScale
     * @param p_size p_size
     * @param p_overlay p_overlay
     * @param p_label p_label
     */
    explicit PlotLines(const std::vector<float> &p_data = std::vector<float>(),
                       float p_min_scale = std::numeric_limits<float>::min(),
                       float p_max_scale = std::numeric_limits<float>::max(),
                       const Vector2F &p_size = {0.0f, 0.0f},
                       const std::string &p_overlay = "",
                       const std::string &p_label = "");
    
protected:
    void draw_impl() override;
};

}
#endif /* DIGITALVOX_VOX_RENDER_UI_WIDGETS_PLOT_PLOT_LINES_H_ */
