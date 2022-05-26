//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <functional>

#include "vox.visual/gui/dialog.h"

#define GUI_USE_NATIVE_FILE_DIALOG 1

namespace vox::visualization::gui {

struct Theme;

class FileDialog : public Dialog {
    using Super = Dialog;

public:
    enum class Mode { OPEN, SAVE, OPEN_DIR };

    FileDialog(Mode type, const char *title, const Theme &theme);
    ~FileDialog() override;

    /// May either be a directory or a file. If path is a file, it will be
    /// selected if it exists. Defaults to current working directory if
    /// no path is specified.
    void SetPath(const char *path);

    /// 'filter' is a string of extensions separated by a space or comma.
    /// An empty filter string matches all extensions.
    ///    AddFilter(".jpg .png .gif", "Image file (.jpg, .png, .gif)")
    ///    AddFilter(".jpg", "JPEG image (.jpg)")
    ///    AddFilter("", "All files")
    void AddFilter(const char *filter, const char *description);

    /// The OnCancel and OnDone callbacks *must* be specified.
    void SetOnCancel(std::function<void()> on_cancel);
    /// The OnCancel and OnDone callbacks *must* be specified.
    void SetOnDone(std::function<void(const char *)> on_done);

    [[nodiscard]] Size CalcPreferredSize(const LayoutContext &context, const Constraints &constraints) const override;

    void OnWillShow() override;

protected:
    void OnDone();

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

}  // namespace vox::visualization::gui
