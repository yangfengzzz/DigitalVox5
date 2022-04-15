//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "image.h"
#include "ui/widgets/converter.h"

namespace vox::ui {
Image::Image(VkDescriptorSet p_texture_id, const Vector2F &p_size) :
texture_id_{p_texture_id}, size_(p_size) {
    
}

void Image::set_texture_view(VkDescriptorSet p_texture_id) {
    texture_id_ = p_texture_id;
}

void Image::draw_impl() {
    ImGui::Image(texture_id_, Converter::to_imVec2(size_),
                 ImVec2(0.f, 0.f), ImVec2(1.f, 1.f));
}

}