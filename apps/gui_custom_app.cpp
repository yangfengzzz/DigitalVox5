//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "apps/gui_custom_app.h"

#include "vox.render/camera.h"
#include "vox.render/controls/orbit_control.h"
#include "vox.render/ui/widgets/texts/text_labelled.h"

namespace vox {
class CustomGUI : public ui::Widget {
    void DrawImpl() override {
        ImGui::Text("Custom UI");
        ImGui::Button("Click");
    }
};

void GuiCustomApp::LoadScene() {
    gui_->LoadFont("Ruda_Big", "Fonts/Ruda-Bold.ttf", 16);
    gui_->LoadFont("Ruda_Medium", "Fonts/Ruda-Bold.ttf", 14);
    gui_->LoadFont("Ruda_Small", "Fonts/Ruda-Bold.ttf", 12);
    gui_->UseFont("Ruda_Medium");
    gui_->SetEditorLayoutAutosaveFrequency(60.0f);
    gui_->EnableEditorLayoutSave(true);
    gui_->EnableDocking(true);

    gui_->SetCanvas(canvas_);
    canvas_.AddPanel(panel_);
    panel_.CreateWidget<CustomGUI>();

    auto scene = scene_manager_->CurrentScene();
    auto root_entity = scene->CreateRootEntity();

    auto camera_entity = root_entity->CreateChild();
    camera_entity->transform->SetPosition(10, 10, 10);
    camera_entity->transform->LookAt(Point3F(0, 0, 0));
    main_camera_ = camera_entity->AddComponent<Camera>();
    camera_entity->AddComponent<control::OrbitControl>();

    scene->Play();
}

}  // namespace vox
