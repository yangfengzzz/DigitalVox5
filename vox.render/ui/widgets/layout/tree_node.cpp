//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "tree_node.h"

#include <utility>

namespace vox::ui {
TreeNode::TreeNode(std::string name, bool arrow_click_to_open) :
DataWidget(name_),
name_(std::move(name)),
arrow_click_to_open_(arrow_click_to_open) {
    auto_execute_plugins_ = false;
}

void TreeNode::open() {
    should_open_ = true;
    should_close_ = false;
}

void TreeNode::close() {
    should_close_ = true;
    should_open_ = false;
}

bool TreeNode::is_opened() const {
    return opened_;
}

void TreeNode::draw_impl() {
    bool prev_opened = opened_;
    
    if (should_open_) {
        ImGui::SetNextItemOpen(true);
        should_open_ = false;
    } else if (should_close_) {
        ImGui::SetNextItemOpen(false);
        should_close_ = false;
    }
    
    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_None;
    if (arrow_click_to_open_) flags |= ImGuiTreeNodeFlags_OpenOnArrow;
    if (selected_) flags |= ImGuiTreeNodeFlags_Selected;
    if (leaf_) flags |= ImGuiTreeNodeFlags_Leaf;
    
    bool opened = ImGui::TreeNodeEx((name_ + widget_id_).c_str(), flags);
    
    if (ImGui::IsItemClicked()
        && (ImGui::GetMousePos().x - ImGui::GetItemRectMin().x) > ImGui::GetTreeNodeToLabelSpacing()) {
        clicked_event_.invoke();
        
        if (ImGui::IsMouseDoubleClicked(0)) {
            double_clicked_event_.invoke();
        }
    }
    
    if (opened) {
        if (!prev_opened)
            opened_event_.invoke();
        
        opened_ = true;
        
        execute_plugins(); // Manually execute plugins to make plugins considering the TreeNode and no childs
        
        draw_widgets();
        
        ImGui::TreePop();
    } else {
        if (prev_opened)
            closed_event_.invoke();
        
        opened_ = false;
        
        execute_plugins(); // Manually execute plugins to make plugins considering the TreeNode and no childs
    }
}

}
