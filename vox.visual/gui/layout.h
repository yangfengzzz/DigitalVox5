//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.visual/gui/widget.h"

namespace vox::visualization::gui {

struct Margins {
    int left;
    int top;
    int right;
    int bottom;

    /// Margins are specified in pixels, which are not the same size on all
    /// monitors. It is best to use a multiple of
    /// Window::GetTheme().fontSize to specify margins. Theme::fontSize,
    /// represents 1em and is scaled according to the scaling factor of the
    /// window. For example, 0.5em (that is, 0.5 * theme.fontSize) is typically
    /// a good size for a margin.
    Margins();  // all values zero
    explicit Margins(int px);
    Margins(int horiz_px, int vert_px);
    Margins(int left_px, int top_px, int right_px, int bottom_px);

    /// Convenience function that returns left + right
    [[nodiscard]] int GetHoriz() const;
    /// Convenience function that returns top + bottom
    [[nodiscard]] int GetVert() const;
};

/// Lays out widgets either horizontally or vertically.
/// Base class for Vert and Horiz.
class Layout1D : public Widget {
    using Super = Widget;

public:
    enum Dir { VERT, HORIZ };

    static void debug_PrintPreferredSizes(Layout1D* layout,
                                          const LayoutContext& context,
                                          const Constraints& constraints,
                                          int depth = 0);

    /// Spacing is in pixels; see the comment in Margin(). 1em is typically
    /// a good value for spacing.
    Layout1D(Dir dir, int spacing, const Margins& margins, const std::vector<std::shared_ptr<Widget>>& children);
    ~Layout1D() override;

    [[nodiscard]] int GetSpacing() const;
    [[nodiscard]] const Margins& GetMargins() const;
    /// Sets spacing. Need to signal a relayout after calling (unless it is
    /// before a layout that will happen, such as before adding as a child).
    void SetSpacing(int spacing);
    /// Sets margins. Need to signal a relayout after calling (unless it is
    /// before a layout that will happen, such as before adding as a child).
    void SetMargins(const Margins& margins);

    [[nodiscard]] Size CalcPreferredSize(const LayoutContext& context, const Constraints& constraints) const override;
    void Layout(const LayoutContext& context) override;

    /// Adds a fixed number of pixels after the previously added widget.
    void AddFixed(int size);
    /// Adds a virtual widget that takes up as much space as possible.
    /// This is useful for centering widgets: { stretch, w1, w2, stretch }
    /// or for aligning widgets to one side or the other:
    /// { stretch, ok, cancel }.
    void AddStretch();

public:
    class Fixed : public Widget {
    public:
        Fixed(int size, Dir dir);
        [[nodiscard]] Size CalcPreferredSize(const LayoutContext& context,
                                             const Constraints& constraints) const override;

    private:
        int size_;
        Dir dir_;
    };

    class Stretch : public Widget {
        [[nodiscard]] Size CalcPreferredSize(const LayoutContext& context,
                                             const Constraints& constraints) const override;
    };

protected:
    [[nodiscard]] int GetMinorAxisPreferredSize() const;
    void SetMinorAxisPreferredSize(int size);

    Margins& GetMutableMargins();
    [[nodiscard]] std::vector<std::shared_ptr<Widget>> GetVisibleChildren() const;

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

/// Lays out widgets vertically.
class Vert : public Layout1D {
public:
    static std::shared_ptr<Layout1D::Fixed> MakeFixed(int size);
    static std::shared_ptr<Layout1D::Stretch> MakeStretch();

    Vert();
    /// Spacing is in pixels; see the comment in Margin(). 1em is typically
    /// a good value for spacing.
    explicit Vert(int spacing, const Margins& margins = Margins());
    Vert(int spacing, const Margins& margins, const std::vector<std::shared_ptr<Widget>>& children);
    ~Vert() override;

    [[nodiscard]] int GetPreferredWidth() const;
    void SetPreferredWidth(int w);
};

/// This is a vertical layout with a twisty + title that can be clicked on
/// to expand or collapse the layout. Collapsing the layout will hide all
/// the items and shrink the size of the layout to the height of the title.
class CollapsableVert : public Vert {
    using Super = Vert;

public:
    explicit CollapsableVert(const char* text);
    CollapsableVert(const char* text, int spacing, const Margins& margins = Margins());
    ~CollapsableVert() override;

    /// You will need to call Window::SetNeedsLayout() after this.
    /// (If you call this before the widnows is displayed everything
    /// will work out fine, as layout will automatically be called when
    /// the window is shown.)
    void SetIsOpen(bool is_open);

    /// Returns true if open and false if collapsed.
    bool GetIsOpen();

    [[nodiscard]] FontId GetFontId() const;
    void SetFontId(FontId font_id);

    [[nodiscard]] Size CalcPreferredSize(const LayoutContext& context, const Constraints& constraints) const override;
    void Layout(const LayoutContext& context) override;
    Widget::DrawResult Draw(const DrawContext& context) override;

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

/// This a vertical layout that scrolls if it is smaller than its contents
class ScrollableVert : public Vert {
    using Super = Vert;

public:
    ScrollableVert();
    explicit ScrollableVert(int spacing, const Margins& margins = Margins());
    ScrollableVert(int spacing, const Margins& margins, const std::vector<std::shared_ptr<Widget>>& children);
    ~ScrollableVert() override;

    Widget::DrawResult Draw(const DrawContext& context) override;

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

/// Lays out widgets horizontally.
class Horiz : public Layout1D {
public:
    static std::shared_ptr<Layout1D::Fixed> MakeFixed(int size);
    static std::shared_ptr<Layout1D::Stretch> MakeStretch();
    static std::shared_ptr<Horiz> MakeCentered(std::shared_ptr<Widget> w);

    Horiz();
    /// Spacing is in pixels; see the comment in Margin(). 1em is typically
    /// a good value for spacing.
    explicit Horiz(int spacing, const Margins& margins = Margins());
    Horiz(int spacing, const Margins& margins, const std::vector<std::shared_ptr<Widget>>& children);
    ~Horiz() override;

    [[nodiscard]] int GetPreferredHeight() const;
    void SetPreferredHeight(int h);
};

/// Lays out widgets in a grid. The widgets are assigned to the next
/// horizontal column, and when all the columns in a row are used, a new
/// row will be created.
class VGrid : public Widget {
    using Super = Widget;

public:
    explicit VGrid(int num_cols, int spacing = 0, const Margins& margins = Margins());
    ~VGrid() override;

    [[nodiscard]] int GetSpacing() const;
    [[nodiscard]] const Margins& GetMargins() const;

    [[nodiscard]] int GetPreferredWidth() const;
    void SetPreferredWidth(int w);

    [[nodiscard]] Size CalcPreferredSize(const LayoutContext& context, const Constraints& constraints) const override;
    void Layout(const LayoutContext& context) override;

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

}  // namespace vox::visualization::gui
