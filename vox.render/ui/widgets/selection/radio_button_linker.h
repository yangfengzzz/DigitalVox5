//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "radio_button.h"

namespace vox::ui {
/**
 * Handle the link of radio buttons. Necessary to enable the correct behaviour
 * of radio buttons
 */
class RadioButtonLinker : public DataWidget<int> {
public:
    /**
     * Constructor
     */
    RadioButtonLinker();

    /**
     * Link the given radio button
     */
    void link(RadioButton &radio_button);

    /**
     * Unlink the given radio button
     */
    void unlink(RadioButton &radio_button);

    /**
     * Returns the id of the selected radio button
     */
    [[nodiscard]] int selected() const;

protected:
    void draw_impl() override;

private:
    void on_radio_button_clicked(int radio_id);

public:
    Event<int> value_changed_event_;

private:
    int available_radio_id_ = 0;
    int selected_ = -1;
    std::vector<std::pair<ListenerId, std::reference_wrapper<RadioButton>>> radio_buttons_;
};

}  // namespace vox::ui
