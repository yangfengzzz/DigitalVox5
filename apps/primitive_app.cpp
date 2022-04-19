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
#include "image/stb_img.h"

namespace vox {
namespace {
class MoveScript : public Script {
public:
    MoveScript(Entity* entity): Script(entity) {}
    
    void on_update(float deltaTime) override {
        _rTri += 90 * deltaTime;
        entity()->transform_->set_rotation(0, _rTri, 0);
    }
    
private:
    float _rTri = 0;
};

}

void PrimitiveApp::load_scene() {
    auto scene = scene_manager_->current_scene();
    scene->ambient_light()->set_diffuse_solid_color(Color(1, 1, 1));
    auto rootEntity = scene->create_root_entity();
    
    auto cameraEntity = rootEntity->create_child();
    cameraEntity->transform_->set_position(10, 10, 10);
    cameraEntity->transform_->look_at(Point3F(0, 0, 0));
    main_camera_ = cameraEntity->add_component<Camera>();
    cameraEntity->add_component<control::OrbitControl>();
    
    // init point light
    auto light = rootEntity->create_child("light");
    light->transform_->set_position(0, 3, 0);
    auto pointLight = light->add_component<PointLight>();
    pointLight->intensity_ = 0.3;
    
    auto cubeEntity = rootEntity->create_child();
    cubeEntity->add_component<MoveScript>();
    auto renderer = cubeEntity->add_component<MeshRenderer>();
    renderer->set_mesh(PrimitiveMesh::create_cuboid(*device_, 1));
    auto material = std::make_shared<BlinnPhongMaterial>(*device_);
    material->set_base_color(Color(0.4, 0.6, 0.6));
    renderer->set_material(material);
    
    auto planeEntity = rootEntity->create_child();
    planeEntity->transform_->set_position(0, 5, 0);
    auto planeRenderer = planeEntity->add_component<MeshRenderer>();
    planeRenderer->set_mesh(PrimitiveMesh::create_sphere(*device_, 1));
    auto texturedMaterial = std::make_shared<UnlitMaterial>(*device_);
    texturedMaterial->set_base_texture(ImageManager::get_singleton().load_texture("Textures/wood.png"));
    planeRenderer->set_material(texturedMaterial);
    
    scene->play();
}

}
