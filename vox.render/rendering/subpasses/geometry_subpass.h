//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.render/rendering/subpass.h"

namespace vox {
/**
 * @brief This subpass is responsible for rendering a Scene
 */
class GeometrySubpass : public Subpass {
public:
    enum class RenderMode { AUTO, MANUAL };

    /**
     * @brief Constructs a subpass for the geometry pass of Deferred rendering
     * @param render_context Render context
     * @param scene Scene to render on this subpass
     * @param camera Camera used to look at the scene
     */
    GeometrySubpass(RenderContext &render_context, Scene *scene, Camera *camera);

    ~GeometrySubpass() override = default;

    void Prepare() override;

    /**
     * @brief Record draw commands
     */
    void Draw(CommandBuffer &command_buffer) override;

    /**
     * @brief Thread index to use for allocating resources
     */
    void SetThreadIndex(uint32_t index);

public:
    [[nodiscard]] RenderMode GetRenderMode() const;

    void SetRenderMode(RenderMode mode);

    void AddRenderElement(const RenderElement &element);

    void ClearAllRenderElement();

protected:
    void DrawElement(CommandBuffer &command_buffer,
                     const std::vector<RenderElement> &items,
                     const ShaderVariant &variant);

    uint32_t thread_index_{0};

    RenderMode mode_ = RenderMode::AUTO;
    std::vector<RenderElement> elements_{};
};

}  // namespace vox
