//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/rendering/postprocessing_pass.h"

#include "vox.render/rendering/postprocessing_pipeline.h"

namespace vox {
PostProcessingPassBase::PostProcessingPassBase(PostProcessingPipeline *parent) : parent_{parent} {}

RenderContext &PostProcessingPassBase::GetRenderContext() const { return *parent_->render_context_; }

ShaderSource &PostProcessingPassBase::GetTriangleVs() const { return parent_->triangle_vs_; }

PostProcessingPassBase::BarrierInfo PostProcessingPassBase::GetPredecessorSrcBarrierInfo(BarrierInfo fallback) const {
    const size_t kCurPassI = parent_->GetCurrentPassIndex();
    if (kCurPassI > 0) {
        const auto &prev_pass = parent_->GetPass<vox::PostProcessingPassBase>(kCurPassI - 1);
        return prev_pass.GetSrcBarrierInfo();
    } else {
        return fallback;
    }
}

}  // namespace vox
