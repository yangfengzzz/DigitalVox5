//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef DIGITALVOX_VOX_RENDER_UI_PLUGINS_DRAG_DROP_TARGET_H_
#define DIGITALVOX_VOX_RENDER_UI_PLUGINS_DRAG_DROP_TARGET_H_

#include "event.h"
#include "plugin.h"
#include <string>
#include <utility>
#include <imgui.h>

namespace vox::ui {
/**
 * Represents a drag and drop target
 */
template<typename T>
class DDTarget : public Plugin {
public:
    /**
     * Create the drag and drop target
     * @param p_identifier p_identifier
     */
    explicit DDTarget(std::string p_identifier) : identifier_(std::move(p_identifier)) {
    }
    
    /**
     * Execute the drag and drop target behaviour
     */
    void execute() override {
        if (ImGui::BeginDragDropTarget()) {
            if (!is_hovered_)
                hover_start_event_.invoke();
            
            is_hovered_ = true;
            
            ImGuiDragDropFlags target_flags = 0;
            // Don't wait until the delivery (release mouse button on a target) to do something
            // target_flags |= ImGuiDragDropFlags_AcceptBeforeDelivery;
            
            if (!show_yellow_rect_)
                target_flags |= ImGuiDragDropFlags_AcceptNoDrawDefaultRect; // Don't display the yellow rectangle
            
            if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload(identifier_.c_str(), target_flags)) {
                T data = *(T *)payload->Data;
                data_received_event_.invoke(data);
            }
            ImGui::EndDragDropTarget();
        } else {
            if (is_hovered_)
                hover_end_event_.invoke();
            
            is_hovered_ = false;
        }
    }
    
    /**
     * Returns true if the drag and drop target is hovered by a drag and drop source
     */
    [[nodiscard]] bool is_hovered() const {
        return is_hovered_;
    }
    
public:
    std::string identifier_;
    Event<T> data_received_event_;
    Event<> hover_start_event_;
    Event<> hover_end_event_;
    
    bool show_yellow_rect_ = true;
    
private:
    bool is_hovered_{};
};

}
#endif /* DIGITALVOX_VOX_RENDER_UI_PLUGINS_DRAG_DROP_TARGET_H_ */
