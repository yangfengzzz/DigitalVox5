//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "render_pipeline.h"

namespace vox {
RenderPipeline::RenderPipeline(std::vector<std::unique_ptr<Subpass>> &&subpasses) :
subpasses_{std::move(subpasses)} {
    prepare();
    
    // Default clear value
    clear_value_[0].color = {0.0f, 0.0f, 0.0f, 1.0f};
    clear_value_[1].depthStencil = {1.0f, 255U};
}

void RenderPipeline::prepare() {
    for (auto &subpass : subpasses_) {
        subpass->prepare();
    }
}

void RenderPipeline::add_subpass(std::unique_ptr<Subpass> &&subpass) {
    subpass->prepare();
    subpasses_.emplace_back(std::move(subpass));
}

std::vector<std::unique_ptr<Subpass>> &RenderPipeline::get_subpasses() {
    return subpasses_;
}

const std::vector<LoadStoreInfo> &RenderPipeline::get_load_store() const {
    return load_store_;
}

void RenderPipeline::set_load_store(const std::vector<LoadStoreInfo> &ls) {
    load_store_ = ls;
}

const std::vector<VkClearValue> &RenderPipeline::get_clear_value() const {
    return clear_value_;
}

void RenderPipeline::set_clear_value(const std::vector<VkClearValue> &cv) {
    clear_value_ = cv;
}

void RenderPipeline::draw(CommandBuffer &command_buffer, RenderTarget &render_target, VkSubpassContents contents) {
    assert(!subpasses_.empty() && "Render pipeline should contain at least one sub-pass");
    
    // Pad clear values if they're less than render target attachments
    while (clear_value_.size() < render_target.get_attachments().size()) {
        clear_value_.push_back({0.0f, 0.0f, 0.0f, 1.0f});
    }
    
    for (size_t i = 0; i < subpasses_.size(); ++i) {
        active_subpass_index_ = i;
        
        auto &subpass = subpasses_[i];
        
        subpass->update_render_target_attachments(render_target);
        
        if (i == 0) {
            command_buffer.begin_render_pass(render_target, load_store_, clear_value_, subpasses_, contents);
        } else {
            command_buffer.next_subpass();
        }
        
        if (subpass->get_debug_name().empty()) {
            subpass->set_debug_name(fmt::format("RP subpass #{}", i));
        }
        ScopedDebugLabel subpass_debug_label{command_buffer, subpass->get_debug_name().c_str()};
        
        subpass->draw(command_buffer);
    }
    
    active_subpass_index_ = 0;
}

std::unique_ptr<Subpass> &RenderPipeline::get_active_subpass() {
    return subpasses_[active_subpass_index_];
}

}        // namespace vox
