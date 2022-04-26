//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "cascade_shadowmap_app.h"
#include "mesh/primitive_mesh.h"
#include "mesh/mesh_renderer.h"
#include "material/blinn_phong_material.h"
#include "camera.h"
#include "controls/orbit_control.h"
#include "lighting/direct_light.h"

namespace vox {
namespace {
class ShadowDebugMaterial : public BaseMaterial {
public:
    ShadowDebugMaterial(Device &device):BaseMaterial(device, "") {
        vertex_source_ = ShaderManager::get_singleton().load_shader("base/blinn-phong.vert");
        fragment_source_ = ShaderManager::get_singleton().load_shader("base/cascade-shadow-debugger.frag");
    }
};

} // namespace

void CascadeShadowMapApp::load_scene() {
    auto scene = scene_manager_->current_scene();
    auto root_entity = scene->create_root_entity();
    
    auto camera_entity = root_entity->create_child();
    camera_entity->transform_->set_position(0, 10, 50);
    camera_entity->transform_->look_at(Point3F(0, 0, 0));
    main_camera_ = camera_entity->add_component<Camera>();
    camera_entity->add_component<control::OrbitControl>();
    
    auto light = root_entity->create_child("light");
    light->transform_->set_position(10, 10, 0);
    light->transform_->look_at(Point3F());
    auto direct_light = light->add_component<DirectLight>();
    direct_light->intensity_ = 1.0;
    direct_light->set_enable_shadow(true);
    
    // create box test entity
    float cube_size = 2.0;
    auto box_mesh = PrimitiveMesh::create_cuboid(cube_size, cube_size, cube_size);
    auto box_mtl = std::make_shared<BlinnPhongMaterial>(*device_);
    box_mtl->set_base_color(Color(0.3, 0.3, 0.3, 0.5));
    for (int i = 0; i < 40; i++) {
        auto box_entity = root_entity->create_child("BoxEntity");
        box_entity->transform_->set_position(Point3F(0, 2, i * 10 - 200));
        
        auto box_renderer = box_entity->add_component<MeshRenderer>();
        box_renderer->set_mesh(box_mesh);
        box_renderer->set_material(box_mtl);
        box_renderer->cast_shadow_ = true;
    }
    
    auto plane_entity = root_entity->create_child("PlaneEntity");
    auto plane_mtl = std::make_shared<BlinnPhongMaterial>(*device_);
    plane_mtl->set_base_color(Color(1.0, 0, 0, 1.0));
    plane_mtl->set_render_face(RenderFace::DOUBLE);
    
    auto shadow_debug = std::make_shared<ShadowDebugMaterial>(*device_);
    
    auto plane_renderer = plane_entity->add_component<MeshRenderer>();
    plane_renderer->set_mesh(PrimitiveMesh::create_plane(10, 400));
    plane_renderer->set_material(plane_mtl);
    // planeRenderer->setMaterial(shadowDebug);
    plane_renderer->receive_shadow_ = true;
    
    scene->play();
}

}
