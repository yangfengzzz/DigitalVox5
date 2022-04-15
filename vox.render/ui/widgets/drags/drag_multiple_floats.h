//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef DIGITALVOX_VOX_RENDER_UI_WIDGETS_DRAGS_DRAG_MULTIPLE_FLOATS_H_
#define DIGITALVOX_VOX_RENDER_UI_WIDGETS_DRAGS_DRAG_MULTIPLE_FLOATS_H_

#include "drag_multiple_scalars.h"

namespace vox::ui {
/**
 * Drag widget of multiple type float
 */
template<size_t Size>
class DragMultipleFloats : public DragMultipleScalars<int, Size> {
public:
    explicit DragMultipleFloats(float p_min = 0.0f,
                                float p_max = 1.0f,
                                float p_value = 0.5f,
                                float p_speed = 0.1f,
                                const std::string &p_label = "",
                                const std::string &p_format = "%.3f")
    : DragMultipleScalars<float, Size>(ImGuiDataType_::ImGuiDataType_Float,
                                       p_min, p_max, p_value, p_speed, p_label, p_format) {
    }
};

}
#endif /* DIGITALVOX_VOX_RENDER_UI_WIDGETS_DRAGS_DRAG_MULTIPLE_FLOATS_H_ */
