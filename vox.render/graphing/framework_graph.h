//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "core/device.h"
#include "core/image_view.h"
#include "core/swapchain.h"
#include "shader/shader_module.h"
#include "fence_pool.h"
#include "graphing/graph.h"
#include "graphing/graph_node.h"
#include "rendering/render_context.h"
#include "rendering/render_frame.h"
#include "rendering/render_target.h"
#include "resource_cache.h"
#include "semaphore_pool.h"

namespace vox::graphing::framework_graph {
bool generate(RenderContext &context);

template<typename T>
size_t create_vk_node(Graph &graph, const char *name, const T &handle) {
    std::stringstream temp;
    temp << Node::handle_to_uintptr_t(handle);
    std::string tag = "VK_HANDLE-" + temp.str();
    
    size_t id = graph.find_ref(tag);
    
    if (id == Graph::node_not_found_) {
        id = graph.create_node(name, "Vulkan", nlohmann::json{{name, Node::handle_to_uintptr_t(handle)}});
        graph.add_ref(tag, id);
    }
    
    return id;
}

size_t create_vk_image(Graph &graph, const VkImage &image);

size_t create_vk_image_view(Graph &graph, const VkImageView &image);

size_t device_node(Graph &graph, const Device &device);
size_t render_context_node(Graph &graph, const RenderContext &context);
size_t semaphore_pool_node(Graph &graph, const SemaphorePool &semaphore_pool);
size_t fence_pool_node(Graph &graph, const FencePool &fence_pool);
size_t render_frame_node(Graph &graph, const std::unique_ptr<RenderFrame> &frame, const std::string &label);
size_t render_target_node(Graph &graph, const RenderTarget &render_target);
size_t image_view_node(Graph &graph, const core::ImageView &image_view);
size_t image_node(Graph &graph, const core::Image &image);
size_t swapchain_node(Graph &graph, const Swapchain &swapchain);
size_t resource_cache_node(Graph &graph, const ResourceCache &resource_cache);
size_t descriptor_set_layout_node(Graph &graph, const DescriptorSetLayout &descriptor_set_layout);
size_t framebuffer_node(Graph &graph, const Framebuffer &framebuffer);
size_t render_pass_node(Graph &graph, const RenderPass &render_pass);
size_t shader_module_node(Graph &graph, const ShaderModule &shader_module);
size_t shader_resource_node(Graph &graph, const ShaderResource &shader_resource);
size_t pipeline_layout_node(Graph &graph, const PipelineLayout &pipeline_layout);
size_t graphics_pipeline_node(Graph &graph, const GraphicsPipeline &graphics_pipeline);
size_t compute_pipeline_node(Graph &graph, const ComputePipeline &compute_pipeline);
size_t pipeline_state_node(Graph &graph, const PipelineState &pipeline_state);
size_t descriptor_set_node(Graph &graph, const DescriptorSet &descriptor_set);
size_t specialization_constant_state_node(Graph &graph, const SpecializationConstantState &specialization_constant_state);
size_t vertex_input_state_node(Graph &graph, const VertexInputState &vertex_input_state);
size_t input_assembly_state_node(Graph &graph, const InputAssemblyState &input_assembly_state);
size_t rasterization_state_node(Graph &graph, const RasterizationState &rasterization_state);
size_t viewport_state_node(Graph &graph, const ViewportState &viewport_state);
size_t multisample_state_node(Graph &graph, const MultisampleState &multisample_state);
size_t depth_stencil_state_node(Graph &graph, const DepthStencilState &depth_stencil_state);
size_t color_blend_state_node(Graph &graph, const ColorBlendState &color_blend_state);
size_t color_blend_attachment_state_node(Graph &graph, const ColorBlendAttachmentState &state);

}        // namespace vox
