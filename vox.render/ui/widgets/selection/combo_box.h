//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef DIGITALVOX_VOX_RENDER_UI_WIDGETS_SELECTION_COMBO_BOX_H_
#define DIGITALVOX_VOX_RENDER_UI_WIDGETS_SELECTION_COMBO_BOX_H_

#include "ui/widgets/data_widget.h"
#include "event.h"
#include <map>

namespace vox::ui {
/**
 * Widget that can display a list of values that the user can select
 */
class ComboBox : public DataWidget<int> {
public:
    /**
     * Constructor
     * @param p_current_choice p_currentChoice
     */
    explicit ComboBox(int p_current_choice = 0);
    
protected:
    void draw_impl() override;
    
public:
    std::map<int, std::string> choices_;
    int current_choice_;
    
public:
    Event<int> value_changed_event_;
};

}
#endif /* DIGITALVOX_VOX_RENDER_UI_WIDGETS_SELECTION_COMBO_BOX_H_ */
