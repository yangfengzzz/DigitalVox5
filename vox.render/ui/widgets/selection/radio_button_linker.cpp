//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "radio_button_linker.h"

namespace vox::ui {
RadioButtonLinker::RadioButtonLinker() : DataWidget<int>(selected_) {
}

void RadioButtonLinker::link(RadioButton &radio_button) {
    radio_button.radio_id_ = available_radio_id_++;
    auto listener_id = radio_button.clicked_event_ +=
    std::bind(&RadioButtonLinker::on_radio_button_clicked, this, std::placeholders::_1);
    
    if (radio_button.is_selected() && selected_ == -1)
        selected_ = radio_button.radio_id_;
    
    radio_buttons_.emplace_back(listener_id, std::ref(radio_button));
}

void RadioButtonLinker::unlink(RadioButton &radio_button) {
    auto it = std::find_if(radio_buttons_.begin(), radio_buttons_.end(),
                           [&radio_button](std::pair<ListenerId, std::reference_wrapper<RadioButton>> &pair) {
        return &pair.second.get() == &radio_button;
    });
    
    if (it != radio_buttons_.end()) {
        it->second.get().clicked_event_.remove_listener(it->first);
        radio_buttons_.erase(it);
    }
}

int RadioButtonLinker::selected() const {
    return selected_;
}

void RadioButtonLinker::draw_impl() {
    // The RadioButtonLinker is special, it has nothing to display :)
}

void RadioButtonLinker::on_radio_button_clicked(int radio_id) {
    if (radio_id != selected_) {
        selected_ = radio_id;
        value_changed_event_.invoke(selected_);
        notify_change();
        
        for (const auto &[kListener, kRadioButton] : radio_buttons_)
            kRadioButton.get().selected_ = kRadioButton.get().radio_id_ == selected_;
    }
}

}
