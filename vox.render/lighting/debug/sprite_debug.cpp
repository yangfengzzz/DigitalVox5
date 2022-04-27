//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "sprite_debug.h"
#include "shader/shader_manager.h"
#include "scene.h"
#include "entity.h"
#include "mesh/mesh_renderer.h"
#include "mesh/mesh_manager.h"
#include "lighting/light_manager.h"

namespace vox {
SpriteDebugMaterial::SpriteDebugMaterial(Device &device, bool is_spot_light) :
BaseMaterial(device) {
    set_is_transparent(true);
    set_blend_mode(BlendMode::ADDITIVE);
    
    vertex_source_ = ShaderManager::get_singleton().load_shader("base/spotlight_sprite_debug.vert");
    if (is_spot_light) {
        fragment_source_ = ShaderManager::get_singleton().load_shader("base/spotlight_sprite_debug.frag");
    } else {
        fragment_source_ = ShaderManager::get_singleton().load_shader("base/pointlight_sprite_debug.frag");
    }
}

std::string SpriteDebug::name() {
    return "SpriteDebug";
}

SpriteDebug::SpriteDebug(Entity *entity) :
Script(entity) {
    spot_light_mesh_ = MeshManager::get_singleton().load_mesh();
    spot_light_mesh_->add_sub_mesh(0, 4);
    spot_entity_ = entity->create_child();
    auto spot_renderer = spot_entity_->add_component<MeshRenderer>();
    spot_renderer->set_material(std::make_shared<SpriteDebugMaterial>(entity->scene()->device(), true));
    spot_renderer->set_mesh(spot_light_mesh_);
    
    point_light_mesh_ = MeshManager::get_singleton().load_mesh();
    point_light_mesh_->add_sub_mesh(0, 4);
    point_entity_ = entity->create_child();
    auto point_renderer = point_entity_->add_component<MeshRenderer>();
    point_renderer->set_material(std::make_shared<SpriteDebugMaterial>(entity->scene()->device(), false));
    point_renderer->set_mesh(point_light_mesh_);
}

void SpriteDebug::on_update(float delta_time) {
    auto spot_light_count = LightManager::get_singleton().spot_lights().size();
    if (spot_light_count > 0) {
        spot_light_mesh_->set_instance_count(static_cast<uint32_t>(spot_light_count));
        spot_entity_->set_is_active(true);
    } else {
        spot_entity_->set_is_active(false);
    }
    
    auto point_light_count = LightManager::get_singleton().point_lights().size();
    if (point_light_count > 0) {
        point_light_mesh_->set_instance_count(static_cast<uint32_t>(point_light_count));
        point_entity_->set_is_active(true);
    } else {
        point_entity_->set_is_active(false);
    }
}

}
