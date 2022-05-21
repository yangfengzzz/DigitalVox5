//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

// Include FileDialog here to get value of GUI_USE_NATIVE_FILE_DIALOG
#include "gui/file_dialog.h"

#if defined(__APPLE__) && GUI_USE_NATIVE_FILE_DIALOG

#include <utility>
#include <vector>

#include "gui/native.h"

namespace vox::visualization::gui {

struct FileDialog::Impl {
    FileDialog::Mode mode_;
    std::string path_;
    std::vector<std::pair<std::string, std::string>> filters_;
    std::function<void()> on_cancel_;
    std::function<void(const char *)> on_done_;
};

FileDialog::FileDialog(Mode mode, const char *title, const Theme &theme)
    : Dialog(title), impl_(new FileDialog::Impl()) {
    impl_->mode_ = mode;
}

FileDialog::~FileDialog() = default;

void FileDialog::SetPath(const char *path) { impl_->path_ = path; }

void FileDialog::AddFilter(const char *filter, const char *description) {
    impl_->filters_.push_back(std::make_pair<std::string, std::string>(filter, description));
}

void FileDialog::SetOnCancel(std::function<void()> on_cancel) { impl_->on_cancel_ = std::move(on_cancel); }

void FileDialog::SetOnDone(std::function<void(const char *)> on_done) { impl_->on_done_ = std::move(on_done); }

Size FileDialog::CalcPreferredSize(const LayoutContext &context, const Constraints &constraints) const {
    return {0, 0};
}

void FileDialog::OnWillShow() {
    auto on_ok = [this](const char *path) { this->impl_->on_done_(path); };
    auto on_cancel = [this]() { this->impl_->on_cancel_(); };
    ShowNativeFileDialog(impl_->mode_, impl_->path_, impl_->filters_, on_ok, on_cancel);
}

void FileDialog::OnDone() {}

}  // namespace vox::visualization::gui

#endif  // __APPLE__ && GUI_USE_NATIVE_FILE_DIALOG
