//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef DIGITALVOX_VOX_RENDER_UI_WIDGETS_SELECTION_CHECK_BOX_H_
#define DIGITALVOX_VOX_RENDER_UI_WIDGETS_SELECTION_CHECK_BOX_H_

#include "ui/widgets/data_widget.h"
#include "event.h"

namespace vox::ui {
/**
 * Checkbox widget that can be checked or not
 */
class CheckBox : public DataWidget<bool> {
public:
    /**
     * Constructor
     * @param p_value p_value
     * @param p_label p_label
     */
    explicit CheckBox(bool p_value = false, std::string p_label = "");
    
protected:
    void draw_impl() override;
    
public:
    bool value_;
    std::string label_;
    Event<bool> value_changed_event_;
};

}
#endif /* DIGITALVOX_VOX_RENDER_UI_WIDGETS_SELECTION_CHECK_BOX_H_ */
