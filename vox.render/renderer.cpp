//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/renderer.h"

#include "vox.render/components_manager.h"
#include "vox.render/entity.h"
#include "vox.render/material/material.h"
#include "vox.render/scene.h"

namespace vox {
size_t Renderer::MaterialCount() { return materials_.size(); }

BoundingBox3F Renderer::Bounds() {
    auto &change_flag = transform_change_flag_;
    if (change_flag->flag_) {
        UpdateBounds(bounds_);
        change_flag->flag_ = false;
    }
    return bounds_;
}

Renderer::Renderer(Entity *entity)
    : Component(entity),
      shader_data_(entity->Scene()->Device()),
      transform_change_flag_(entity->transform->RegisterWorldChangeFlag()),
      renderer_property_("rendererData") {}

void Renderer::OnEnable() { ComponentsManager::GetSingleton().AddRenderer(this); }

void Renderer::OnDisable() { ComponentsManager::GetSingleton().RemoveRenderer(this); }

MaterialPtr Renderer::GetInstanceMaterial(size_t index) {
    const auto &materials = materials_;
    if (materials.size() > index) {
        const auto &material = materials[index];
        if (material != nullptr) {
            if (materials_instanced_[index]) {
                return material;
            } else {
                return CreateInstanceMaterial(material, index);
            }
        }
    }
    return nullptr;
}

MaterialPtr Renderer::GetMaterial(size_t index) { return materials_[index]; }

void Renderer::SetMaterial(const MaterialPtr &material) {
    size_t index = 0;

    if (index >= materials_.size()) {
        materials_.reserve(index + 1);
        for (size_t i = materials_.size(); i <= index; i++) {
            materials_.push_back(nullptr);
        }
    }

    const auto &internal_material = materials_[index];
    if (internal_material != material) {
        materials_[index] = material;
        if (index < materials_instanced_.size()) {
            materials_instanced_[index] = false;
        }
    }
}

void Renderer::SetMaterial(size_t index, const MaterialPtr &material) {
    if (index >= materials_.size()) {
        materials_.reserve(index + 1);
        for (size_t i = materials_.size(); i <= index; i++) {
            materials_.push_back(nullptr);
        }
    }

    const auto &internal_material = materials_[index];
    if (internal_material != material) {
        materials_[index] = material;
        if (index < materials_instanced_.size()) {
            materials_instanced_[index] = false;
        }
    }
}

std::vector<MaterialPtr> Renderer::GetInstanceMaterials() {
    for (size_t i = 0; i < materials_.size(); i++) {
        if (!materials_instanced_[i]) {
            CreateInstanceMaterial(materials_[i], i);
        }
    }
    return materials_;
}

std::vector<MaterialPtr> Renderer::GetMaterials() { return materials_; }

void Renderer::SetMaterials(const std::vector<MaterialPtr> &materials) {
    size_t count = materials.size();
    if (materials_.size() != count) {
        materials_.reserve(count);
        for (size_t i = materials_.size(); i < count; i++) {
            materials_.push_back(nullptr);
        }
    }
    if (!materials_instanced_.empty()) {
        materials_instanced_.clear();
    }

    for (size_t i = 0; i < count; i++) {
        const auto &internal_material = materials_[i];
        const auto &material = materials[i];
        if (internal_material != material) {
            materials_[i] = material;
        }
    }
}

void Renderer::PushPrimitive(const RenderElement &element,
                             std::vector<RenderElement> &opaque_queue,
                             std::vector<RenderElement> &alpha_test_queue,
                             std::vector<RenderElement> &transparent_queue) {
    const auto kRenderQueueType = element.material->render_queue_;

    if (kRenderQueueType > (RenderQueueType::TRANSPARENT + RenderQueueType::ALPHA_TEST) >> 1) {
        transparent_queue.push_back(element);
    } else if (kRenderQueueType > (RenderQueueType::ALPHA_TEST + RenderQueueType::OPAQUE) >> 1) {
        alpha_test_queue.push_back(element);
    } else {
        opaque_queue.push_back(element);
    }
}

void Renderer::SetDistanceForSort(float dist) { distance_for_sort_ = dist; }

float Renderer::DistanceForSort() const { return distance_for_sort_; }

void Renderer::UpdateShaderData() {
    auto world_matrix = GetEntity()->transform->WorldMatrix();
    normal_matrix_ = world_matrix.inverse();
    normal_matrix_ = normal_matrix_.transposed();

    renderer_data_.local_mat = GetEntity()->transform->LocalMatrix();
    renderer_data_.model_mat = world_matrix;
    renderer_data_.normal_mat = normal_matrix_;
    shader_data_.SetData(Renderer::renderer_property_, renderer_data_);
}

MaterialPtr Renderer::CreateInstanceMaterial(const MaterialPtr &material, size_t index) { return nullptr; }

}  // namespace vox
