//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef DIGITALVOX_VOX_RENDER_UI_WIDGETS_INPUT_FIELDS_INPUT_SINGLE_SCALAR_H_
#define DIGITALVOX_VOX_RENDER_UI_WIDGETS_INPUT_FIELDS_INPUT_SINGLE_SCALAR_H_

#include <utility>

#include "ui/widgets/data_widget.h"
#include "event.h"

namespace vox::ui {
/**
 * Input widget of generic type
 */
template<typename T>
class InputSingleScalar : public DataWidget<T> {
    static_assert(std::is_scalar<T>::value, "Invalid InputSingleScalar T (Scalar expected)");
    
public:
    /**
     * Constructor
     * @param p_data_type p_dataType
     * @param p_default_value p_defaultValue
     * @param p_step p_step
     * @param p_fast_step p_fastStep
     * @param p_label p_label
     * @param p_format p_format
     * @param p_select_all_on_click p_selectAllOnClick
     */
    InputSingleScalar(ImGuiDataType p_data_type,
                      T p_default_value,
                      T p_step,
                      T p_fast_step,
                      std::string p_label,
                      std::string p_format,
                      bool p_select_all_on_click) :
    DataWidget<T>(value_),
    data_type_(p_data_type),
    value_(p_default_value),
    step_(p_step),
    fast_step_(p_fast_step),
    label_(std::move(p_label)),
    format_(std::move(p_format)),
    select_all_on_click_(p_select_all_on_click) {
    }
    
protected:
    void draw_impl() override {
        T previous_value = value_;
        
        ImGuiInputTextFlags flags = ImGuiInputTextFlags_EnterReturnsTrue;
        if (select_all_on_click_)
            flags |= ImGuiInputTextFlags_AutoSelectAll;
        
        bool enter_pressed = ImGui::InputScalar((label_ + DataWidget<T>::widget_id_).c_str(), data_type_, &value_,
                                                step_ != 0.0f ? &step_ : nullptr,
                                                fast_step_ != 0.0f ? &fast_step_ : nullptr,
                                                format_.c_str(), flags);
        
        if (previous_value != value_) {
            content_changed_event_.invoke(value_);
            DataWidget<T>::notify_change();
        }
        
        if (enter_pressed)
            enter_pressed_event_.invoke(value_);
    }
    
public:
    T value_;
    T step_;
    T fast_step_;
    std::string label_;
    std::string format_;
    bool select_all_on_click_;
    Event<T> content_changed_event_;
    Event<T> enter_pressed_event_;
    
private:
    ImGuiDataType data_type_;
};

}
#endif /* DIGITALVOX_VOX_RENDER_UI_WIDGETS_INPUT_FIELDS_INPUT_SINGLE_SCALAR_H_ */
