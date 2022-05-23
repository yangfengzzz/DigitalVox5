//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/ui/widgets/visual/image.h"

#include "vox.render/ui/widgets/converter.h"

namespace vox::ui {
Image::Image(VkDescriptorSet texture_id, const Vector2F &size) : texture_id_{texture_id}, size_(size) {}

void Image::SetTextureView(VkDescriptorSet texture_id) { texture_id_ = texture_id; }

void Image::DrawImpl() { ImGui::Image(texture_id_, Converter::ToImVec2(size_), ImVec2(0.f, 0.f), ImVec2(1.f, 1.f)); }

}  // namespace vox::ui
