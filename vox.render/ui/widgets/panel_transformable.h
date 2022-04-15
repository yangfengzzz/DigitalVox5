//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef DIGITALVOX_VOX_RENDER_UI_WIDGETS_PANEL_TRANSFORMABLE_H_
#define DIGITALVOX_VOX_RENDER_UI_WIDGETS_PANEL_TRANSFORMABLE_H_

#include <vector>
#include <memory>

#include "vector2.h"
#include "event.h"

#include "ui/widgets/panel.h"
#include "ui/widgets/alignment.h"

namespace vox::ui {
/**
 * APanelTransformable is a panel that is localized in the canvas
 */
class PanelTransformable : public Panel {
public:
    /**
     * Create a APanelTransformable
     * @param p_default_position p_defaultPosition
     * @param p_default_size p_defaultSize
     * @param p_default_horizontal_alignment p_defaultHorizontalAlignment
     * @param p_default_vertical_alignment p_defaultVerticalAlignment
     * @param p_ignore_config_file p_ignoreConfigFile
     */
    explicit PanelTransformable(const Vector2F &p_default_position = Vector2F(-1.f, -1.f),
                                const Vector2F &p_default_size = Vector2F(-1.f, -1.f),
                                HorizontalAlignment p_default_horizontal_alignment = HorizontalAlignment::LEFT,
                                VerticalAlignment p_default_vertical_alignment = VerticalAlignment::TOP,
                                bool p_ignore_config_file = false);
    
    /**
     * Defines the position of the panel
     * @param p_position p_position
     */
    void set_position(const Vector2F &p_position);
    
    /**
     * Defines the size of the panel
     * @param p_size p_size
     */
    void set_size(const Vector2F &p_size);
    
    /**
     * Defines the alignment of the panel
     * @param p_horizontal_alignment p_horizontalAlignment
     * @param p_vertical_aligment p_verticalAligment
     */
    void set_alignment(HorizontalAlignment p_horizontal_alignment,
                       VerticalAlignment p_vertical_aligment);
    
    /**
     * Returns the current position of the panel
     */
    [[nodiscard]] const Vector2F &position() const;
    
    /**
     * Returns the current size of the panel
     */
    [[nodiscard]] const Vector2F &size() const;
    
    /**
     * Returns the current horizontal alignment of the panel
     */
    [[nodiscard]] HorizontalAlignment horizontal_alignment() const;
    
    /**
     * Returns the current vertical alignment of the panel
     */
    [[nodiscard]] VerticalAlignment vertical_alignment() const;
    
protected:
    void update();
    
    void draw_impl() override = 0;
    
private:
    Vector2F calculate_position_alignment_offset(bool p_default = false);
    
    void update_position();
    
    void update_size();
    
    void copy_imGui_position();
    
    void copy_imGui_size();
    
public:
    bool auto_size_ = true;
    
protected:
    Vector2F default_position_;
    Vector2F default_size_;
    HorizontalAlignment default_horizontal_alignment_;
    VerticalAlignment default_vertical_alignment_;
    bool ignore_config_file_;
    
    Vector2F position_ = Vector2F(0.0f, 0.0f);
    Vector2F size_ = Vector2F(0.0f, 0.0f);
    
    bool position_changed_ = false;
    bool size_changed_ = false;
    
    HorizontalAlignment horizontal_alignment_ = HorizontalAlignment::LEFT;
    VerticalAlignment vertical_alignment_ = VerticalAlignment::TOP;
    
    bool alignment_changed_ = false;
    bool first_frame_ = true;
};

}
#endif /* DIGITALVOX_VOX_RENDER_UI_WIDGETS_PANEL_TRANSFORMABLE_H_ */
