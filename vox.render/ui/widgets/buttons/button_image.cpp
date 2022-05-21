//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "button_image.h"

#include "ui/widgets/converter.h"

namespace vox::ui {
ButtonImage::ButtonImage(VkDescriptorSet texture_id, const Vector2F &size) : texture_id_{texture_id}, size_(size) {}

void ButtonImage::DrawImpl() {
    ImVec4 bg = Converter::ToImVec4(background_);
    ImVec4 tn = Converter::ToImVec4(tint_);

    if (ImGui::ImageButton((ImTextureID)texture_id_, Converter::ToImVec2(size_), ImVec2(0.f, 0.f), ImVec2(1.f, 1.f), -1,
                           bg, tn))
        clicked_event_.Invoke();
}

}  // namespace vox::ui
