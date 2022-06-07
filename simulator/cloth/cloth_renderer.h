//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.cloth/NvCloth/Cloth.h"
#include "vox.cloth/NvClothExt/ClothMeshDesc.h"
#include "vox.render/renderer.h"

namespace vox::cloth {
class ClothRenderer : public Renderer {
public:
    struct Vertex {
        physx::PxVec3 position;
        physx::PxVec3 normal;
    };

    nv::cloth::Cloth* cloth_{nullptr};

    /**
     * Returns the name of the component
     */
    std::string name() override;

    explicit ClothRenderer(Entity* entity);

    void Render(std::vector<RenderElement>& opaque_queue,
                std::vector<RenderElement>& alpha_test_queue,
                std::vector<RenderElement>& transparent_queue) override;

    void SetClothMeshDesc(const nv::cloth::ClothMeshDesc& desc);

    void Update(const physx::PxVec3* positions, uint32_t num_vertices);

    void UpdateBounds(BoundingBox3F& world_bounds) override;

public:
    /**
     * Called when the serialization is asked
     */
    void OnSerialize(nlohmann::json& data) override;

    /**
     * Called when the deserialization is asked
     */
    void OnDeserialize(const nlohmann::json& data) override;

    /**
     * Defines how the component should be drawn in the inspector
     */
    void OnInspector(ui::WidgetContainer& p_root) override;

private:
    void Initialize(void* vertices, uint32_t num_vertices, uint32_t vertex_size, uint16_t* faces, uint32_t num_faces);

    std::vector<Vertex> vertices_;
    std::vector<uint16_t> indices_;
    std::vector<uint32_t> submesh_offsets_;

    uint32_t num_faces_{};
    uint32_t num_vertices_{};
    uint32_t vertex_size_{};

    std::unique_ptr<core::Buffer> vertex_buffers_{nullptr};
    VertexInputState vertex_input_state_;

    MeshPtr mesh_{nullptr};
};

}  // namespace vox::cloth
