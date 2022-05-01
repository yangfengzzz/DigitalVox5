//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "ui/widgets/panel_transformables/panel_window.h"
#include "ui/widgets/visual/image.h"
#include "vector3.h"
#include "quaternion.h"
#include "rendering/render_pipeline.h"
#include "material/base_material.h"

namespace vox {
using namespace ui;

namespace editor::ui {
/**
 * Base class for any view
 */
class View : public PanelWindow {
public:
    View(const std::string &title, bool opened,
         const PanelWindowSettings &window_settings,
         RenderContext &render_context);
    
    /**
     * Update the view
     */
    virtual void update(float delta_time);
    
    /**
     * Custom implementation of the draw method
     */
    void draw_impl() override;
    
    /**
     * Render the view
     */
    virtual void render(CommandBuffer &command_buffer) = 0;
    
    /**
     * Returns the size of the panel ignoring its title-bar height
     */
    [[nodiscard]] std::pair<uint16_t, uint16_t> safe_size() const;
    
public:
    static ModelMeshPtr create_plane();
    
    /**
     * Returns the grid color of the view
     */
    [[nodiscard]] const Vector3F &grid_color() const;
    
    /**
     * Defines the grid color of the view
     * @param p_color p_color
     */
    void set_grid_color(const Vector3F &p_color);
    
protected:
    RenderContext &render_context_;
    Vector3F grid_color_ = {0.176f, 0.176f, 0.176f};
    
    ::vox::ui::Image *image_{nullptr};
    VkSamplerCreateInfo sampler_create_info_;
    std::unique_ptr<core::Sampler> sampler_{nullptr};
    std::unique_ptr<RenderTarget> render_target_{nullptr};
    std::unique_ptr<RenderPipeline> render_pipeline_{nullptr};
    
    std::unique_ptr<RenderTarget> create_render_target(uint32_t width, uint32_t height,
                                                       VkFormat format = VK_FORMAT_UNDEFINED);
};

//MARK: - Grid
class GridMaterial : public BaseMaterial {
public:
    explicit GridMaterial(Device &device);
};

}
}
