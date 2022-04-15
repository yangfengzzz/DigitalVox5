//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef DIGITALVOX_VOX_RENDER_UI_WIDGETS_PANEL_TRANSFORMABLES_PANEL_WINDOW_H_
#define DIGITALVOX_VOX_RENDER_UI_WIDGETS_PANEL_TRANSFORMABLES_PANEL_WINDOW_H_

#include "ui/widgets/panel_transformable.h"
#include "ui/widgets/panel_window_settings.h"
#include "event.h"

namespace vox::ui {
/**
 * A PanelWindow is a panel that is localized and behave like a window (Movable, resizable...)
 */
class PanelWindow : public PanelTransformable {
public:
    /**
     * Creates the PanelWindow
     * @param p_name p_name
     * @param p_opened p_opened
     * @param p_panel_settings p_panelSettings
     */
    explicit PanelWindow(std::string p_name = "",
                         bool p_opened = true,
                         const PanelWindowSettings &p_panel_settings = PanelWindowSettings{});
    
    /**
     * Open (show) the panel
     */
    void open();
    
    /**
     * Close (hide) the panel
     */
    void close();
    
    /**
     * Focus the panel
     */
    void focus();
    
    /**
     * Defines the opened state of the window
     * @param p_value p_value
     */
    void set_opened(bool p_value);
    
    /**
     * Returns true if the panel is opened
     */
    [[nodiscard]] bool is_opened() const;
    
    /**
     * Returns true if the panel is hovered
     */
    [[nodiscard]] bool is_hovered() const;
    
    /**
     * Returns true if the panel is focused
     */
    [[nodiscard]] bool is_focused() const;
    
    /**
     * Returns true if the panel is appearing
     */
    [[nodiscard]] bool is_appearing() const;
    
    /**
     * Scrolls to the bottom of the window
     */
    void scroll_to_bottom();
    
    /**
     * Scrolls to the top of the window
     */
    void scroll_to_top();
    
    /**
     * Returns true if the window is scrolled to the bottom
     */
    [[nodiscard]] bool is_scrolled_to_bottom() const;
    
    /**
     * Returns true if the window is scrolled to the bottom
     */
    [[nodiscard]] bool is_scrolled_to_top() const;
    
protected:
    void draw_impl() override;
    
public:
    std::string name_;
    
    Vector2F min_size_ = {0.f, 0.f};
    Vector2F max_size_ = {0.f, 0.f};
    
    bool resizable_ = true;
    bool closable_ = false;
    bool movable_ = true;
    bool scrollable_ = true;
    bool dockable_ = false;
    bool hide_background_ = false;
    bool force_horizontal_scrollbar_ = false;
    bool force_vertical_scrollbar_ = false;
    bool allow_horizontal_scrollbar_ = false;
    bool bring_to_front_on_focus_ = true;
    bool collapsable_ = false;
    bool allow_inputs_ = true;
    bool title_bar_ = true;
    
    Event<> open_event_;
    Event<> close_event_;
    
private:
    bool opened_;
    bool hovered_{};
    bool focused_{};
    bool must_scroll_to_bottom_ = false;
    bool must_scroll_to_top_ = false;
    bool scrolled_to_bottom_ = false;
    bool scrolled_to_top_ = false;
};

}
#endif /* DIGITALVOX_VOX_RENDER_UI_WIDGETS_PANEL_TRANSFORMABLES_PANEL_WINDOW_H_ */
