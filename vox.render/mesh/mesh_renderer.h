//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "renderer.h"

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
    void set_mesh(const MeshPtr &mesh);
    
    MeshPtr mesh();
    
private:
    void render(std::vector<RenderElement> &opaque_queue,
                std::vector<RenderElement> &alpha_test_queue,
                std::vector<RenderElement> &transparent_queue) override;
    
    void update_bounds(BoundingBox3F &world_bounds) override;
    
public:
    /**
     * Called when the serialization is asked
     */
    void on_serialize(nlohmann::json &data) override;
    
    /**
     * Called when the deserialization is asked
     */
    void on_deserialize(const nlohmann::json &data) override;
    
    /**
     * Defines how the component should be drawn in the inspector
     */
    void on_inspector(ui::WidgetContainer &p_root) override;
    
private:
    MeshPtr mesh_;
    std::unique_ptr<UpdateFlag> mesh_update_flag_;
};

}
