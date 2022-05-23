//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/rendering/render_pipeline.h"

namespace vox {
RenderPipeline::RenderPipeline(std::vector<std::unique_ptr<Subpass>> &&subpasses) : subpasses_{std::move(subpasses)} {
    Prepare();

    // Default clear value
    clear_value_[0].color = {0.0f, 0.0f, 0.0f, 1.0f};
    clear_value_[1].depthStencil = {1.0f, 255U};
}

void RenderPipeline::Prepare() {
    for (auto &subpass : subpasses_) {
        subpass->Prepare();
    }
}

void RenderPipeline::AddSubpass(std::unique_ptr<Subpass> &&subpass) {
    subpass->Prepare();
    subpasses_.emplace_back(std::move(subpass));
}

std::vector<std::unique_ptr<Subpass>> &RenderPipeline::GetSubpasses() { return subpasses_; }

const std::vector<LoadStoreInfo> &RenderPipeline::GetLoadStore() const { return load_store_; }

void RenderPipeline::SetLoadStore(const std::vector<LoadStoreInfo> &load_store) { load_store_ = load_store; }

const std::vector<VkClearValue> &RenderPipeline::GetClearValue() const { return clear_value_; }

void RenderPipeline::SetClearValue(const std::vector<VkClearValue> &clear_values) { clear_value_ = clear_values; }

void RenderPipeline::Draw(CommandBuffer &command_buffer, RenderTarget &render_target, VkSubpassContents contents) {
    assert(!subpasses_.empty() && "Render pipeline should contain at least one sub-pass");

    // Pad clear values if they're less than render target attachments
    while (clear_value_.size() < render_target.GetAttachments().size()) {
        clear_value_.push_back({0.0f, 0.0f, 0.0f, 1.0f});
    }

    for (size_t i = 0; i < subpasses_.size(); ++i) {
        active_subpass_index_ = i;

        auto &subpass = subpasses_[i];

        subpass->UpdateRenderTargetAttachments(render_target);

        if (i == 0) {
            command_buffer.BeginRenderPass(render_target, load_store_, clear_value_, subpasses_, contents);
        } else {
            command_buffer.NextSubpass();
        }

        if (subpass->GetDebugName().empty()) {
            subpass->SetDebugName(fmt::format("RP subpass #{}", i));
        }
        ScopedDebugLabel subpass_debug_label{command_buffer, subpass->GetDebugName().c_str()};

        subpass->Draw(command_buffer);
    }

    active_subpass_index_ = 0;
}

std::unique_ptr<Subpass> &RenderPipeline::GetActiveSubpass() { return subpasses_[active_subpass_index_]; }

}  // namespace vox
