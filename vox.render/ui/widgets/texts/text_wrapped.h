//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef DIGITALVOX_VOX_RENDER_UI_WIDGETS_TEXTS_TEXT_WRAPPED_H_
#define DIGITALVOX_VOX_RENDER_UI_WIDGETS_TEXTS_TEXT_WRAPPED_H_

#include "text.h"

namespace vox::ui {
/**
 * Simple widget to display a long text with word-wrap on a panel
 */
class TextWrapped : public Text {
public:
    /**
     * Constructor
     * @param p_content p_content
     */
    explicit TextWrapped(const std::string &p_content = "");
    
protected:
    void draw_impl() override;
};

}
#endif /* DIGITALVOX_VOX_RENDER_UI_WIDGETS_TEXTS_TEXT_WRAPPED_H_ */
