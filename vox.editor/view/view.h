//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.math/quaternion.h"
#include "vox.math/vector3.h"
#include "vox.render/material/base_material.h"
#include "vox.render/rendering/render_pipeline.h"
#include "vox.render/ui/widgets/panel_transformables/panel_window.h"
#include "vox.render/ui/widgets/visual/image.h"

namespace vox {
using namespace ui;

namespace editor::ui {
/**
 * Base class for any view
 */
class View : public PanelWindow {
public:
    View(const std::string &title,
         bool opened,
         const PanelWindowSettings &window_settings,
         RenderContext &render_context);

    /**
     * Update the view
     */
    virtual void Update(float delta_time);

    /**
     * Custom implementation of the draw method
     */
    void DrawImpl() override;

    /**
     * Render the view
     */
    virtual void Render(CommandBuffer &command_buffer) = 0;

    /**
     * Returns the size of the panel ignoring its title-bar height
     */
    [[nodiscard]] std::pair<uint16_t, uint16_t> SafeSize() const;

public:
    static ModelMeshPtr CreatePlane();

    /**
     * Returns the grid color of the view
     */
    [[nodiscard]] const Vector3F &GridColor() const;

    /**
     * Defines the grid color of the view
     * @param p_color p_color
     */
    void SetGridColor(const Vector3F &p_color);

protected:
    RenderContext &render_context_;
    Vector3F grid_color_ = {0.176f, 0.176f, 0.176f};

    ::vox::ui::Image *image_{nullptr};
    VkSamplerCreateInfo sampler_create_info_;
    std::unique_ptr<core::Sampler> sampler_{nullptr};
    std::unique_ptr<RenderTarget> render_target_{nullptr};
    std::unique_ptr<RenderPipeline> render_pipeline_{nullptr};

    std::unique_ptr<RenderTarget> CreateRenderTarget(uint32_t width,
                                                     uint32_t height,
                                                     VkFormat format = VK_FORMAT_UNDEFINED);
};

// MARK: - Grid
class GridMaterial : public BaseMaterial {
public:
    explicit GridMaterial(Device &device);
};

}  // namespace editor::ui
}  // namespace vox
