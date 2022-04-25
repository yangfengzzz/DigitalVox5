//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "rendering/subpass.h"

namespace vox {
class ShadowSubpass: public Subpass {
public:
    ShadowSubpass(RenderContext &render_context,
                  Scene* scene,
                  Camera* camera);
        
    void setViewport(const std::optional<Vector4F>& viewport);
    
    void prepare() override;
    
    void draw(CommandBuffer &command_buffer) override;
    
private:
    
};

}
