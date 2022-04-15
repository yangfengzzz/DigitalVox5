//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "canvas.h"

namespace vox::ui {
void Canvas::draw() {
    if (!panels_.empty()) {
        ImGui::NewFrame();
        
        if (is_dockspace_) {
            ImGuiViewport *viewport = ImGui::GetMainViewport();
            ImGui::SetNextWindowPos(viewport->Pos);
            ImGui::SetNextWindowSize(viewport->Size);
            ImGui::SetNextWindowViewport(viewport->ID);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
            
            ImGui::Begin("##dockspace", nullptr,
                         ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize
                         | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus
                         | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking);
            ImGuiID dockspace_id = ImGui::GetID("MyDockspace");
            ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);
            ImGui::SetWindowPos({0.f, 0.f});
            ImVec2 display_size = ImGui::GetIO().DisplaySize;
            ImGui::SetWindowSize({(float)display_size.x, (float)display_size.y});
            ImGui::End();
            
            ImGui::PopStyleVar(3);
        }
        
        for (auto &panel : panels_)
            panel.get().draw();
        
        ImGui::Render();
    }
}

void Canvas::add_panel(Panel &p_panel) {
    panels_.push_back(std::ref(p_panel));
}

void Canvas::remove_panel(Panel &p_panel) {
    panels_.erase(std::remove_if(panels_.begin(), panels_.end(),
                                 [&p_panel](std::reference_wrapper<Panel> &p_item) {
        return &p_panel == &p_item.get();
    }), panels_.end());
}

void Canvas::remove_all_panels() {
    panels_.clear();
}

void Canvas::make_dock_space(bool p_state) {
    is_dockspace_ = p_state;
}

bool Canvas::is_dock_space() const {
    return is_dockspace_;
}

}