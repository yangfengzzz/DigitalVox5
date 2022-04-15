//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "button_simple.h"
#include "ui/widgets/converter.h"
#include <imgui_internal.h>

#include <utility>

namespace vox::ui {
ButtonSimple::ButtonSimple(std::string p_label, const Vector2F &p_size, bool p_disabled) :
label_(std::move(p_label)), size_(p_size), disabled_(p_disabled) {
    auto &style = ImGui::GetStyle();
    
    idle_background_color_ = Converter::to_color(style.Colors[ImGuiCol_Button]);
    hovered_background_color_ = Converter::to_color(style.Colors[ImGuiCol_ButtonHovered]);
    clicked_background_color_ = Converter::to_color(style.Colors[ImGuiCol_ButtonActive]);
    text_color_ = Converter::to_color(style.Colors[ImGuiCol_Text]);
}

void ButtonSimple::draw_impl() {
    auto &style = ImGui::GetStyle();
    
    auto default_idle_color = style.Colors[ImGuiCol_Button];
    auto default_hovered_color = style.Colors[ImGuiCol_ButtonHovered];
    auto default_clicked_color = style.Colors[ImGuiCol_ButtonActive];
    auto default_text_color = style.Colors[ImGuiCol_Text];
    
    style.Colors[ImGuiCol_Button] = Converter::to_imVec4(idle_background_color_);
    style.Colors[ImGuiCol_ButtonHovered] = Converter::to_imVec4(hovered_background_color_);
    style.Colors[ImGuiCol_ButtonActive] = Converter::to_imVec4(clicked_background_color_);
    style.Colors[ImGuiCol_Text] = Converter::to_imVec4(text_color_);
    
    if (ImGui::ButtonEx((label_ + widget_id_).c_str(),
                        Converter::to_imVec2(size_),
                        disabled_ ? ImGuiItemFlags_Disabled : 0))
        clicked_event_.invoke();
    
    style.Colors[ImGuiCol_Button] = default_idle_color;
    style.Colors[ImGuiCol_ButtonHovered] = default_hovered_color;
    style.Colors[ImGuiCol_ButtonActive] = default_clicked_color;
    style.Colors[ImGuiCol_Text] = default_text_color;
}

}