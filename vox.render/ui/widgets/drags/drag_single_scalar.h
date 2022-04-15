//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef DIGITALVOX_VOX_RENDER_UI_WIDGETS_DRAGS_DRAG_SINGLE_SCALAR_H_
#define DIGITALVOX_VOX_RENDER_UI_WIDGETS_DRAGS_DRAG_SINGLE_SCALAR_H_

#include <utility>

#include "ui/widgets/data_widget.h"
#include "event.h"

namespace vox::ui {
/**
 * Drag widget of generic type
 */
template<typename T>
class DragSingleScalar : public DataWidget<T> {
    static_assert(std::is_scalar<T>::value, "Invalid DragSingleScalar T (Scalar expected)");
    
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
    DragSingleScalar(ImGuiDataType p_data_type,
                     T p_min,
                     T p_max,
                     T p_value,
                     float p_speed,
                     std::string p_label,
                     std::string p_format)
    : DataWidget<T>(value_), data_type_(p_data_type), min_(p_min), max_(p_max),
    value_(p_value), speed_(p_speed), label_(std::move(p_label)), format_(std::move(p_format)) {
    }
    
protected:
    void draw_impl() override {
        if (max_ < min_)
            max_ = min_;
        
        if (value_ < min_)
            value_ = min_;
        else if (value_ > max_)
            value_ = max_;
        
        if (ImGui::DragScalar((label_ + DataWidget<T>::widget_id_).c_str(),
                              data_type_, &value_, speed_, &min_, &max_, format_.c_str())) {
            value_changed_event_.invoke(value_);
            DataWidget<T>::notify_change();
        }
    }
    
public:
    T min_;
    T max_;
    T value_;
    float speed_;
    std::string label_;
    std::string format_;
    Event<T> value_changed_event_;
    
private:
    ImGuiDataType data_type_;
};

}
#endif /* DIGITALVOX_VOX_RENDER_UI_WIDGETS_DRAGS_DRAG_SINGLE_SCALAR_H_ */
