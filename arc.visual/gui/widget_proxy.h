//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <memory>
#include <vector>

#include "gui/gui.h"
#include "gui/widget.h"

namespace vox::visualization::gui {

/// \class WidgetProxy
///
/// \brief Widget container to delegate any widget dynamically.
///
/// Widget can not be managed dynamically. Although it is allowed
/// to add more child widgets, it's impossible to replace some child
/// with new on or remove children. WidgetProxy is designed to solve
/// this problem.
///
/// When WidgetProxy is created, it's invisible and disabled, so it
/// won't be drawn or layout, seeming like it does not exist. When
/// a widget is set by \ref SetWidget, all \ref Widget's APIs will be
/// conducted to that child widget. It looks like WidgetProxy is
/// that widget.
///
/// At any time, a new widget could be set, to replace the old one.
/// and the old widget will be destroyed.
///
/// Due to the content changing after a new widget is set or cleared,
/// a relayout of Window might be called after SetWidget.
///
/// The delegated widget could be retrieved by \ref GetWidget in case you
/// need to access it directly, like get check status of a CheckBox.
///
/// API other than \ref SetWidget and \ref GetWidget has completely
/// same function as \ref Widget.
class WidgetProxy : public Widget {
public:
    WidgetProxy();
    ~WidgetProxy() override;

    void AddChild(std::shared_ptr<Widget> child) override;
    [[nodiscard]] std::vector<std::shared_ptr<Widget>> GetChildren() const override;

    /// \brief set a new widget to be delegated by this one.
    ///
    /// After SetWidget, the previously delegated widget will be abandon,
    /// all calls to \ref Widget's API will be conducted to \p widget.
    ///
    /// Before any SetWidget call, this widget is invisible and disabled,
    /// seems it does not exist because it won't be drawn or in a layout.
    ///
    /// \param widget Any widget to be delegated. Set to NULL to clear
    ///               current delegated proxy.
    virtual void SetWidget(std::shared_ptr<Widget> widget);

    /// \brief Retrieve current delegated widget.
    ///
    /// \return Instance of current delegated widget set by \ref SetWidget.
    ///         An empty pointer will be returned if there is none.
    virtual std::shared_ptr<Widget> GetWidget();

    [[nodiscard]] const Rect& GetFrame() const override;
    void SetFrame(const Rect& f) override;

    [[nodiscard]] const vox::Color& GetBackgroundColor() const override;
    [[nodiscard]] bool IsDefaultBackgroundColor() const override;
    void SetBackgroundColor(const vox::Color& color) override;

    [[nodiscard]] bool IsVisible() const override;
    void SetVisible(bool vis) override;

    [[nodiscard]] bool IsEnabled() const override;
    void SetEnabled(bool enabled) override;
    void SetTooltip(const char* text) override;
    [[nodiscard]] const char* GetTooltip() const override;
    [[nodiscard]] Size CalcPreferredSize(const LayoutContext& context, const Constraints& constraints) const override;
    [[nodiscard]] Size CalcMinimumSize(const LayoutContext& context) const override;
    void Layout(const LayoutContext& context) override;
    DrawResult Draw(const DrawContext& context) override;
    EventResult Mouse(const vox::MouseButtonInputEvent& e) override;
    EventResult Key(const vox::KeyInputEvent& e) override;
    DrawResult Tick(float dt) override;

protected:
    [[nodiscard]] virtual std::shared_ptr<Widget> GetActiveWidget() const;

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

}  // namespace vox::visualization::gui
