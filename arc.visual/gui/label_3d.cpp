//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "gui/label_3d.h"

#include <string>

namespace vox::visualization::gui {

static const vox::Color DEFAULT_COLOR(0, 0, 0, 1);

struct Label3D::Impl {
    std::string text_;
    vox::Vector3F position_;
    vox::Color color_ = DEFAULT_COLOR;
    float scale_ = 1.f;
};

Label3D::Label3D(const vox::Vector3F& pos, const char* text /*= nullptr*/) : impl_(new Label3D::Impl()) {
    SetPosition(pos);
    if (text) {
        SetText(text);
    }
}

Label3D::~Label3D() = default;

const char* Label3D::GetText() const { return impl_->text_.c_str(); }

void Label3D::SetText(const char* text) { impl_->text_ = text; }

vox::Vector3F Label3D::GetPosition() const { return impl_->position_; }

void Label3D::SetPosition(const vox::Vector3F& pos) { impl_->position_ = pos; }

vox::Color Label3D::GetTextColor() const { return impl_->color_; }

void Label3D::SetTextColor(const vox::Color& color) { impl_->color_ = color; }

float Label3D::GetTextScale() const { return impl_->scale_; }

void Label3D::SetTextScale(float scale) { impl_->scale_ = scale; }

}  // namespace vox::visualization::gui
