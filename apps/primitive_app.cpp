//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "primitive_app.h"
#include "entity.h"
#include "mesh/primitive_mesh.h"
#include "mesh/mesh_renderer.h"
#include "material/unlit_material.h"
#include "material/blinn_phong_material.h"
#include "camera.h"
#include "controls/orbit_control.h"

namespace vox {
namespace {
class MoveScript : public Script {
public:
    explicit MoveScript(Entity *entity) : Script(entity) {}
    
    void on_update(float delta_time) override {
        r_tri_ += 90 * delta_time;
        entity()->transform_->set_rotation(0, r_tri_, 0);
    }
    
private:
    float r_tri_ = 0;
};

}

void PrimitiveApp::load_scene() {
    auto scene = scene_manager_->current_scene();
    scene->ambient_light()->set_diffuse_solid_color(Color(1, 1, 1));
    auto root_entity = scene->create_root_entity();
    
    auto camera_entity = root_entity->create_child();
    camera_entity->transform_->set_position(10, 10, 10);
    camera_entity->transform_->look_at(Point3F(0, 0, 0));
    main_camera_ = camera_entity->add_component<Camera>();
    camera_entity->add_component<control::OrbitControl>();
    
    // init point light
    auto light = root_entity->create_child("light");
    light->transform_->set_position(0, 3, 0);
    auto point_light = light->add_component<PointLight>();
    point_light->intensity_ = 0.3;
    
    auto cube_entity = root_entity->create_child();
    cube_entity->add_component<MoveScript>();
    auto renderer = cube_entity->add_component<MeshRenderer>();
    renderer->set_mesh(PrimitiveMesh::create_cuboid(*device_, 1));
    auto material = std::make_shared<BlinnPhongMaterial>(*device_);
    material->set_base_color(Color(0.4, 0.6, 0.6));
    renderer->set_material(material);
    
    auto plane_entity = root_entity->create_child();
    plane_entity->transform_->set_position(0, 5, 0);
    auto plane_renderer = plane_entity->add_component<MeshRenderer>();
    plane_renderer->set_mesh(PrimitiveMesh::create_sphere(*device_, 1));
    auto textured_material = std::make_shared<BlinnPhongMaterial>(*device_);
    textured_material->set_base_texture(ImageManager::get_singleton().load_texture("Textures/wood.png"));
    plane_renderer->set_material(textured_material);
    
    scene->play();
}

}
