//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.render/renderer.h"

namespace vox {
class MeshRenderer : public Renderer {
public:
    /**
     * Returns the name of the component
     */
    std::string name() override;

    explicit MeshRenderer(Entity *entity);

    /**
     * Mesh assigned to the renderer.
     */
    void SetMesh(const MeshPtr &mesh);

    MeshPtr Mesh();

private:
    void Render(std::vector<RenderElement> &opaque_queue,
                std::vector<RenderElement> &alpha_test_queue,
                std::vector<RenderElement> &transparent_queue) override;

    void UpdateBounds(BoundingBox3F &world_bounds) override;

public:
    /**
     * Called when the serialization is asked
     */
    void OnSerialize(nlohmann::json &data) override;

    /**
     * Called when the deserialization is asked
     */
    void OnDeserialize(const nlohmann::json &data) override;

    /**
     * Defines how the component should be drawn in the inspector
     */
    void OnInspector(ui::WidgetContainer &p_root) override;

private:
    MeshPtr mesh_;
    std::unique_ptr<UpdateFlag> mesh_update_flag_;
};

}  // namespace vox
