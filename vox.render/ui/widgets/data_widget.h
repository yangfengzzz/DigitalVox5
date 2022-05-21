//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "../plugins/data_dispatcher.h"
#include "buttons/button.h"

namespace vox::ui {
/**
 * DataWidget are widgets that contains a value. It is very usefully in combination with
 * DataDispatcher plugin
 */
template <typename T>
class DataWidget : public Widget {
public:
    /**
     * Create a DataWidget with the data specification
     */
    explicit DataWidget(T &data_holder) : data_(data_holder){};

    /**
     * Draw the widget
     */
    void draw() override;

    /**
     * Notify that the widget data has changed to allow the data dispatcher to execute its behaviour
     */
    void notify_change();

private:
    T &data_;
};

template <typename T>
inline void DataWidget<T>::draw() {
    if (enabled_) {
        TRY_GATHER(T, data_)
        Widget::draw();
        TRY_PROVIDE(T, data_)
    }
}

template <typename T>
inline void DataWidget<T>::notify_change() {
    TRY_NOTIFY_CHANGE(T)
}

}  // namespace vox::ui
