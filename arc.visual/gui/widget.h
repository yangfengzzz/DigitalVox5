//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <memory>
#include <vector>

#include "color.h"
#include "core/command_buffer.h"
#include "gui/gui.h"
#include "platform/input_events.h"

namespace arc::visualization::gui {
struct Theme;

struct LayoutContext {
    const Theme& theme;
    FontContext& fonts;
};

struct DrawContext {
    const Theme& theme;
    vox::CommandBuffer& command_buffer;
    FontContext& fonts;
    int uiOffsetX;
    int uiOffsetY;
    int screenWidth;
    int screenHeight;
    int emPx;
    float frameDelta;  // in seconds
};

class Widget {
public:
    Widget();
    explicit Widget(const std::vector<std::shared_ptr<Widget>>& children);
    virtual ~Widget();

    virtual void AddChild(std::shared_ptr<Widget> child);
    [[nodiscard]] virtual std::vector<std::shared_ptr<Widget>> GetChildren() const;

    /// Returns the frame size in pixels.
    [[nodiscard]] virtual const Rect& GetFrame() const;
    /// The frame is in pixels. The size of a pixel varies on different
    /// and operating systems now frequently scale text sizes on high DPI
    /// monitors. Prefer using a Layout to using this function, but if you
    /// must use it, it is best to use a multiple of
    /// Window::GetTheme().fontSize, which represents 1em and is scaled
    /// according to the scaling factor of the window.
    virtual void SetFrame(const Rect& f);

    [[nodiscard]] virtual const vox::Color& GetBackgroundColor() const;
    [[nodiscard]] virtual bool IsDefaultBackgroundColor() const;
    virtual void SetBackgroundColor(const vox::Color& color);

    [[nodiscard]] virtual bool IsVisible() const;
    virtual void SetVisible(bool vis);

    [[nodiscard]] virtual bool IsEnabled() const;
    virtual void SetEnabled(bool enabled);

    virtual void SetTooltip(const char* text);
    [[nodiscard]] virtual const char* GetTooltip() const;

    static constexpr int DIM_GROW = 10000;
    struct Constraints {
        int width = DIM_GROW;
        int height = DIM_GROW;
    };
    [[nodiscard]] virtual Size CalcPreferredSize(const LayoutContext& context, const Constraints& constraints) const;

    [[nodiscard]] virtual Size CalcMinimumSize(const LayoutContext& context) const;

    virtual void Layout(const LayoutContext& context);

    enum class DrawResult { NONE, REDRAW, RELAYOUT };
    /// Draws the widget. If this is a Dear ImGUI widget, this is where
    /// the actual event processing happens. Return NONE if no action
    /// needs to be taken, REDRAW if the widget needs to be redrawn
    /// (e.g. its value changed), and RELAYOUT if the widget wishes to
    /// change size.
    virtual DrawResult Draw(const DrawContext& context);

    enum class EventResult { IGNORED, CONSUMED, DISCARD };

    /// Widgets that use Dear ImGUI should not need to override this,
    /// as Dear ImGUI will take care of all the mouse handling during
    /// the Draw().
    virtual EventResult Mouse(const vox::MouseButtonInputEvent& e);

    /// Widgets that use Dear ImGUI should not need to override this,
    /// as Dear ImGUI will take care of all the key handling during
    /// the Draw().
    virtual EventResult Key(const vox::KeyInputEvent& e);

    /// Tick events are sent regularly and allow for things like smoothly
    /// moving the camera based on keys that are pressed, or animations.
    /// Return DrawResult::REDRAW if you want to be redrawn.
    virtual DrawResult Tick(float dt);

protected:
    void DrawImGuiPushEnabledState();
    void DrawImGuiPopEnabledState();
    void DrawImGuiTooltip();

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

}  // namespace arc::visualization::gui
