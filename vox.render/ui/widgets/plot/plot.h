//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef DIGITALVOX_VOX_RENDER_UI_WIDGETS_PLOT_PLOT_H_
#define DIGITALVOX_VOX_RENDER_UI_WIDGETS_PLOT_PLOT_H_

#include <vector>
#include <limits>

#include "vector2.h"

#include "ui/widgets/widget.h"
#include "ui/widgets/data_widget.h"

namespace vox::ui {
/**
 * Base class for any plot widget
 */
class Plot : public DataWidget<std::vector<float>> {
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
    explicit Plot(std::vector<float> p_data = std::vector<float>(),
                  float p_min_scale = std::numeric_limits<float>::min(),
                  float p_max_scale = std::numeric_limits<float>::max(),
                  const Vector2F &p_size = {0.0f, 0.0f},
                  std::string p_overlay = "",
                  std::string p_label = "");
    
protected:
    void draw_impl() override = 0;
    
public:
    std::vector<float> data_;
    float min_scale_;
    float max_scale_;
    Vector2F size_;
    std::string overlay_;
    std::string label_;
};

}
#endif /* DIGITALVOX_VOX_RENDER_UI_WIDGETS_PLOT_PLOT_H_ */
