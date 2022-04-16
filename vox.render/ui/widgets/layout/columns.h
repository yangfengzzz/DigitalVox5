//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef DIGITALVOX_VOX_RENDER_UI_WIDGETS_LAYOUT_COLUMNS_H_
#define DIGITALVOX_VOX_RENDER_UI_WIDGETS_LAYOUT_COLUMNS_H_

#include <array>

#include "ui/widgets/widget_container.h"

namespace vox::ui {
/**
 * Widget that allow columnification
 */
template<size_t Size>
class Columns : public Widget, public WidgetContainer {
public:
    /**
     * Constructor
     */
    Columns() {
        widths_.fill(-1.f);
    }
    
protected:
    void draw_impl() override {
        ImGui::Columns(static_cast<int>(Size), ("##" + widget_id_).c_str(), false);
        
        int counter = 0;
        
        collect_garbage();
        
        for (auto it = widgets_.begin(); it != widgets_.end();) {
            it->first->draw();
            
            ++it;
            
            if (it != widgets_.end()) {
                if (widths_[counter] != -1.f)
                    ImGui::SetColumnWidth(counter, widths_[counter]);
                
                ImGui::NextColumn();
            }
            
            ++counter;
            
            if (counter == Size)
                counter = 0;
        }
        
        ImGui::Columns(1); // Necessary to not break the layout for following widget
    }
    
public:
    std::array<float, Size> widths_;
};

}
#endif /* DIGITALVOX_VOX_RENDER_UI_WIDGETS_LAYOUT_COLUMNS_H_ */
