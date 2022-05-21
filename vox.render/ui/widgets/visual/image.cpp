//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "image.h"

#include "ui/widgets/converter.h"

namespace vox::ui {
Image::Image(VkDescriptorSet texture_id, const Vector2F &size) : texture_id_{texture_id}, size_(size) {}

void Image::set_texture_view(VkDescriptorSet texture_id) { texture_id_ = texture_id; }

void Image::draw_impl() { ImGui::Image(texture_id_, Converter::to_imVec2(size_), ImVec2(0.f, 0.f), ImVec2(1.f, 1.f)); }

}  // namespace vox::ui
