//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "gui_app.h"
#include "mesh/mesh_renderer.h"
#include "material/unlit_material.h"
#include "camera.h"
#include "controls/orbit_control.h"
#include "ui/widgets/texts/text_labelled.h"

namespace vox {
void GuiApp::load_scene() {
    gui_->set_canvas(canvas_);
    canvas_.add_panel(panel_);
    panel_.create_widget<ui::TextLabelled>("hello", "world");
    
    auto scene = scene_manager_->current_scene();
    auto root_entity = scene->create_root_entity();
    
    auto camera_entity = root_entity->create_child();
    camera_entity->transform_->set_position(10, 10, 10);
    camera_entity->transform_->look_at(Point3F(0, 0, 0));
    main_camera_ = camera_entity->add_component<Camera>();
    camera_entity->add_component<control::OrbitControl>();
    
    scene->play();
}

}
