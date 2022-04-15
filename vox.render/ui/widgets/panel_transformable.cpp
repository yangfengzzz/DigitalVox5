//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "panel_transformable.h"
#include "ui/widgets/converter.h"

namespace vox::ui {
PanelTransformable::PanelTransformable(const Vector2F &p_default_position,
                                       const Vector2F &p_default_size,
                                       HorizontalAlignment p_default_horizontal_alignment,
                                       VerticalAlignment p_default_vertical_alignment,
                                       bool p_ignore_config_file) :
default_position_(p_default_position),
default_size_(p_default_size),
default_horizontal_alignment_(p_default_horizontal_alignment),
default_vertical_alignment_(p_default_vertical_alignment),
ignore_config_file_(p_ignore_config_file) {
}

void PanelTransformable::set_position(const Vector2F &p_position) {
    position_ = p_position;
    position_changed_ = true;
}

void PanelTransformable::set_size(const Vector2F &p_size) {
    size_ = p_size;
    size_changed_ = true;
}

void PanelTransformable::set_alignment(HorizontalAlignment p_horizontal_alignment,
                                       VerticalAlignment p_vertical_aligment) {
    horizontal_alignment_ = p_horizontal_alignment;
    vertical_alignment_ = p_vertical_aligment;
    alignment_changed_ = true;
}

const Vector2F &PanelTransformable::position() const {
    return position_;
}

const Vector2F &PanelTransformable::size() const {
    return size_;
}

HorizontalAlignment PanelTransformable::horizontal_alignment() const {
    return horizontal_alignment_;
}

VerticalAlignment PanelTransformable::vertical_alignment() const {
    return vertical_alignment_;
}

void PanelTransformable::update_position() {
    if (default_position_.x != -1.f && default_position_.y != 1.f) {
        Vector2F offsetted_default_pos = default_position_ + calculate_position_alignment_offset(true);
        ImGui::SetWindowPos(Converter::to_imVec2(offsetted_default_pos),
                            ignore_config_file_ ? ImGuiCond_Once : ImGuiCond_FirstUseEver);
    }
    
    if (position_changed_ || alignment_changed_) {
        Vector2F offset = calculate_position_alignment_offset(false);
        Vector2F offsetted_pos(position_.x + offset.x, position_.y + offset.y);
        ImGui::SetWindowPos(Converter::to_imVec2(offsetted_pos), ImGuiCond_Always);
        position_changed_ = false;
        alignment_changed_ = false;
    }
}

void PanelTransformable::update_size() {
    /*
     if (_defaultSize.x != -1.f && _defaultSize.y != 1.f)
     ImGui::SetWindowSize(Internal::Converter::to_imVec2(_defaultSize), _ignoreConfigFile ? ImGuiCond_Once : ImGuiCond_FirstUseEver);
     */
    if (size_changed_) {
        ImGui::SetWindowSize(Converter::to_imVec2(size_), ImGuiCond_Always);
        size_changed_ = false;
    }
}

void PanelTransformable::copy_imGui_position() {
    position_ = Converter::to_vector2F(ImGui::GetWindowPos());
}

void PanelTransformable::copy_imGui_size() {
    size_ = Converter::to_vector2F(ImGui::GetWindowSize());
}

void PanelTransformable::update() {
    if (!first_frame_) {
        if (!auto_size_)
            update_size();
        copy_imGui_size();
        
        update_position();
        copy_imGui_position();
    }
    
    first_frame_ = false;
}

Vector2F PanelTransformable::calculate_position_alignment_offset(bool p_default) {
    Vector2F result(0.0f, 0.0f);
    
    switch (p_default ? default_horizontal_alignment_ : horizontal_alignment_) {
        case HorizontalAlignment::CENTER:result.x -= size_.x / 2.0f;
            break;
        case HorizontalAlignment::RIGHT:result.x -= size_.x;
            break;
        default:break;
    }
    
    switch (p_default ? default_vertical_alignment_ : vertical_alignment_) {
        case VerticalAlignment::MIDDLE:result.y -= size_.y / 2.0f;
            break;
        case VerticalAlignment::BOTTOM:result.y -= size_.y;
            break;
        default:break;
    }
    
    return result;
}

}
