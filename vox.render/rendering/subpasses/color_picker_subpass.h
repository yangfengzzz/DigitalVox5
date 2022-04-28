//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "rendering/subpass.h"
#include "shader/shader_source.h"
#include "material/base_material.h"

namespace vox {
class ColorPickerMaterial : public BaseMaterial {
public:
    ColorPickerMaterial(Device &device);
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
    
    void prepare() override;
    
    /**
     * @brief Record draw commands
     */
    void draw(CommandBuffer &command_buffer) override;
    
public:
    /**
     * Convert id to RGB color value, 0 and 0xffffff are illegal values.
     */
    static Vector3F id_to_color(uint32_t id);
    
    /**
     * Convert RGB color to id.
     * @param color - Color
     */
    static uint32_t color_to_id(const std::array<uint8_t, 4> &color);
    
    /**
     * Get renderer element by color.
     */
    std::pair<Renderer *, MeshPtr> get_object_by_color(const std::array<uint8_t, 4> &color);
    
private:
    void draw_element(CommandBuffer &command_buffer,
                      const std::vector<RenderElement> &items,
                      const ShaderVariant &variant);
    
    uint32_t current_id_ = 0;
    std::unordered_map<size_t, std::pair<Renderer *, MeshPtr>> primitives_map_;
    
    ColorPickerMaterial material_;
};

}

