//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/rendering/postprocessing_pipeline.h"

namespace vox {
PostProcessingPipeline::PostProcessingPipeline(RenderContext &render_context, ShaderSource triangle_vs)
    : render_context_{&render_context}, triangle_vs_{std::move(triangle_vs)} {}

void PostProcessingPipeline::Draw(CommandBuffer &command_buffer, RenderTarget &default_render_target) {
    for (current_pass_index_ = 0; current_pass_index_ < passes_.size(); current_pass_index_++) {
        auto &pass = *passes_[current_pass_index_];

        if (pass.debug_name_.empty()) {
            pass.debug_name_ = fmt::format("PPP pass #{}", current_pass_index_);
        }
        ScopedDebugLabel marker{command_buffer, pass.debug_name_.c_str()};

        if (!pass.prepared_) {
            ScopedDebugLabel marker{command_buffer, "Prepare"};

            pass.Prepare(command_buffer, default_render_target);
            pass.prepared_ = true;
        }

        if (pass.pre_draw_) {
            ScopedDebugLabel marker{command_buffer, "Pre-draw"};

            pass.pre_draw_();
        }

        pass.Draw(command_buffer, default_render_target);

        if (pass.post_draw_) {
            ScopedDebugLabel marker{command_buffer, "Post-draw"};

            pass.post_draw_();
        }
    }

    current_pass_index_ = 0;
}

}  // namespace vox
