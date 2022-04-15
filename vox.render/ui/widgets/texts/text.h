//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef DIGITALVOX_VOX_RENDER_UI_WIDGETS_TEXTS_TEXT_H_
#define DIGITALVOX_VOX_RENDER_UI_WIDGETS_TEXTS_TEXT_H_

#include "ui/widgets/data_widget.h"

namespace vox::ui {
/**
 * Simple widget to display text on a panel
 */
class Text : public DataWidget<std::string> {
public:
    /**
     * Constructor
     * @param p_content p_content
     */
    explicit Text(std::string p_content = "");
    
protected:
    void draw_impl() override;
    
public:
    std::string content_;
};

}
#endif /* DIGITALVOX_VOX_RENDER_UI_WIDGETS_TEXTS_TEXT_H_ */
