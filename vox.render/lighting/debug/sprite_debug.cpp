//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/lighting/debug/sprite_debug.h"

#include "vox.render/entity.h"
#include "vox.render/lighting/light_manager.h"
#include "vox.render/mesh/mesh_manager.h"
#include "vox.render/mesh/mesh_renderer.h"
#include "vox.render/scene.h"
#include "vox.render/shader/shader_manager.h"

namespace vox {
SpriteDebugMaterial::SpriteDebugMaterial(Device &device) : BaseMaterial(device) {
    SetIsTransparent(true);
    SetBlendMode(BlendMode::ADDITIVE);
    input_assembly_state_.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;

    vertex_source_ = ShaderManager::GetSingleton().LoadShader("base/light/light_sprite.vert");
    fragment_source_ = ShaderManager::GetSingleton().LoadShader("base/light/light_sprite.frag");
}

void SpriteDebugMaterial::SetIsSpotLight(bool value) {
    if (value) {
        shader_data_.AddDefine("IS_SPOT_LIGHT");
    }
}

// MARK: - SpriteDebug
std::string SpriteDebug::name() { return "SpriteDebug"; }

SpriteDebug::SpriteDebug(Entity *entity) : Script(entity) {
    spot_light_mesh_ = MeshManager::GetSingleton().LoadBufferMesh();
    spot_light_mesh_->AddSubMesh(0, 4);
    spot_entity_ = entity->CreateChild();
    auto spot_renderer = spot_entity_->AddComponent<MeshRenderer>();
    auto mtl = std::make_shared<SpriteDebugMaterial>(entity->Scene()->Device());
    mtl->SetIsSpotLight(true);
    spot_renderer->SetMaterial(mtl);
    spot_renderer->SetMesh(spot_light_mesh_);

    point_light_mesh_ = MeshManager::GetSingleton().LoadBufferMesh();
    point_light_mesh_->AddSubMesh(0, 4);
    point_entity_ = entity->CreateChild();
    auto point_renderer = point_entity_->AddComponent<MeshRenderer>();
    point_renderer->SetMaterial(std::make_shared<SpriteDebugMaterial>(entity->Scene()->Device()));
    point_renderer->SetMesh(point_light_mesh_);
}

void SpriteDebug::OnUpdate(float delta_time) {
    auto spot_light_count = LightManager::GetSingleton().SpotLights().size();
    if (spot_light_count > 0) {
        spot_light_mesh_->SetInstanceCount(static_cast<uint32_t>(spot_light_count));
        spot_entity_->SetIsActive(true);
    } else {
        spot_entity_->SetIsActive(false);
    }

    auto point_light_count = LightManager::GetSingleton().PointLights().size();
    if (point_light_count > 0) {
        point_light_mesh_->SetInstanceCount(static_cast<uint32_t>(point_light_count));
        point_entity_->SetIsActive(true);
    } else {
        point_entity_->SetIsActive(false);
    }
}

}  // namespace vox
