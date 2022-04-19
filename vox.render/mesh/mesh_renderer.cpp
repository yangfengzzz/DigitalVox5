//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "mesh_renderer.h"
#include "mesh.h"
#include "shader/shader_common.h"
#include "entity.h"

namespace vox {
std::string MeshRenderer::name() {
    return "MeshRenderer";
}

MeshRenderer::MeshRenderer(Entity *entity) :
Renderer(entity) {
    
}

void MeshRenderer::set_mesh(const MeshPtr &mesh) {
    auto &last_mesh = mesh_;
    if (last_mesh != mesh) {
        if (last_mesh != nullptr) {
            mesh_update_flag_.reset();
        }
        if (mesh != nullptr) {
            mesh_update_flag_ = mesh->register_update_flag();
        }
        mesh_ = mesh;
    }
}

MeshPtr MeshRenderer::mesh() {
    return mesh_;
}

void MeshRenderer::render(std::vector<RenderElement> &opaque_queue,
                          std::vector<RenderElement> &alpha_test_queue,
                          std::vector<RenderElement> &transparent_queue) {
    if (mesh_ != nullptr) {
        if (mesh_update_flag_->flag_) {
            const auto &vertex_input_state = mesh_->vertex_input_state();
            
            shader_data_.remove_define("HAS_UV");
            shader_data_.remove_define("HAS_NORMAL");
            shader_data_.remove_define("HAS_TANGENT");
            shader_data_.remove_define("HAS_VERTEXCOLOR");
            
            for (auto attribute : vertex_input_state.attributes) {
                if (attribute.location == (uint32_t)Attributes::UV_0) {
                    shader_data_.add_define("HAS_UV");
                }
                if (attribute.location == (uint32_t)Attributes::NORMAL) {
                    shader_data_.add_define("HAS_NORMAL");
                }
                if (attribute.location == (uint32_t)Attributes::TANGENT) {
                    shader_data_.add_define("HAS_TANGENT");
                }
                if (attribute.location == (uint32_t)Attributes::COLOR_0) {
                    shader_data_.add_define("HAS_VERTEXCOLOR");
                }
            }
            mesh_update_flag_->flag_ = false;
        }
        
        auto &sub_meshes = mesh_->sub_meshes();
        for (size_t i = 0; i < sub_meshes.size(); i++) {
            MaterialPtr material;
            if (i < materials_.size()) {
                material = materials_[i];
            } else {
                material = nullptr;
            }
            if (material != nullptr) {
                RenderElement element(this, mesh_, &sub_meshes[i], material);
                push_primitive(element, opaque_queue, alpha_test_queue, transparent_queue);
            }
        }
    } else {
        assert(false && "mesh is nullptr.");
    }
}

void MeshRenderer::update_bounds(BoundingBox3F &world_bounds) {
    if (mesh_ != nullptr) {
        const auto kLocalBounds = mesh_->bounds_;
        const auto kWorldMatrix = entity_->transform_->world_matrix();
        world_bounds = kLocalBounds.transform(kWorldMatrix);
    } else {
        world_bounds.lowerCorner = Point3F(0, 0, 0);
        world_bounds.upperCorner = Point3F(0, 0, 0);
    }
}

//MARK: - Reflection
void MeshRenderer::on_serialize(nlohmann::json &data) {
    
}

void MeshRenderer::on_deserialize(const nlohmann::json &data) {
    
}

void MeshRenderer::on_inspector(ui::WidgetContainer &p_root) {
    
}

}
