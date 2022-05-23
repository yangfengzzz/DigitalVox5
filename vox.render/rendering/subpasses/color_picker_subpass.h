//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.render/material/base_material.h"
#include "vox.render/rendering/subpass.h"
#include "vox.render/shader/shader_source.h"

namespace vox {
class ColorPickerMaterial : public BaseMaterial {
public:
    explicit ColorPickerMaterial(Device &device);
};

/**
 * @brief This subpass is responsible for rendering a ColorPicker
 */
class ColorPickerSubpass : public Subpass {
public:
    /**
     * @brief Constructs a subpass for the geometry pass of Deferred rendering
     * @param render_context Render context
     * @param scene Scene to render on this subpass
     * @param camera Camera used to look at the scene
     */
    ColorPickerSubpass(RenderContext &render_context, Scene *scene, Camera *camera);

    ~ColorPickerSubpass() override = default;

    void Prepare() override;

    /**
     * @brief Record draw commands
     */
    void Draw(CommandBuffer &command_buffer) override;

public:
    void AddExclusiveRenderer(Renderer *renderer);

    void ClearExclusiveList();

public:
    /**
     * Convert id to RGB color value, 0 and 0xffffff are illegal values.
     */
    static Color IdToColor(uint32_t id);

    /**
     * Convert RGB color to id.
     * @param color - Color
     */
    static uint32_t ColorToId(const std::array<uint8_t, 4> &color);

    /**
     * Get renderer element by color.
     */
    std::pair<Renderer *, MeshPtr> GetObjectByColor(const std::array<uint8_t, 4> &color);

private:
    void DrawElement(CommandBuffer &command_buffer,
                     const std::vector<RenderElement> &items,
                     const ShaderVariant &variant);

    uint32_t current_id_ = 0;
    std::unordered_map<size_t, std::pair<Renderer *, MeshPtr>> primitives_map_;

    ColorPickerMaterial material_;

    std::vector<Renderer *> exclusive_list_;
};

}  // namespace vox
