//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "postprocessing_pass.h"

#include "postprocessing_pipeline.h"

namespace vox {
PostProcessingPassBase::PostProcessingPassBase(PostProcessingPipeline *parent) :
parent_{parent} {}

RenderContext &PostProcessingPassBase::get_render_context() const {
    return *parent_->render_context_;
}

ShaderSource &PostProcessingPassBase::get_triangle_vs() const {
    return parent_->triangle_vs_;
}

PostProcessingPassBase::BarrierInfo PostProcessingPassBase::get_predecessor_src_barrier_info(BarrierInfo fallback) const {
    const size_t kCurPassI = parent_->get_current_pass_index();
    if (kCurPassI > 0) {
        const auto &prev_pass = parent_->get_pass<vox::PostProcessingPassBase>(kCurPassI - 1);
        return prev_pass.get_src_barrier_info();
    } else {
        return fallback;
    }
}

}        // namespace vox
