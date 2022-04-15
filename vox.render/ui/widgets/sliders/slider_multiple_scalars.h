//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef DIGITALVOX_VOX_RENDER_UI_WIDGETS_SLIDERS_SLIDER_MULTIPLE_SCALARS_H_
#define DIGITALVOX_VOX_RENDER_UI_WIDGETS_SLIDERS_SLIDER_MULTIPLE_SCALARS_H_

#include "event.h"
#include "ui/widgets/data_widget.h"
#include <array>
#include <utility>

namespace vox::ui {
/**
 * Slider widget of multiple generic type
 */
template<typename T, size_t Size>
class SliderMultipleScalars : public DataWidget<std::array<T, Size>> {
    using DataWidget<std::array<T, Size>>::widget_id_;
    using DataWidget<std::array<T, Size>>::notify_change;
    
    static_assert(Size > 1, "Invalid SliderMultipleScalars _Size (2 or more required)");
    
public:
    /**
     * Constructor
     * @param p_data_type
     * @param p_min
     * @param p_max
     * @param p_value
     * @param p_label
     * @param p_format
     */
    SliderMultipleScalars
    (
     ImGuiDataType_ p_data_type,
     T p_min,
     T p_max,
     T p_value,
     std::string p_label,
     std::string p_format
     )
    : DataWidget<std::array<T, Size>>(values_),
    data_type_(p_data_type),
    min_(p_min),
    max_(p_max),
    label_(std::move(p_label)),
    format_(std::move(p_format)) {
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
        
        bool value_changed = ImGui::SliderScalarN((label_ + widget_id_).c_str(),
                                                  data_type_,
                                                  values_.data(),
                                                  Size,
                                                  &min_,
                                                  &max_,
                                                  format_.c_str());
        
        if (value_changed) {
            value_changed_event_.invoke(values_);
            notify_change();
        }
    }
    
public:
    T min_;
    T max_;
    std::array<T, Size> values_;
    std::string label_;
    std::string format_;
    Event<std::array<T, Size> &> value_changed_event_;
    
protected:
    ImGuiDataType_ data_type_;
};

}
#endif /* DIGITALVOX_VOX_RENDER_UI_WIDGETS_SLIDERS_SLIDER_MULTIPLE_SCALARS_H_ */
