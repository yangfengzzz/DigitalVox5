//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.render/resource_record.h"

namespace vox {
class ResourceCache;

/**
 * @brief Reads Vulkan objects from a memory stream and creates them in the resource cache.
 */
class ResourceReplay {
public:
    ResourceReplay();

    void Play(ResourceCache &resource_cache, ResourceRecord &recorder);

protected:
    void CreateShaderModule(ResourceCache &resource_cache, std::istringstream &stream);

    void CreatePipelineLayout(ResourceCache &resource_cache, std::istringstream &stream);

    void CreateRenderPass(ResourceCache &resource_cache, std::istringstream &stream);

    void CreateGraphicsPipeline(ResourceCache &resource_cache, std::istringstream &stream);

private:
    using ResourceFunc = std::function<void(ResourceCache &, std::istringstream &)>;

    std::unordered_map<ResourceType, ResourceFunc> stream_resources_;

    std::vector<ShaderModule *> shader_modules_;

    std::vector<PipelineLayout *> pipeline_layouts_;

    std::vector<const RenderPass *> render_passes_;

    std::vector<const GraphicsPipeline *> graphics_pipelines_;
};

}  // namespace vox
