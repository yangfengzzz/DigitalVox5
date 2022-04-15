//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef DIGITALVOX_VOX_RENDER_UI_WIDGETS_DRAGS_DRAG_MULTIPLE_SCALARS_H_
#define DIGITALVOX_VOX_RENDER_UI_WIDGETS_DRAGS_DRAG_MULTIPLE_SCALARS_H_

#include <utility>

#include "ui/widgets/data_widget.h"
#include "event.h"

namespace vox::ui {
/**
 * Drag widget of multiple generic type
 */
template<typename T, size_t Size>
class DragMultipleScalars : public DataWidget<std::array<T, Size>> {
    static_assert(Size > 1, "Invalid DragMultipleScalars _Size (2 or more required)");
    
public:
    /**
     * Constructor
     * @param p_data_type p_dataType
     * @param p_min p_min
     * @param p_max p_max
     * @param p_value p_value
     * @param p_speed p_speed
     * @param p_label p_label
     * @param p_format p_format
     */
    DragMultipleScalars(ImGuiDataType_ p_data_type,
                        T p_min,
                        T p_max,
                        T p_value,
                        float p_speed,
                        std::string p_label,
                        std::string p_format) :
    DataWidget<std::array<T, Size>>(values_), data_type_(p_data_type),
    min_(p_min), max_(p_max), speed_(p_speed), label_(std::move(p_label)), format_(std::move(p_format)) {
        values_.fill(p_value);
    }
    
protected:
    void draw_impl() override {
        if (max_ < min_)
            max_ = min_;
        
        for (size_t i = 0; i < Size; ++i) {
            if (values_[i] < min_)
                values_[i] = min_;
            else if (values_[i] > max_)
                values_[i] = max_;
        }
        
        if (ImGui::DragScalarN((label_ + DataWidget<std::array<T, Size>>::widget_id_).c_str(),
                               data_type_, values_.data(), Size, speed_, &min_, &max_, format_.c_str())) {
            value_changed_event_.invoke(values_);
            DataWidget<std::array<T, Size>>::notify_change();
        }
    }
    
public:
    T min_;
    T max_;
    float speed_;
    std::array<T, Size> values_;
    std::string label_;
    std::string format_;
    Event<std::array<T, Size> &> value_changed_event_;
    
protected:
    ImGuiDataType_ data_type_;
};

}
#endif /* DIGITALVOX_VOX_RENDER_UI_WIDGETS_DRAGS_DRAG_MULTIPLE_SCALARS_H_ */
