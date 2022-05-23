//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.render/forward_application.h"
#include "vox.render/rendering/postprocessing_pipeline.h"

namespace vox {
class AtomicComputeApp : public ForwardApplication {
public:
    bool Prepare(Platform &platform) override;

    void LoadScene() override;

    void UpdateGpuTask(CommandBuffer &command_buffer, RenderTarget &render_target) override;

private:
    std::shared_ptr<Material> material_{nullptr};
    std::unique_ptr<PostProcessingPipeline> pipeline_{nullptr};
};

}  // namespace vox
