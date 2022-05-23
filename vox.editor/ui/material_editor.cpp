//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.editor/ui/material_editor.h"

#include "vox.render/ui/gui_drawer.h"
#include "vox.render/ui/widgets/buttons/button_simple.h"
#include "vox.render/ui/widgets/buttons/button_small.h"
#include "vox.render/ui/widgets/layout/columns.h"
#include "vox.render/ui/widgets/layout/group_collapsable.h"
#include "vox.render/ui/widgets/selection/color_edit.h"
#include "vox.render/ui/widgets/texts/text_colored.h"
#include "vox.render/ui/widgets/visual/separator.h"

namespace vox::editor::ui {
namespace {
void DrawHybridVec3(
        WidgetContainer &p_root, const std::string &p_name, Vector3F &p_data, float p_step, float p_min, float p_max) {
    GuiDrawer::CreateTitle(p_root, p_name);

    auto &right_side = p_root.CreateWidget<Group>();

    auto &xyz_widget = right_side.CreateWidget<DragMultipleScalars<float, 3>>(
            GuiDrawer::GetDataType<float>(), p_min, p_max, 0.f, p_step, "", GuiDrawer::GetFormat<float>());
    auto &xyz_dispatcher = xyz_widget.AddPlugin<DataDispatcher<std::array<float, 3>>>();
    xyz_dispatcher.RegisterReference(reinterpret_cast<std::array<float, 3> &>(p_data));
    xyz_widget.line_break_ = false;

    auto &rgb_widget = right_side.CreateWidget<ColorEdit>(false, Color{p_data.x, p_data.y, p_data.z});
    auto &rgb_dispatcher = rgb_widget.AddPlugin<DataDispatcher<Color>>();
    rgb_dispatcher.RegisterReference(reinterpret_cast<Color &>(p_data));
    rgb_widget.enabled_ = false;
    rgb_widget.line_break_ = false;

    auto &xyz_button = right_side.CreateWidget<ButtonSimple>("XYZ");
    xyz_button.idle_background_color_ = {0.7f, 0.5f, 0.0f};
    xyz_button.line_break_ = false;

    auto &rgb_button = right_side.CreateWidget<ButtonSimple>("RGB");
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

void DrawHybridVec4(
        WidgetContainer &p_root, const std::string &p_name, Vector4F &p_data, float p_step, float p_min, float p_max) {
    GuiDrawer::CreateTitle(p_root, p_name);

    auto &right_side = p_root.CreateWidget<Group>();

    auto &xyz_widget = right_side.CreateWidget<DragMultipleScalars<float, 4>>(
            GuiDrawer::GetDataType<float>(), p_min, p_max, 0.f, p_step, "", GuiDrawer::GetFormat<float>());
    auto &xyz_dispatcher = xyz_widget.AddPlugin<DataDispatcher<std::array<float, 4>>>();
    xyz_dispatcher.RegisterReference(reinterpret_cast<std::array<float, 4> &>(p_data));
    xyz_widget.line_break_ = false;

    auto &rgba_widget = right_side.CreateWidget<ColorEdit>(true, Color{p_data.x, p_data.y, p_data.z, p_data.w});
    auto &rgba_dispatcher = rgba_widget.AddPlugin<DataDispatcher<Color>>();
    rgba_dispatcher.RegisterReference(reinterpret_cast<Color &>(p_data));
    rgba_widget.enabled_ = false;
    rgba_widget.line_break_ = false;

    auto &xyzw_button = right_side.CreateWidget<ButtonSimple>("XYZW");
    xyzw_button.idle_background_color_ = {0.7f, 0.5f, 0.0f};
    xyzw_button.line_break_ = false;

    auto &rgba_button = right_side.CreateWidget<ButtonSimple>("RGBA");
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
}  // namespace

// MARK: - MaterialEditor
MaterialEditor::MaterialEditor(const std::string &p_title, bool p_opened, const PanelWindowSettings &window_settings)
    : PanelWindow(p_title, p_opened, window_settings) {
    CreateHeaderButtons();
    CreateWidget<Separator>();
    CreateMaterialSelector();
    settings_ = &CreateWidget<Group>();
    CreateShaderSelector();
    CreateMaterialSettings();
    CreateShaderSettings();

    settings_->enabled_ = false;
    shader_settings_->enabled_ = false;

    material_dropped_event_ += std::bind(&MaterialEditor::OnMaterialDropped, this);
    shader_dropped_event_ += std::bind(&MaterialEditor::OnShaderDropped, this);
}

void MaterialEditor::Refresh() {}

void MaterialEditor::SetTarget(const std::shared_ptr<Material> &new_target) {}

std::shared_ptr<Material> MaterialEditor::GetTarget() const { return nullptr; }

void MaterialEditor::RemoveTarget() {}

void MaterialEditor::Preview() {}

void MaterialEditor::Reset() {}

void MaterialEditor::OnMaterialDropped() {}

void MaterialEditor::OnShaderDropped() {}

void MaterialEditor::CreateHeaderButtons() {}

void MaterialEditor::CreateMaterialSelector() {}

void MaterialEditor::CreateShaderSelector() {}

void MaterialEditor::CreateMaterialSettings() {}

void MaterialEditor::CreateShaderSettings() {}

void MaterialEditor::GenerateShaderSettingsContent() {}

void MaterialEditor::GenerateMaterialSettingsContent() {}

}  // namespace vox::editor::ui
