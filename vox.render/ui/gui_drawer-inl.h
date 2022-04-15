//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef DIGITALVOX_VOX_RENDER_UI_GUI_DRAWER_INL_H_
#define DIGITALVOX_VOX_RENDER_UI_GUI_DRAWER_INL_H_

#include "gui_drawer.h"

namespace vox::ui {
template<typename T>
inline ImGuiDataType_ GuiDrawer::get_data_type() {
    if constexpr (std::is_same<T, float>::value) return ImGuiDataType_Float;
    else if constexpr (std::is_same<T, double>::value) return ImGuiDataType_Double;
    else if constexpr (std::is_same<T, uint8_t>::value) return ImGuiDataType_U8;
    else if constexpr (std::is_same<T, uint16_t>::value) return ImGuiDataType_U16;
    else if constexpr (std::is_same<T, uint32_t>::value) return ImGuiDataType_U32;
    else if constexpr (std::is_same<T, uint64_t>::value) return ImGuiDataType_U64;
    else if constexpr (std::is_same<T, int8_t>::value) return ImGuiDataType_S8;
    else if constexpr (std::is_same<T, int16_t>::value) return ImGuiDataType_S16;
    else if constexpr (std::is_same<T, int32_t>::value) return ImGuiDataType_S32;
    else if constexpr (std::is_same<T, int64_t>::value) return ImGuiDataType_S64;
}

template<typename T>
inline std::string GuiDrawer::get_format() {
    if constexpr (std::is_same<T, double>::value) return "%.5f";
    else if constexpr (std::is_same<T, float>::value) return "%.3f";
    else return "%d";
}

template<typename T>
inline void GuiDrawer::draw_scalar(WidgetContainer &p_root, const std::string &p_name,
                                   T &p_data, float p_step, T p_min, T p_max) {
    static_assert(std::is_scalar<T>::value, "T must be a scalar");
    
    create_title(p_root, p_name);
    auto &widget = p_root.create_widget<DragSingleScalar<T >>(get_data_type<T>(), p_min, p_max,
                                                              p_data, p_step, "", get_format<T>());
    auto &dispatcher = widget.template add_plugin<DataDispatcher<T>>
    ();
    dispatcher.register_reference(p_data);
}

template<typename T>
inline void GuiDrawer::draw_scalar(WidgetContainer &p_root, const std::string &p_name,
                                   std::function<T(void)> p_gatherer, std::function<void(T)> p_provider,
                                   float p_step, T p_min, T p_max) {
    static_assert(std::is_scalar<T>::value, "T must be a scalar");
    
    create_title(p_root, p_name);
    auto &widget = p_root.create_widget<DragSingleScalar<T >>(get_data_type<T>(), p_min, p_max,
                                                              static_cast<T>(0), p_step, "", get_format<T>());
    auto &dispatcher = widget.template add_plugin<DataDispatcher<T>>
    ();
    dispatcher.register_gatherer(p_gatherer);
    dispatcher.register_provider(p_provider);
}

}
#endif /* DIGITALVOX_VOX_RENDER_UI_GUI_DRAWER_INL_H_ */
