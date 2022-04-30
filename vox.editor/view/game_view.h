//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "view.h"

namespace vox {
using namespace ui;

namespace editor::ui {
class GameView : public View {
public:
    GameView(const std::string &title, bool opened,
             const PanelWindowSettings &window_settings,
             RenderContext &render_context, Scene *scene);
    
    /**
     * Update the scene view
     */
    void update(float delta_time) override;
    
    /**
     * Custom implementation of the render method
     */
    void render(CommandBuffer &command_buffer) override;
    
    void load_scene(Entity *root_entity);
    
private:
    Camera *main_camera_{nullptr};
    Scene *scene_{nullptr};
};

}
}
