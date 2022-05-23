//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <volk.h>

#include "vox.math/vector2.h"
#include "vox.render/ui/widgets/widget.h"

namespace vox::ui {
/**
 * Simple widget that display an image
 */
class Image : public Widget {
public:
    Image(VkDescriptorSet texture_id, const Vector2F &size);

    void SetTextureView(VkDescriptorSet texture_id);

protected:
    void DrawImpl() override;

public:
    VkDescriptorSet texture_id_;
    Vector2F size_;
};

}  // namespace vox::ui
