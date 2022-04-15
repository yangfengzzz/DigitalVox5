//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef DIGITALVOX_VOX_RENDER_UI_WIDGETS_TEXTS_TEXT_DISABLED_H_
#define DIGITALVOX_VOX_RENDER_UI_WIDGETS_TEXTS_TEXT_DISABLED_H_

#include "text.h"

namespace vox::ui {
/**
 * Widget to display a disabled text on a panel
 */
class TextDisabled : public Text {
public:
    /**
     * Constructor
     * @param p_content p_content
     */
    explicit TextDisabled(const std::string &p_content = "");
    
protected:
    void draw_impl() override;
};

}
#endif /* DIGITALVOX_VOX_RENDER_UI_WIDGETS_TEXTS_TEXT_DISABLED_H_ */
