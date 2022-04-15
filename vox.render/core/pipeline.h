//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "helpers.h"
#include "vk_common.h"
#include "rendering/pipeline_state.h"

namespace vox {
class Device;

class Pipeline {
public:
    explicit Pipeline(Device &device);
    
    Pipeline(const Pipeline &) = delete;
    
    Pipeline(Pipeline &&other) noexcept;
    
    virtual ~Pipeline();
    
    Pipeline &operator=(const Pipeline &) = delete;
    
    Pipeline &operator=(Pipeline &&) = delete;
    
    [[nodiscard]] VkPipeline get_handle() const;
    
    [[nodiscard]] const PipelineState &get_state() const;
    
protected:
    Device &device_;
    
    VkPipeline handle_ = VK_NULL_HANDLE;
    
    PipelineState state_;
};

class ComputePipeline : public Pipeline {
public:
    ComputePipeline(ComputePipeline &&) = default;
    
    ~ComputePipeline() override = default;
    
    ComputePipeline(Device &device,
                    VkPipelineCache pipeline_cache,
                    PipelineState &pipeline_state);
};

class GraphicsPipeline : public Pipeline {
public:
    GraphicsPipeline(GraphicsPipeline &&) = default;
    
    ~GraphicsPipeline() override = default;
    
    GraphicsPipeline(Device &device,
                     VkPipelineCache pipeline_cache,
                     PipelineState &pipeline_state);
};

}        // namespace vox
