//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.


#include "input_events.h"

namespace vox {
InputEvent::InputEvent(EventSource source) :
source_{source} {
}

EventSource InputEvent::get_source() const {
    return source_;
}

KeyInputEvent::KeyInputEvent(KeyCode code, KeyAction action) :
InputEvent{EventSource::KEYBOARD},
code_{code},
action_{action} {
}

KeyCode KeyInputEvent::get_code() const {
    return code_;
}

KeyAction KeyInputEvent::get_action() const {
    return action_;
}

MouseButtonInputEvent::MouseButtonInputEvent(MouseButton button, MouseAction action, float pos_x, float pos_y) :
	InputEvent{EventSource::MOUSE},
	button_{button},
	action_{action},
	pos_x_{pos_x},
	pos_y_{pos_y} {
}

MouseButton MouseButtonInputEvent::get_button() const {
    return button_;
}

MouseAction MouseButtonInputEvent::get_action() const {
    return action_;
}

float MouseButtonInputEvent::get_pos_x() const {
    return pos_x_;
}

float MouseButtonInputEvent::get_pos_y() const {
    return pos_y_;
}

TouchInputEvent::TouchInputEvent(int32_t pointer_id, std::size_t touch_points, TouchAction action, float pos_x,
                                 float pos_y) :
InputEvent{EventSource::TOUCHSCREEN},
action_{action},
pointer_id_{pointer_id},
touch_points_{touch_points},
pos_x_{pos_x},
pos_y_{pos_y} {
}

TouchAction TouchInputEvent::get_action() const {
    return action_;
}

int32_t TouchInputEvent::get_pointer_id() const {
    return pointer_id_;
}

std::size_t TouchInputEvent::get_touch_points() const {
    return touch_points_;
}

float TouchInputEvent::get_pos_x() const {
    return pos_x_;
}

float TouchInputEvent::get_pos_y() const {
    return pos_y_;
}


}        // namespace vox
