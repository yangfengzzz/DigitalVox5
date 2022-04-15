//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef DIGITALVOX_VOX_RENDER_UI_WIDGETS_TEXTS_TEXT_LABELLED_H_
#define DIGITALVOX_VOX_RENDER_UI_WIDGETS_TEXTS_TEXT_LABELLED_H_

#include "text.h"

namespace vox::ui {
/**
 * Simple widget to display a labelled text on a panel
 */
class TextLabelled : public Text {
public:
    /**
     * Constructor
     * @param p_content p_content
     * @param p_label p_label
     */
    explicit TextLabelled(const std::string &p_content = "", std::string p_label = "");
    
protected:
    void draw_impl() override;
    
public:
    std::string label_;
};

}
#endif /* DIGITALVOX_VOX_RENDER_UI_WIDGETS_TEXTS_TEXT_LABELLED_H_ */
