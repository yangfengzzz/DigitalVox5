//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <memory>

#include "color.h"
#include "vector3.h"

namespace vox::visualization::gui {

// Label3D is a helper class for labels (like UI Labels) at 3D points as opposed
// to screen points. It is NOT a UI widget but is instead used via Open3DScene
// class. See Open3DScene::AddLabel/RemoveLabel.
class Label3D {
public:
    /// Copies text
    explicit Label3D(const vox::Vector3F& pos, const char* text = nullptr);
    ~Label3D();

    [[nodiscard]] const char* GetText() const;
    /// Sets the text of the label (copies text)
    void SetText(const char* text);

    [[nodiscard]] vox::Vector3F GetPosition() const;
    void SetPosition(const vox::Vector3F& pos);

    /// Returns the color with which the text will be drawn
    [[nodiscard]] vox::Color GetTextColor() const;

    /// Set the color with which the text will be drawn
    void SetTextColor(const vox::Color& color);

    /// Get the current scale. See not below on meaning of scale.
    [[nodiscard]] float GetTextScale() const;

    /// Sets the scale factor for the text sprite. It does not change the
    /// underlying font size but scales how large it appears. Warning: large
    /// scale factors may result in blurry text.
    void SetTextScale(float scale);

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

}  // namespace vox::visualization::gui
