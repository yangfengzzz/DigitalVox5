//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef DIGITALVOX_VOX_RENDER_EVENT_INL_H_
#define DIGITALVOX_VOX_RENDER_EVENT_INL_H_

#include "event.h"

namespace vox {
template<class... ArgTypes>
ListenerId Event<ArgTypes...>::add_listener(Callback p_callback) {
    ListenerId listener_id = available_listener_id_++;
    callbacks_.emplace(listener_id, p_callback);
    return listener_id;
}

template<class... ArgTypes>
ListenerId Event<ArgTypes...>::operator+=(Callback p_callback) {
    return add_listener(p_callback);
}

template<class... ArgTypes>
bool Event<ArgTypes...>::remove_listener(ListenerId p_listener_id) {
    return callbacks_.erase(p_listener_id) != 0;
}

template<class... ArgTypes>
bool Event<ArgTypes...>::operator-=(ListenerId p_listener_id) {
    return remove_listener(p_listener_id);
}

template<class... ArgTypes>
void Event<ArgTypes...>::remove_all_listeners() {
    callbacks_.clear();
}

template<class... ArgTypes>
uint64_t Event<ArgTypes...>::listener_count() {
    return callbacks_.size();
}

template<class... ArgTypes>
void Event<ArgTypes...>::invoke(ArgTypes... p_args) {
    for (auto const&[kEy, kValue] : callbacks_)
        kValue(p_args...);
}
}

#endif /* DIGITALVOX_VOX_RENDER_EVENT_INL_H_ */
