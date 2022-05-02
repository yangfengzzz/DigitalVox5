//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "ui/widgets/data_widget.h"
#include "event.h"

namespace vox::ui {
/**
 * Widget that represent a button that is selectable. Only one radio button can be selected
 * in the same RadioButtonLinker
 */
class RadioButton : public DataWidget<bool> {
    friend class RadioButtonLinker;
    
public:
    explicit RadioButton(bool selected = false, std::string label = "");
    
    /**
     * Make the radio button selected
     */
    void select();
    
    /**
     * Returns true if the radio button is selected
     */
    [[nodiscard]] bool is_selected() const;
    
    /**
     * Returns the radio button ID
     */
    [[nodiscard]] bool radio_id() const;
    
protected:
    void draw_impl() override;
    
public:
    std::string label_;
    Event<int> clicked_event_;
    
private:
    bool selected_ = false;
    int radio_id_ = 0;
};

}
