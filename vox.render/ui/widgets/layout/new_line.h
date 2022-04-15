//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef DIGITALVOX_VOX_RENDER_UI_WIDGETS_LAYOUT_NEW_LINE_H_
#define DIGITALVOX_VOX_RENDER_UI_WIDGETS_LAYOUT_NEW_LINE_H_

#include "ui/widgets/widget.h"

namespace vox::ui {
/**
 * Widget that adds an empty line to the panel
 */
class NewLine : public Widget {
protected:
    void draw_impl() override;
};

}
#endif /* DIGITALVOX_VOX_RENDER_UI_WIDGETS_LAYOUT_NEW_LINE_H_ */
