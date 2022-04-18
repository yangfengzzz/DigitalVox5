//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "postprocessing_pipeline.h"

#include "utils.h"

namespace vox {
PostProcessingPipeline::PostProcessingPipeline(RenderContext &render_context, ShaderSource triangle_vs) :
render_context{&render_context},
triangle_vs{std::move(triangle_vs)} {}

void PostProcessingPipeline::draw(CommandBuffer &command_buffer, RenderTarget &default_render_target) {
    for (current_pass_index = 0; current_pass_index < passes.size(); current_pass_index++) {
        auto &pass = *passes[current_pass_index];
        
        if (pass.debug_name.empty()) {
            pass.debug_name = fmt::format("PPP pass #{}", current_pass_index);
        }
        ScopedDebugLabel marker{command_buffer, pass.debug_name.c_str()};
        
        if (!pass.prepared) {
            ScopedDebugLabel marker{command_buffer, "Prepare"};
            
            pass.prepare(command_buffer, default_render_target);
            pass.prepared = true;
        }
        
        if (pass.pre_draw) {
            ScopedDebugLabel marker{command_buffer, "Pre-draw"};
            
            pass.pre_draw();
        }
        
        pass.draw(command_buffer, default_render_target);
        
        if (pass.post_draw) {
            ScopedDebugLabel marker{command_buffer, "Post-draw"};
            
            pass.post_draw();
        }
    }
    
    current_pass_index = 0;
}

}        // namespace vox
