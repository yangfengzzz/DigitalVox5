//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vector2.h"
#include "vector3.h"
#include "vector4.h"
#include "quaternion.h"
#include "color.h"

#include "ui/widgets/widget_container.h"
#include "ui/widgets/texts/text.h"
#include "ui/widgets/drags/drag_single_scalar.h"
#include "ui/widgets/drags/drag_multiple_scalars.h"
#include "ui/widgets/input_fields/input_text.h"
#include "ui/widgets/visual/image.h"

namespace vox::ui {
/**
 * Provide some helpers to draw UI elements
 */
class GuiDrawer {
public:
    static const Color title_color_;
    static const Color clear_button_color_;
    
    static const float min_float_;
    static const float max_float_;
    
    /**
     * Defines the texture to use when there is no texture in a texture resource field
     * @param p_emptyTexture
     */
    // static void ProvideEmptyTexture(OvRendering::Resources::Texture& p_emptyTexture);
    
    /**
     * Draw a title with the title color
     * @param p_root p_root
     * @param p_name p_name
     */
    static void create_title(WidgetContainer &p_root, const std::string &p_name);
    
    template<typename T>
    static void draw_scalar(WidgetContainer &p_root,
                            const std::string &p_name,
                            T &p_data,
                            float p_step = 1.f,
                            T p_min = std::numeric_limits<T>::min(),
                            T p_max = std::numeric_limits<T>::max());
    
    static void draw_boolean(WidgetContainer &p_root, const std::string &p_name, bool &p_data);
    
    static void draw_vec_2(WidgetContainer &p_root, const std::string &p_name, Vector2F &p_data,
                           float p_step = 1.f, float p_min = min_float_, float p_max = max_float_);
    
    static void draw_vec_3(WidgetContainer &p_root, const std::string &p_name, Vector3F &p_data,
                           float p_step = 1.f, float p_min = min_float_, float p_max = max_float_);
    
    static void draw_vec_4(WidgetContainer &p_root, const std::string &p_name, Vector4F &p_data,
                           float p_step = 1.f, float p_min = min_float_, float p_max = max_float_);
    
    static void draw_quat(WidgetContainer &p_root, const std::string &p_name, QuaternionF &p_data,
                          float p_step = 1.f, float p_min = min_float_, float p_max = max_float_);
    
    static void draw_string(WidgetContainer &p_root, const std::string &p_name, std::string &p_data);
    
    static void draw_color(WidgetContainer &p_root, const std::string &p_name, Color &p_color, bool p_has_alpha = false);
    
    // static Text& draw_mesh(WidgetContainer& p_root, const std::string& p_name, OvRendering::Resources::Model*& p_data, Event<>* p_update_notifier = nullptr);
    // static Image& draw_texture(WidgetContainer& p_root, const std::string& p_name, OvRendering::Resources::Texture*& p_data, Event<>* p_update_notifier = nullptr);
    // static Text& draw_shader(WidgetContainer& p_root, const std::string& p_name, OvRendering::Resources::Shader*& p_data, Event<>* p_update_notifier = nullptr);
    // static Text& draw_material(WidgetContainer& p_root, const std::string& p_name, OvCore::Resources::Material*& p_data, Event<>* p_update_notifier = nullptr);
    // static Text& draw_sound(WidgetContainer& p_root, const std::string& p_name, OvAudio::Resources::Sound*& p_data, Event<>* p_update_notifier = nullptr);
    static Text &draw_asset(WidgetContainer &p_root,
                            const std::string &p_name,
                            std::string &p_data,
                            Event<> *p_update_notifier = nullptr);
    
    template<typename T>
    static void draw_scalar(WidgetContainer &p_root,
                            const std::string &p_name,
                            std::function<T(void)> p_gatherer,
                            std::function<void(T)> p_provider,
                            float p_step = 1.f,
                            T p_min = std::numeric_limits<T>::min(),
                            T p_max = std::numeric_limits<T>::max());
    
    static void draw_boolean(WidgetContainer &p_root, const std::string &p_name,
                             const std::function<bool(void)> &p_gatherer, const std::function<void(bool)> &p_provider);
    
    static void draw_vec_2(WidgetContainer &p_root,
                           const std::string &p_name,
                           const std::function<Vector2F(void)> &p_gatherer,
                           const std::function<void(Vector2F)> &p_provider,
                           float p_step = 1.f,
                           float p_min = min_float_,
                           float p_max = max_float_);
    
    static void draw_vec_3(WidgetContainer &p_root,
                           const std::string &p_name,
                           const std::function<Vector3F(void)> &p_gatherer,
                           const std::function<void(Vector3F)> &p_provider,
                           float p_step = 1.f,
                           float p_min = min_float_,
                           float p_max = max_float_);
    
    static void draw_vec_4(WidgetContainer &p_root,
                           const std::string &p_name,
                           const std::function<Vector4F(void)> &p_gatherer,
                           const std::function<void(Vector4F)> &p_provider,
                           float p_step = 1.f,
                           float p_min = min_float_,
                           float p_max = max_float_);
    
    static void draw_quat(WidgetContainer &p_root,
                          const std::string &p_name,
                          const std::function<QuaternionF(void)> &p_gatherer,
                          const std::function<void(QuaternionF)> &p_provider,
                          float p_step = 1.f,
                          float p_min = min_float_,
                          float p_max = max_float_);
    
    static void draw_ddstring(WidgetContainer &p_root, const std::string &p_name,
                              std::function<std::string(void)> p_gatherer, std::function<void(std::string)> p_provider,
                              const std::string &p_identifier);
    
    static void draw_string(WidgetContainer &p_root, const std::string &p_name,
                            std::function<std::string(void)> p_gatherer, std::function<void(std::string)> p_provider);
    
    static void draw_color(WidgetContainer &p_root,
                           const std::string &p_name,
                           std::function<Color(void)> p_gatherer,
                           std::function<void(Color)> p_provider,
                           bool p_has_alpha = false);
    
    template<typename T>
    static ImGuiDataType_ get_data_type();
    
    template<typename T>
    static std::string get_format();
    
private:
    // static OvRendering::Resources::Texture* __EMPTY_TEXTURE;
};

}

#include "gui_drawer-inl.h"