//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.visual/gui/widget.h"

namespace vox::visualization::gui {

class Window;

/// Base class for dialogs.
class Dialog : public Widget {
    using Super = Widget;

public:
    explicit Dialog(const char* title);
    ~Dialog() override;

    [[nodiscard]] Size CalcPreferredSize(const LayoutContext& context, const Constraints& constraints) const override;
    void Layout(const LayoutContext& context) override;
    DrawResult Draw(const DrawContext& context) override;

    virtual void OnWillShow();

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

}  // namespace vox::visualization::gui
