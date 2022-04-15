//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef DIGITALVOX_VOX_RENDER_UI_WIDGETS_LAYOUT_TREE_NODE_H_
#define DIGITALVOX_VOX_RENDER_UI_WIDGETS_LAYOUT_TREE_NODE_H_

#include "ui/widgets/widget_container.h"
#include "ui/widgets/data_widget.h"
#include "event.h"

namespace vox::ui {
/**
 * Widget that allow columnification
 */
class TreeNode : public DataWidget<std::string>, public WidgetContainer {
public:
    /**
     * Constructor
     * @param p_name p_name
     * @param p_arrow_click_to_open p_arrowClickToOpen
     */
    explicit TreeNode(std::string p_name = "", bool p_arrow_click_to_open = false);
    
    /**
     * Open the tree node
     */
    void open();
    
    /**
     * Close the tree node
     */
    void close();
    
    /**
     * Returns true if the TreeNode is currently opened
     */
    [[nodiscard]] bool is_opened() const;
    
protected:
    void draw_impl() override;
    
public:
    std::string name_;
    bool selected_ = false;
    bool leaf_ = false;
    
    Event<> clicked_event_;
    Event<> double_clicked_event_;
    Event<> opened_event_;
    Event<> closed_event_;
    
private:
    bool arrow_click_to_open_ = false;
    bool should_open_ = false;
    bool should_close_ = false;
    bool opened_ = false;
};

}
#endif /* DIGITALVOX_VOX_RENDER_UI_WIDGETS_LAYOUT_TREE_NODE_H_ */
