//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "renderer.h"
#include "entity.h"
#include "scene.h"
#include "material/material.h"
#include "components_manager.h"

namespace vox {
size_t Renderer::material_count() {
    return materials_.size();
}

BoundingBox3F Renderer::bounds() {
    auto &change_flag = transform_change_flag_;
    if (change_flag->flag_) {
        update_bounds(bounds_);
        change_flag->flag_ = false;
    }
    return bounds_;
}

Renderer::Renderer(Entity *entity) :
Component(entity),
shader_data_(entity->scene()->device()),
transform_change_flag_(entity->transform_->register_world_change_flag()),
local_matrix_property_("u_localMat"),
world_matrix_property_("u_modelMat"),
normal_matrix_property_("u_normalMat") {
}

void Renderer::on_enable() {
    ComponentsManager::get_singleton().add_renderer(this);
}

void Renderer::on_disable() {
    ComponentsManager::get_singleton().remove_renderer(this);
}

MaterialPtr Renderer::get_instance_material(size_t index) {
    const auto &materials = materials_;
    if (materials.size() > index) {
        const auto &material = materials[index];
        if (material != nullptr) {
            if (materials_instanced_[index]) {
                return material;
            } else {
                return create_instance_material(material, index);
            }
        }
    }
    return nullptr;
}

MaterialPtr Renderer::get_material(size_t index) {
    return materials_[index];
}

void Renderer::set_material(const MaterialPtr &material) {
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

void Renderer::set_material(size_t index, const MaterialPtr &material) {
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

std::vector<MaterialPtr> Renderer::get_instance_materials() {
    for (size_t i = 0; i < materials_.size(); i++) {
        if (!materials_instanced_[i]) {
            create_instance_material(materials_[i], i);
        }
    }
    return materials_;
}

std::vector<MaterialPtr> Renderer::get_materials() {
    return materials_;
}

void Renderer::set_materials(const std::vector<MaterialPtr> &materials) {
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

void Renderer::push_primitive(const RenderElement &element,
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

void Renderer::set_distance_for_sort(float dist) {
    distance_for_sort_ = dist;
}

float Renderer::distance_for_sort() const {
    return distance_for_sort_;
}

void Renderer::update_shader_data() {
    auto world_matrix = entity()->transform_->world_matrix();
    normal_matrix_ = world_matrix.inverse();
    normal_matrix_ = normal_matrix_.transposed();
    
    shader_data_.set_data(Renderer::local_matrix_property_, entity()->transform_->local_matrix());
    shader_data_.set_data(Renderer::world_matrix_property_, world_matrix);
    shader_data_.set_data(Renderer::normal_matrix_property_, normal_matrix_);
}

MaterialPtr Renderer::create_instance_material(const MaterialPtr &material, size_t index) {
    return nullptr;
}

}
