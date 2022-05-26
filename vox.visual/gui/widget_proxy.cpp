//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "widget_proxy.h"

#include <utility>

#include "vox.math/color.h"

namespace vox::visualization::gui {
struct WidgetProxy::Impl {
    std::shared_ptr<Widget> widget_;
    bool need_layout_ = false;
};

WidgetProxy::WidgetProxy() : impl_(new WidgetProxy::Impl()) {}
WidgetProxy::~WidgetProxy() = default;

std::shared_ptr<Widget> WidgetProxy::GetActiveWidget() const { return impl_->widget_; }
void WidgetProxy::SetWidget(std::shared_ptr<Widget> widget) {
    impl_->widget_ = std::move(widget);
    impl_->need_layout_ = true;
}
std::shared_ptr<Widget> WidgetProxy::GetWidget() { return GetActiveWidget(); }
void WidgetProxy::AddChild(std::shared_ptr<Widget> child) {
    auto widget = GetActiveWidget();
    if (widget) {
        widget->AddChild(child);
    }
}

std::vector<std::shared_ptr<Widget>> WidgetProxy::GetChildren() const {
    auto widget = GetActiveWidget();
    if (widget) {
        return widget->GetChildren();
    }
    return Widget::GetChildren();
}

const Rect& WidgetProxy::GetFrame() const {
    auto widget = GetActiveWidget();
    if (widget) {
        return widget->GetFrame();
    }
    return Widget::GetFrame();
}

void WidgetProxy::SetFrame(const Rect& f) {
    auto widget = GetActiveWidget();
    if (widget) {
        widget->SetFrame(f);
    }
    Widget::SetFrame(f);
}

const vox::Color& WidgetProxy::GetBackgroundColor() const {
    auto widget = GetActiveWidget();
    if (widget) {
        return widget->GetBackgroundColor();
    }
    return Widget::GetBackgroundColor();
}

bool WidgetProxy::IsDefaultBackgroundColor() const {
    auto widget = GetActiveWidget();
    if (widget) {
        return widget->IsDefaultBackgroundColor();
    }
    return Widget::IsDefaultBackgroundColor();
}

void WidgetProxy::SetBackgroundColor(const vox::Color& color) {
    auto widget = GetActiveWidget();
    if (widget) {
        widget->SetBackgroundColor(color);
    }
    Widget::SetBackgroundColor(color);
}

bool WidgetProxy::IsVisible() const {
    auto widget = GetActiveWidget();
    if (widget) {
        return Widget::IsVisible() && widget->IsVisible();
    }
    return false;
}

void WidgetProxy::SetVisible(bool vis) {
    auto widget = GetActiveWidget();
    if (widget) {
        widget->SetVisible(vis);
    }
}

bool WidgetProxy::IsEnabled() const {
    auto widget = GetActiveWidget();
    if (widget) {
        return Widget::IsEnabled() && widget->IsEnabled();
    }
    return false;
}

void WidgetProxy::SetEnabled(bool enabled) {
    auto widget = GetActiveWidget();
    if (widget) {
        widget->SetEnabled(enabled);
    }
}

void WidgetProxy::SetTooltip(const char* text) {
    auto widget = GetActiveWidget();
    if (widget) {
        widget->SetTooltip(text);
    }
    Widget::SetTooltip(text);
}

const char* WidgetProxy::GetTooltip() const {
    auto widget = GetActiveWidget();
    if (widget) {
        return widget->GetTooltip();
    }
    return Widget::GetTooltip();
}

Size WidgetProxy::CalcPreferredSize(const LayoutContext& context, const Constraints& constraints) const {
    auto widget = GetActiveWidget();
    if (widget) {
        return widget->CalcPreferredSize(context, constraints);
    }
    return Widget::CalcPreferredSize(context, constraints);
}

Size WidgetProxy::CalcMinimumSize(const LayoutContext& context) const {
    auto widget = GetActiveWidget();
    if (widget) {
        return widget->CalcMinimumSize(context);
    }
    return Widget::CalcMinimumSize(context);
}

void WidgetProxy::Layout(const LayoutContext& context) {
    auto widget = GetActiveWidget();
    if (widget) {
        widget->Layout(context);
    }
}

Widget::DrawResult WidgetProxy::Draw(const DrawContext& context) {
    if (!IsVisible()) {
        return DrawResult::NONE;
    }

    DrawResult result = DrawResult::NONE;
    auto widget = GetActiveWidget();
    if (widget) {
        result = widget->Draw(context);
    }
    if (impl_->need_layout_) {
        impl_->need_layout_ = false;
        result = DrawResult::RELAYOUT;
    }
    return result;
}

Widget::EventResult WidgetProxy::Mouse(const vox::MouseButtonInputEvent& e) {
    if (!IsVisible()) {
        return EventResult::IGNORED;
    }
    auto widget = GetActiveWidget();
    if (widget) {
        return widget->Mouse(e);
    }
    return EventResult::DISCARD;
}

Widget::EventResult WidgetProxy::Key(const vox::KeyInputEvent& e) {
    if (!IsVisible()) {
        return EventResult::IGNORED;
    }
    auto widget = GetActiveWidget();
    if (widget) {
        return widget->Key(e);
    }
    return EventResult::DISCARD;
}

Widget::DrawResult WidgetProxy::Tick(float dt) {
    auto result = DrawResult::NONE;
    auto widget = GetActiveWidget();
    if (widget) {
        result = widget->Tick(dt);
    }
    return result;
}

}  // namespace vox::visualization::gui