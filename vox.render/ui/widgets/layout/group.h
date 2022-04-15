//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef DIGITALVOX_VOX_RENDER_UI_WIDGETS_LAYOUT_GROUP_H_
#define DIGITALVOX_VOX_RENDER_UI_WIDGETS_LAYOUT_GROUP_H_

#include "ui/widgets/widget_container.h"

namespace vox::ui {
/**
 * Widget that can contains other widgets
 */
class Group : public Widget, public WidgetContainer {
protected:
    void draw_impl() override;
};

}
#endif /* DIGITALVOX_VOX_RENDER_UI_WIDGETS_LAYOUT_GROUP_H_ */
