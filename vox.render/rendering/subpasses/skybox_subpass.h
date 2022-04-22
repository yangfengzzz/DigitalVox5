//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "rendering/subpass.h"
#include "shader/shader_source.h"

namespace vox {
/**
 * @brief This subpass is responsible for rendering a Skybox
 */
class SkyboxSubpass : public Subpass {
public:
    /**
     * @brief Constructs a subpass for the geometry pass of Deferred rendering
     * @param render_context Render context
     * @param scene Scene to render on this subpass
     * @param camera Camera used to look at the scene
     */
    SkyboxSubpass(RenderContext &render_context, Scene *scene, Camera *camera);
    
    ~SkyboxSubpass() override = default;
    
    void prepare() override;
    
    /**
     * @brief Record draw commands
     */
    void draw(CommandBuffer &command_buffer) override;
    
public:
    void create_sphere(float radius);
    
    void create_cuboid();
    
    /**
     * Texture cube map of the sky box material.
     */
    [[nodiscard]] const std::shared_ptr<Image> &texture_cube_map() const;
    
    void set_texture_cube_map(const std::shared_ptr<Image> &v);
    
private:
    enum class SkyBoxType {
        CUBOID, SPHERE
    } type_;
    
    ModelMeshPtr mesh_{nullptr};
    std::shared_ptr<Image> cube_map_{nullptr};
    std::unique_ptr<core::Buffer> vp_matrix_;
    
    ShaderSource vertex_source_;
    ShaderSource fragment_source_;
};

}
