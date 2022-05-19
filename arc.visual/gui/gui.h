//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

namespace arc {
namespace visualization {
namespace gui {

struct Point {
    int x;
    int y;

    Point();
    Point(int x_, int y_);
};

struct Size {
    int width;
    int height;

    Size();
    Size(int w, int h);
};

struct Rect {
    int x;
    int y;
    int width;
    int height;

    Rect();
    Rect(int x_, int y_, int w_, int h_);

    int GetTop() const;
    int GetBottom() const;
    int GetLeft() const;
    int GetRight() const;

    bool Contains(int x, int y) const;
    bool Contains(const Point& pt) const;

    Rect UnionedWith(const Rect& r) const;

    bool operator==(const Rect& other) const;
    bool operator!=(const Rect& other) const;
};

enum class BorderShape { NONE = 0, RECT, ROUNDED_RECT };

enum class Alignment : unsigned int {
    LEFT = 1,
    HCENTER = 2,
    RIGHT = 3,
    TOP = (1 << 4),
    VCENTER = (2 << 4),
    BOTTOM = (3 << 4),
    CENTER = (2 | (2 << 4))
};
constexpr Alignment operator|(Alignment x, Alignment y) {
    return Alignment((unsigned int)(x) | (unsigned int)(y));
}

using FontId = unsigned int;

enum class FontStyle {
    NORMAL = 0,
    BOLD = 1,
    ITALIC = 2,
    BOLD_ITALIC = 3  /// BOLD | ITALIC
};

class FontContext {
public:
    virtual ~FontContext(){};

    virtual void* GetFont(FontId font_id) = 0;
};

}  // namespace gui
}  // namespace visualization
}  // namespace arc
