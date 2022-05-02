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
     * @param emptyTexture
     */
    // static void ProvideEmptyTexture(OvRendering::Resources::Texture& emptyTexture);
    
    /**
     * Draw a title with the title color
     */
    static void create_title(WidgetContainer &root, const std::string &name);
    
    template<typename T>
    static void draw_scalar(WidgetContainer &root,
                            const std::string &name,
                            T &data,
                            float step = 1.f,
                            T min = std::numeric_limits<T>::min(),
                            T max = std::numeric_limits<T>::max());
    
    static void draw_boolean(WidgetContainer &root, const std::string &name, bool &data);
    
    static void draw_vec_2(WidgetContainer &root, const std::string &name, Vector2F &data,
                           float step = 1.f, float min = min_float_, float max = max_float_);
    
    static void draw_vec_3(WidgetContainer &root, const std::string &name, Vector3F &data,
                           float step = 1.f, float min = min_float_, float max = max_float_);
    
    static void draw_vec_4(WidgetContainer &root, const std::string &name, Vector4F &data,
                           float step = 1.f, float min = min_float_, float max = max_float_);
    
    static void draw_quat(WidgetContainer &root, const std::string &name, QuaternionF &data,
                          float step = 1.f, float min = min_float_, float max = max_float_);
    
    static void draw_string(WidgetContainer &root, const std::string &name, std::string &data);
    
    static void draw_color(WidgetContainer &root, const std::string &name, Color &color, bool has_alpha = false);
    
    // static Text& draw_mesh(WidgetContainer& root, const std::string& name, OvRendering::Resources::Model*& data, Event<>* update_notifier = nullptr);
    // static Image& draw_texture(WidgetContainer& root, const std::string& name, OvRendering::Resources::Texture*& data, Event<>* update_notifier = nullptr);
    // static Text& draw_shader(WidgetContainer& root, const std::string& name, OvRendering::Resources::Shader*& data, Event<>* update_notifier = nullptr);
    // static Text& draw_material(WidgetContainer& root, const std::string& name, OvCore::Resources::Material*& data, Event<>* update_notifier = nullptr);
    // static Text& draw_sound(WidgetContainer& root, const std::string& name, OvAudio::Resources::Sound*& data, Event<>* update_notifier = nullptr);
    static Text &draw_asset(WidgetContainer &root,
                            const std::string &name,
                            std::string &data,
                            Event<> *update_notifier = nullptr);
    
    template<typename T>
    static void draw_scalar(WidgetContainer &root,
                            const std::string &name,
                            std::function<T(void)> gatherer,
                            std::function<void(T)> provider,
                            float step = 1.f,
                            T min = std::numeric_limits<T>::min(),
                            T max = std::numeric_limits<T>::max());
    
    static void draw_boolean(WidgetContainer &root, const std::string &name,
                             const std::function<bool(void)> &gatherer, const std::function<void(bool)> &provider);
    
    static void draw_vec_2(WidgetContainer &root,
                           const std::string &name,
                           const std::function<Vector2F(void)> &gatherer,
                           const std::function<void(Vector2F)> &provider,
                           float step = 1.f,
                           float min = min_float_,
                           float max = max_float_);
    
    static void draw_vec_3(WidgetContainer &root,
                           const std::string &name,
                           const std::function<Vector3F(void)> &gatherer,
                           const std::function<void(Vector3F)> &provider,
                           float step = 1.f,
                           float min = min_float_,
                           float max = max_float_);
    
    static void draw_vec_4(WidgetContainer &root,
                           const std::string &name,
                           const std::function<Vector4F(void)> &gatherer,
                           const std::function<void(Vector4F)> &provider,
                           float step = 1.f,
                           float min = min_float_,
                           float max = max_float_);
    
    static void draw_quat(WidgetContainer &root,
                          const std::string &name,
                          const std::function<QuaternionF(void)> &gatherer,
                          const std::function<void(QuaternionF)> &provider,
                          float step = 1.f,
                          float min = min_float_,
                          float max = max_float_);
    
    static void draw_ddstring(WidgetContainer &root, const std::string &name,
                              std::function<std::string(void)> gatherer, std::function<void(std::string)> provider,
                              const std::string &identifier);
    
    static void draw_string(WidgetContainer &root, const std::string &name,
                            std::function<std::string(void)> gatherer, std::function<void(std::string)> provider);
    
    static void draw_color(WidgetContainer &root,
                           const std::string &name,
                           std::function<Color(void)> gatherer,
                           std::function<void(Color)> provider,
                           bool has_alpha = false);
    
    template<typename T>
    static ImGuiDataType_ get_data_type();
    
    template<typename T>
    static std::string get_format();
    
private:
    // static OvRendering::Resources::Texture* __EMPTY_TEXTURE;
};

}

#include "gui_drawer-inl.h"
