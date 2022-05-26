//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.visual/gui/widget_stack.h"

#include <stack>
#include <utility>

namespace vox::visualization::gui {
struct WidgetStack::Impl {
    std::stack<std::shared_ptr<Widget>> widgets_;
    std::function<void(std::shared_ptr<Widget>)> on_top_callback_;
};

WidgetStack::WidgetStack() : impl_(new WidgetStack::Impl()) {}
WidgetStack::~WidgetStack() = default;

void WidgetStack::PushWidget(const std::shared_ptr<Widget>& widget) {
    impl_->widgets_.push(widget);
    SetWidget(widget);
}

std::shared_ptr<Widget> WidgetStack::PopWidget() {
    std::shared_ptr<Widget> ret;
    if (!impl_->widgets_.empty()) {
        ret = impl_->widgets_.top();
        impl_->widgets_.pop();
        if (!impl_->widgets_.empty()) {
            SetWidget(impl_->widgets_.top());
            if (impl_->on_top_callback_) {
                impl_->on_top_callback_(impl_->widgets_.top());
            }
        } else {
            SetWidget(nullptr);
        }
    }
    return ret;
}
void WidgetStack::SetOnTop(std::function<void(std::shared_ptr<Widget>)> onTopCallback) {
    impl_->on_top_callback_ = std::move(onTopCallback);
}

}  // namespace vox::visualization::gui
