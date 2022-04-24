//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "ibl_app.h"
#include "mesh/primitive_mesh.h"
#include "mesh/mesh_renderer.h"
#include "material/pbr_material.h"
#include "rendering/subpasses/skybox_subpass.h"
#include "camera.h"
#include "image_manager.h"

namespace vox {
bool IBLApp::prepare(Platform &platform) {
    ForwardApplication::prepare(platform);
    
    auto scene = scene_manager_->current_scene();
    auto skybox = std::make_unique<SkyboxSubpass>(*render_context_,
                                                  scene, main_camera_);
    skybox->create_cuboid();
    skybox->flip_vertically();
    skybox->set_texture_cube_map(ImageManager::get_singleton().load_texture_cubemap("Textures/uffizi_rgba16f_cube.ktx"));
    render_pipeline_->add_subpass(std::move(skybox));
    
    return true;
}

void IBLApp::load_scene() {
}

}
