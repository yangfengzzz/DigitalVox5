//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <functional>
#include <memory>

#include "vox.visual/gui/widget_proxy.h"

namespace vox::visualization::gui {

/// \brief WidgetStack manages multiple widgets in a stack.
///
/// WidgetStack, like \ref WidgetProxy, delegates at most one widget. Unlike
/// \ref WidgetProxy, it maintains multiple widgets. It saves all widgets
/// pushed into by \ref SetWidget and always shows the top one. The
/// WidgetStack is a subclass of \ref WidgetProxy, in another word, the topmost
/// widget will delegate it self to WidgetStack. \ref PopWidget will remove
/// the topmost widget and callback set by \ref SetOnTop taking the new
/// topmost widget will be called. The WidgetStack disappears in GUI if there
/// is no widget in stack.
///
/// Due to content and layout changing of the new widget, after \ref SetWidget
/// or \ref PopWidget the relayout of Window might need be called.
class WidgetStack : public WidgetProxy {
public:
    WidgetStack();
    ~WidgetStack() override;

    /// \brief Push a widget into stack so the it be the topmost widget.
    ///
    /// After a widget is pushed into stack, it will be shown in the GUI.
    /// Due to content and layout changing of the new widget, the relayout
    /// of Window might need be called.
    /// \param widget Widget to push into stack to be topmost showing wiget.
    void PushWidget(const std::shared_ptr<Widget>& widget);

    /// \brief Pop the top most widget.
    ///
    /// Due to content and layout changing of the new widget, the relayout
    /// of Window might need be called.
    /// \param widget Widget to push into stack to be topmost showing wiget.
    /// \return widget is popped, or NULL if no widget in the stack.
    std::shared_ptr<Widget> PopWidget();

    /// \brief Setup a callback while a widget is popped out and a new widget
    /// becomes the topmost one.
    ///
    /// Parameter of callback will be the new topmost widget.
    /// \param onTopCallback Callback function taking the new topmost widget.
    void SetOnTop(std::function<void(std::shared_ptr<Widget>)> onTopCallback);

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

}  // namespace vox::visualization::gui
