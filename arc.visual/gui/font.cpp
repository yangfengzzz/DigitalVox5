//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "gui/font.h"

namespace vox::visualization::gui {

// assigned in header in constexpr declaration, but still need to be defined
constexpr const char *FontDescription::SANS_SERIF;
constexpr const char *FontDescription::MONOSPACE;

FontDescription::FontDescription(const char *typeface,
                                 FontStyle style /*= FontStyle::NORMAL*/,
                                 int point_size /*= 0*/) {
    ranges_.push_back({typeface, "en", {}});
    style_ = style;
    point_size_ = point_size;
}

void FontDescription::AddTypefaceForLanguage(const char *typeface, const char *lang) {
    ranges_.push_back({typeface, lang, {}});
}

void FontDescription::AddTypefaceForCodePoints(const char *typeface, const std::vector<uint32_t> &code_points) {
    ranges_.push_back({typeface, "", code_points});
}

}  // namespace vox::visualization::gui
