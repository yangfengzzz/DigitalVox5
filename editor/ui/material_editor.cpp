//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "material_editor.h"

#include "ui/gui_drawer.h"
#include "ui/widgets/layout/columns.h"
#include "ui/widgets/layout/group_collapsable.h"
#include "ui/widgets/visual/separator.h"
#include "ui/widgets/texts/text_colored.h"
#include "ui/widgets/buttons/button_simple.h"
#include "ui/widgets/buttons/button_small.h"
#include "ui/widgets/selection/color_edit.h"

namespace vox {
namespace editor {
namespace ui {
namespace {
void draw_hybrid_vec_3(WidgetContainer &p_root, const std::string &p_name, Vector3F &p_data,
                       float p_step, float p_min, float p_max) {
    GuiDrawer::create_title(p_root, p_name);
    
    auto &right_side = p_root.create_widget<Group>();
    
    auto &xyz_widget = right_side.create_widget<DragMultipleScalars<float, 3>>(GuiDrawer::get_data_type<float>(), p_min, p_max,
                                                                               0.f, p_step, "", GuiDrawer::get_format<float>());
    auto &xyz_dispatcher = xyz_widget.add_plugin<DataDispatcher<std::array<float, 3>>>();
    xyz_dispatcher.register_reference(reinterpret_cast<std::array<float, 3> &>(p_data));
    xyz_widget.line_break_ = false;
    
    auto &rgb_widget = right_side.create_widget<ColorEdit>(false, Color{p_data.x, p_data.y, p_data.z});
    auto &rgb_dispatcher = rgb_widget.add_plugin<DataDispatcher<Color>>();
    rgb_dispatcher.register_reference(reinterpret_cast<Color &>(p_data));
    rgb_widget.enabled_ = false;
    rgb_widget.line_break_ = false;
    
    auto &xyz_button = right_side.create_widget<ButtonSimple>("XYZ");
    xyz_button.idle_background_color_ = {0.7f, 0.5f, 0.0f};
    xyz_button.line_break_ = false;
    
    auto &rgb_button = right_side.create_widget<ButtonSimple>("RGB");
    rgb_button.idle_background_color_ = {0.7f, 0.5f, 0.0f};
    
    xyz_button.clicked_event_ += [&] {
        xyz_widget.enabled_ = true;
        rgb_widget.enabled_ = false;
    };
    
    rgb_button.clicked_event_ += [&] {
        xyz_widget.enabled_ = false;
        rgb_widget.enabled_ = true;
    };
}

void draw_hybrid_vec_4(WidgetContainer &p_root, const std::string &p_name, Vector4F &p_data,
                       float p_step, float p_min, float p_max) {
    GuiDrawer::create_title(p_root, p_name);
    
    auto &right_side = p_root.create_widget<Group>();
    
    auto &xyz_widget = right_side.create_widget<DragMultipleScalars<float, 4>>(GuiDrawer::get_data_type<float>(), p_min, p_max,
                                                                               0.f, p_step, "", GuiDrawer::get_format<float>());
    auto &xyz_dispatcher = xyz_widget.add_plugin<DataDispatcher<std::array<float, 4>>>();
    xyz_dispatcher.register_reference(reinterpret_cast<std::array<float, 4> &>(p_data));
    xyz_widget.line_break_ = false;
    
    auto &rgba_widget = right_side.create_widget<ColorEdit>(true, Color{p_data.x, p_data.y, p_data.z, p_data.w});
    auto &rgba_dispatcher = rgba_widget.add_plugin<DataDispatcher<Color>>();
    rgba_dispatcher.register_reference(reinterpret_cast<Color &>(p_data));
    rgba_widget.enabled_ = false;
    rgba_widget.line_break_ = false;
    
    auto &xyzw_button = right_side.create_widget<ButtonSimple>("XYZW");
    xyzw_button.idle_background_color_ = {0.7f, 0.5f, 0.0f};
    xyzw_button.line_break_ = false;
    
    auto &rgba_button = right_side.create_widget<ButtonSimple>("RGBA");
    rgba_button.idle_background_color_ = {0.7f, 0.5f, 0.0f};
    
    xyzw_button.clicked_event_ += [&] {
        xyz_widget.enabled_ = true;
        rgba_widget.enabled_ = false;
    };
    
    rgba_button.clicked_event_ += [&] {
        xyz_widget.enabled_ = false;
        rgba_widget.enabled_ = true;
    };
}
} // namespace

//MARK: - MaterialEditor
MaterialEditor::MaterialEditor(const std::string &p_title,
                               bool p_opened,
                               const PanelWindowSettings &window_settings) :
PanelWindow(p_title, p_opened, window_settings) {
    create_header_buttons();
    create_widget<Separator>();
    create_material_selector();
    settings_ = &create_widget<Group>();
    create_shader_selector();
    create_material_settings();
    create_shader_settings();
    
    settings_->enabled_ = false;
    shader_settings_->enabled_ = false;
    
    material_dropped_event_ += std::bind(&MaterialEditor::on_material_dropped, this);
    shader_dropped_event_ += std::bind(&MaterialEditor::on_shader_dropped, this);
}

void MaterialEditor::refresh() {
    
}

void MaterialEditor::set_target(const std::shared_ptr<Material> &new_target) {
    
}

std::shared_ptr<Material> MaterialEditor::get_target() const {
    return nullptr;
}

void MaterialEditor::remove_target() {
    
}

void MaterialEditor::preview() {
    
}

void MaterialEditor::reset() {
    
}

void MaterialEditor::on_material_dropped() {
    
}

void MaterialEditor::on_shader_dropped() {
    
}

void MaterialEditor::create_header_buttons() {
    
}

void MaterialEditor::create_material_selector() {
    
}

void MaterialEditor::create_shader_selector() {
    
}

void MaterialEditor::create_material_settings() {
    
}

void MaterialEditor::create_shader_settings() {
    
}

void MaterialEditor::generate_shader_settings_content() {
    
}

void MaterialEditor::generate_material_settings_content() {
    
}

}
}
}
