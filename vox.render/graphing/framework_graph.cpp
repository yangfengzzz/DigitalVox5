//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "graphing/framework_graph.h"

#include "strings.h"

namespace vox::graphing::framework_graph {
bool generate(RenderContext &context) {
    Graph graph("Framework");
    graph.new_style("Core", "#00BCD4");
    graph.new_style("Rendering", "#4CAF50");
    graph.new_style("Framework", "#FFC107");
    graph.new_style("Vulkan", "#F44336");

    size_t device_id = device_node(graph, context.GetDevice());

    // Device
    auto &device = context.GetDevice();

    auto &resource_cache = device.GetResourceCache();
    size_t resource_cache_id = resource_cache_node(graph, resource_cache);
    graph.add_edge(device_id, resource_cache_id);

    const auto &resource_cache_state = resource_cache.GetInternalState();

    auto it_pipeline_layouts = resource_cache_state.pipeline_layouts.begin();
    while (it_pipeline_layouts != resource_cache_state.pipeline_layouts.end()) {
        size_t pipeline_layouts_id = pipeline_layout_node(graph, it_pipeline_layouts->second);
        graph.add_edge(resource_cache_id, pipeline_layouts_id);

        auto &shader_modules = it_pipeline_layouts->second.GetShaderModules();
        for (const auto *shader_module : shader_modules) {
            size_t shader_modules_id = shader_module_node(graph, *shader_module);
            graph.add_edge(pipeline_layouts_id, shader_modules_id);

            const auto &resources = shader_module->GetResources();
            for (const auto &resource : resources) {
                size_t resource_id = shader_resource_node(graph, resource);
                graph.add_edge(shader_modules_id, resource_id);
            }
        }

        it_pipeline_layouts++;
    }

    auto it_descriptor_set_layouts = resource_cache_state.descriptor_set_layouts.begin();
    while (it_descriptor_set_layouts != resource_cache_state.descriptor_set_layouts.end()) {
        size_t descriptor_set_layouts_id = descriptor_set_layout_node(graph, it_descriptor_set_layouts->second);
        graph.add_edge(resource_cache_id, descriptor_set_layouts_id);
        it_descriptor_set_layouts++;
    }
    auto it_graphics_pipelines = resource_cache_state.graphics_pipelines.begin();
    while (it_graphics_pipelines != resource_cache_state.graphics_pipelines.end()) {
        size_t pipeline_layout =
                pipeline_layout_node(graph, it_graphics_pipelines->second.GetState().GetPipelineLayout());
        graph.add_edge(resource_cache_id, pipeline_layout);

        size_t graphics_pipelines_id = graphics_pipeline_node(graph, it_graphics_pipelines->second);
        graph.add_edge(pipeline_layout, graphics_pipelines_id);

        size_t graphics_pipelines_state_id = pipeline_state_node(graph, it_graphics_pipelines->second.GetState());
        graph.add_edge(graphics_pipelines_id, graphics_pipelines_state_id);

        size_t render_pass = render_pass_node(graph, *it_graphics_pipelines->second.GetState().GetRenderPass());
        graph.add_edge(graphics_pipelines_state_id, render_pass);

        size_t specialization_constant_state = specialization_constant_state_node(
                graph, it_graphics_pipelines->second.GetState().GetSpecializationConstantState());
        graph.add_edge(graphics_pipelines_state_id, specialization_constant_state);

        size_t vertex_input_state =
                vertex_input_state_node(graph, it_graphics_pipelines->second.GetState().GetVertexInputState());
        graph.add_edge(graphics_pipelines_state_id, vertex_input_state);

        size_t input_assembly_state =
                input_assembly_state_node(graph, it_graphics_pipelines->second.GetState().GetInputAssemblyState());
        graph.add_edge(graphics_pipelines_state_id, input_assembly_state);

        size_t rasterization_state =
                rasterization_state_node(graph, it_graphics_pipelines->second.GetState().GetRasterizationState());
        graph.add_edge(graphics_pipelines_state_id, rasterization_state);

        size_t viewport_state = viewport_state_node(graph, it_graphics_pipelines->second.GetState().GetViewportState());
        graph.add_edge(graphics_pipelines_state_id, viewport_state);

        size_t multisample_state =
                multisample_state_node(graph, it_graphics_pipelines->second.GetState().GetMultisampleState());
        graph.add_edge(graphics_pipelines_state_id, multisample_state);

        size_t depth_stencil_state =
                depth_stencil_state_node(graph, it_graphics_pipelines->second.GetState().GetDepthStencilState());
        graph.add_edge(graphics_pipelines_state_id, depth_stencil_state);

        size_t color_blend_state =
                color_blend_state_node(graph, it_graphics_pipelines->second.GetState().GetColorBlendState());
        graph.add_edge(graphics_pipelines_state_id, color_blend_state);
        it_graphics_pipelines++;
    }
    auto it_compute_pipelines = resource_cache_state.compute_pipelines.begin();
    while (it_compute_pipelines != resource_cache_state.compute_pipelines.end()) {
        size_t compute_pipelines_id = compute_pipeline_node(graph, it_compute_pipelines->second);
        graph.add_edge(resource_cache_id, compute_pipelines_id);
        it_compute_pipelines++;
    }

    auto it_framebuffers = resource_cache_state.framebuffers.begin();
    while (it_framebuffers != resource_cache_state.framebuffers.end()) {
        size_t framebuffers_id = framebuffer_node(graph, it_framebuffers->second);
        graph.add_edge(resource_cache_id, framebuffers_id);
        it_framebuffers++;
    }

    size_t render_context_id = render_context_node(graph, context);
    graph.add_edge(device_id, render_context_id);
    size_t swapchain_id = swapchain_node(graph, context.GetSwapchain());

    for (auto image : context.GetSwapchain().GetImages()) {
        size_t vkimage_id = create_vk_image(graph, image);
        graph.add_edge(vkimage_id, swapchain_id);
    }

    const auto &const_context = context;
    const auto &frames = context.GetRenderFrames();
    uint32_t i = 0;
    for (auto &frame : frames) {
        std::string label = "Render Frame";
        if (i == const_context.GetActiveFrameIndex()) {
            label = "Last " + label;
        }
        i++;

        size_t frame_id = render_frame_node(graph, frame, label);
        graph.add_edge(render_context_id, frame_id);

        size_t semaphore_pool_id = semaphore_pool_node(graph, frame->GetSemaphorePool());
        size_t fence_pool_id = fence_pool_node(graph, frame->GetFencePool());
        size_t render_target_id = render_target_node(graph, frame->GetRenderTargetConst());
        graph.add_edge(frame_id, semaphore_pool_id);
        graph.add_edge(frame_id, fence_pool_id);
        graph.add_edge(frame_id, render_target_id);

        for (const auto &view : frame->GetRenderTargetConst().GetViews()) {
            size_t image_view_id = image_view_node(graph, view);
            const auto &image = view.GetImage();
            size_t image_id = image_node(graph, image);

            graph.add_edge(render_target_id, image_view_id);
            graph.add_edge(image_view_id, image_id);

            size_t vkimage_id = create_vk_image(graph, image.GetHandle());
            graph.add_edge(image_id, vkimage_id);

            size_t vkimageview_id = create_vk_image_view(graph, view.GetHandle());
            graph.add_edge(image_view_id, vkimageview_id);
        }
    }

    return graph.dump_to_file("framework.json");
}

size_t create_vk_image(Graph &graph, const VkImage &image) { return create_vk_node(graph, "VkImage", image); }

size_t create_vk_image_view(Graph &graph, const VkImageView &image) {
    return create_vk_node(graph, "VkImageView", image);
}

size_t device_node(Graph &graph, const Device &device) {
    auto pd_props = device.GetGpu().GetProperties();

    nlohmann::json device_properties = {{"deviceID", pd_props.deviceID},
                                        {"deviceName", pd_props.deviceName},
                                        {"deviceType", ToString(pd_props.deviceType)},
                                        {"driverVersion", pd_props.driverVersion},
                                        {"apiVersion", pd_props.apiVersion},
                                        {"vendorID", pd_props.vendorID}};

    nlohmann::json data = {{"VkPhysicalDeviceProperties", device_properties}};

    return graph.create_node("Device", "Core", data);
}

size_t render_context_node(Graph &graph, const RenderContext &context) {
    auto surface = context.GetSurfaceExtent();

    nlohmann::json data = {{"VkExtent2D", {{"width", surface.width}, {"height", surface.height}}},
                           {"active_frame_index", context.GetActiveFrameIndex()}};

    return graph.create_node("Render Context", "Rendering", data);
}

size_t semaphore_pool_node(Graph &graph, const SemaphorePool &semaphore_pool) {
    nlohmann::json data = {{"active_semaphore_count", semaphore_pool.GetActiveSemaphoreCount()}};
    return graph.create_node("Semaphore Pool", "Framework", data);
}

size_t fence_pool_node(Graph &graph, const FencePool &fence_pool) {
    return graph.create_node("Fence Pool", "Framework");
}

size_t render_frame_node(Graph &graph, const std::unique_ptr<RenderFrame> &frame, const std::string &label) {
    return graph.create_node(label.c_str(), "Rendering");
}

size_t render_target_node(Graph &graph, const RenderTarget &render_target) {
    VkExtent2D surface = render_target.GetExtent();

    nlohmann::json data = {{"VkExtent2D", {{"width", surface.width}, {"height", surface.height}}},
                           {"ImageView_count", render_target.GetViews().size()},
                           {"Attachment_count", render_target.GetAttachments().size()},
                           {"output_attachment_count", render_target.GetOutputAttachments().size()}};

    return graph.create_node("Render Target", "Rendering", data);
}

size_t image_view_node(Graph &graph, const core::ImageView &image_view) {
    auto subresource_range = image_view.GetSubresourceRange();
    auto subresource_layers = image_view.GetSubresourceLayers();

    nlohmann::json data = {{"VkFormat", ToString(image_view.GetFormat())},
                           {"VkImageSubresourceRange",
                            {{"VkImageAspectFlags", ImageAspectToString(subresource_range.aspectMask)},
                             {"base_mip_level", subresource_range.baseMipLevel},
                             {"level_count", subresource_range.levelCount},
                             {"base_array_layer", subresource_range.baseArrayLayer},
                             {"layer_count", subresource_range.layerCount}}},
                           {"VkImageSubresourceLayers",
                            {{"VkImageAspectFlags", ImageAspectToString(subresource_layers.aspectMask)},
                             {"mip_level", subresource_layers.mipLevel},
                             {"base_array_layer", subresource_layers.baseArrayLayer},
                             {"layer_count", subresource_layers.layerCount}}}};

    return graph.create_node("Image View", "Core", data);
}

size_t image_node(Graph &graph, const core::Image &image) {
    std::string result;
    bool append = false;
    auto flags = image.GetUsage();
    if (flags & VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT) {
        result += append ? " / " : "";
        result += "COLOR";
        append = true;
    }
    if (flags & VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT) {
        result += append ? " / " : "";
        result += "DEPTH STENCIL";
        append = true;
    }
    auto subresource = image.GetSubresource();

    nlohmann::json data = {{"VkExtent2D", {{"width", image.GetExtent().width}, {"height", image.GetExtent().height}}},
                           {"VkFormat", ToString(image.GetFormat())},
                           {"VkImageUsageFlags", ImageUsageToString(image.GetUsage())},
                           {"VkSampleCountFlagBits", ToString(image.GetSampleCount())},
                           {"VkImageTiling", ToString(image.GetTiling())},
                           {"VkImageType", ToString(image.GetType())},
                           {"VkSubresource",
                            {{"VkImageAspectFlags", ImageAspectToString(subresource.aspectMask)},
                             {"mip_level", subresource.mipLevel},
                             {"array_layer", subresource.arrayLayer}}}};

    return graph.create_node(result.c_str(), "Core", data);
}

size_t swapchain_node(Graph &graph, const Swapchain &swapchain) {
    auto surface = swapchain.GetExtent();
    auto format = swapchain.GetFormat();
    auto image_count = swapchain.GetImages().size();

    nlohmann::json data = {{"VkExtent2D", {{"width", surface.width}, {"height", surface.height}}},
                           {"VkFormat", ToString(format)},
                           {"image_count", image_count},
                           {"VkSurfaceTransformFlagBitsKHR", ToString(swapchain.GetTransform())},
                           {"VkPresentModeKHR", ToString(swapchain.GetPresentMode())},
                           {"VkImageUsageFlags", ImageUsageToString(swapchain.GetUsage())}};

    return graph.create_node("Swapchain", "Core", data);
}

size_t resource_cache_node(Graph &graph, const ResourceCache &resource_cache) {
    return graph.create_node("Resource Cache", "Core");
}

size_t descriptor_set_layout_node(Graph &graph, const DescriptorSetLayout &descriptor_set_layout) {
    std::vector<nlohmann::json> bindings;

    auto it = descriptor_set_layout.GetBindings().begin();
    while (it != descriptor_set_layout.GetBindings().end()) {
        bindings.push_back({{"binding", it->binding},
                            {"descriptorCount", it->descriptorCount},
                            {"stageFlags", ToString(it->stageFlags)}});
        it++;
    }

    nlohmann::json data = {{"handle", Node::handle_to_uintptr_t(descriptor_set_layout.GetHandle())},
                           {"VkDescriptorSetLayoutBinding", bindings}};

    return graph.create_node("Descriptor Set Layout", "Core", data);
}

size_t framebuffer_node(Graph &graph, const Framebuffer &framebuffer) {
    nlohmann::json data = {{"handle", Node::handle_to_uintptr_t(framebuffer.GetHandle())}};

    return graph.create_node("Frame Buffer", "Core", data);
}

size_t render_pass_node(Graph &graph, const RenderPass &render_pass) {
    nlohmann::json data = {{"handle", Node::handle_to_uintptr_t(render_pass.GetHandle())}};

    return graph.create_node("Render Pass", "Rendering", data);
}

size_t shader_module_node(Graph &graph, const ShaderModule &shader_module) {
    std::string stage = ShaderStageToString(shader_module.GetStage());
    std::transform(stage.begin(), stage.end(), stage.begin(), [](unsigned char c) { return std::tolower(c); });

    nlohmann::json data = {{"stage", stage},
                           {"infoLog", shader_module.GetInfoLog()},
                           {"entry_point", shader_module.GetEntryPoint()},
                           {"id", shader_module.GetId()}};

    stage = "Shader Module: " + stage;

    return graph.create_node(stage.c_str(), "Rendering", data);
}

size_t shader_resource_node(Graph &graph, const ShaderResource &shader_resource) {
    std::string label = fmt::format("{}: {}", ToString(shader_resource.type), shader_resource.name);

    nlohmann::json data = {{"ShaderResourceType", ToString(shader_resource.type)},
                           {"VkShaderStageFlags", ToString(shader_resource.stages)},
                           {"set", shader_resource.set},
                           {"binding", shader_resource.binding},
                           {"location", shader_resource.location},
                           {"input_attachment_index", shader_resource.input_attachment_index},
                           {"vec_size", shader_resource.vec_size},
                           {"columns", shader_resource.columns},
                           {"array_size", shader_resource.array_size},
                           {"offset", shader_resource.offset},
                           {"size", shader_resource.size},
                           {"constant_id", shader_resource.constant_id},
                           {"mode", shader_resource.mode},
                           {"name", shader_resource.name}};

    return graph.create_node(label.c_str(), "Rendering", data);
}

size_t pipeline_layout_node(Graph &graph, const PipelineLayout &pipeline_layout) {
    nlohmann::json data = {{"handle", Node::handle_to_uintptr_t(pipeline_layout.GetHandle())}};

    return graph.create_node("Pipeline Layout", "Core", data);
}

size_t graphics_pipeline_node(Graph &graph, const GraphicsPipeline &graphics_pipeline) {
    nlohmann::json data = {{"handle", Node::handle_to_uintptr_t(graphics_pipeline.GetHandle())}};

    return graph.create_node("Graphics Pipeline", "Core", data);
}

size_t compute_pipeline_node(Graph &graph, const ComputePipeline &compute_pipeline) {
    nlohmann::json data = {{"handle", Node::handle_to_uintptr_t(compute_pipeline.GetHandle())}};

    return graph.create_node("Compute Pipeline", "Core", data);
}

size_t pipeline_state_node(Graph &graph, const PipelineState &pipeline_state) {
    nlohmann::json data = {{"subpass_index", pipeline_state.GetSubpassIndex()}};

    return graph.create_node("Pipeline State", "Core", data);
}

size_t descriptor_set_node(Graph &graph, const DescriptorSet &descriptor_set) {
    nlohmann::json data = {{"handle", Node::handle_to_uintptr_t(descriptor_set.GetHandle())}};

    return graph.create_node("Descriptor Set", "Core", data);
}

size_t specialization_constant_state_node(Graph &graph,
                                          const SpecializationConstantState &specialization_constant_state) {
    nlohmann::json data = {};

    const auto &state = specialization_constant_state.GetSpecializationConstantState();
    auto it = state.begin();
    while (it != state.end()) {
        std::stringstream str;
        str << it->first;
        data.push_back({str.str(), it->second});
    }

    return graph.create_node("Specialization Constant state", "Core", data);
}

size_t vertex_input_state_node(Graph &graph, const VertexInputState &vertex_input_state) {
    std::vector<nlohmann::json> bindings;

    for (auto binding : vertex_input_state.bindings) {
        bindings.push_back({{"binding", binding.binding},
                            {"stride", binding.stride},
                            {"VkVertexInputRate", ToString(binding.inputRate)}});
    }

    std::vector<nlohmann::json> binding;

    for (auto attribute : vertex_input_state.attributes) {
        binding.push_back({{"location", attribute.location},
                           {"binding", attribute.binding},
                           {"format", ToString(attribute.format)},
                           {"offset", attribute.offset}});
    }

    nlohmann::json data = {{"VkVertexInputBindingDescription", bindings},
                           {"VkVertexInputAttributeDescription", binding}};

    return graph.create_node("Vertex Input State", "Core", data);
}

size_t input_assembly_state_node(Graph &graph, const InputAssemblyState &input_assembly_state) {
    nlohmann::json data = {
            {"VkPrimitiveTopology", ToString(input_assembly_state.topology)},
            {"primitive_restart_enabled", ToStringVkBool(input_assembly_state.primitive_restart_enable)}};

    return graph.create_node("Input Assembly State", "Core", data);
}

size_t rasterization_state_node(Graph &graph, const RasterizationState &rasterization_state) {
    nlohmann::json data = {
            {"depth_clamp_enable", ToStringVkBool(rasterization_state.depth_clamp_enable)},
            {"rasterizer_discard_enable", ToStringVkBool(rasterization_state.rasterizer_discard_enable)},
            {"polygon_mode", ToString(rasterization_state.polygon_mode)},
            {"cull_mode", CullModeToString(rasterization_state.cull_mode)},
            {"front_face", ToString(rasterization_state.front_face)},
            {"depth_bias_enable", ToStringVkBool(rasterization_state.depth_bias_enable)},
    };

    return graph.create_node("Rasterization State", "Core", data);
}

size_t viewport_state_node(Graph &graph, const ViewportState &viewport_state) {
    nlohmann::json data = {{"viewport_count", viewport_state.viewport_count},
                           {"scissor_count", viewport_state.scissor_count}};

    return graph.create_node("Viewport State", "Core", data);
}

size_t multisample_state_node(Graph &graph, const MultisampleState &multisample_state) {
    nlohmann::json data = {{"rasterization_samples", multisample_state.rasterization_samples},
                           {"sample_shading_enable", ToStringVkBool(multisample_state.sample_shading_enable)},
                           {"min_sample_shading", multisample_state.min_sample_shading},
                           {"sample_mask", multisample_state.sample_mask},
                           {"alpha_to_coverage_enable", ToStringVkBool(multisample_state.alpha_to_coverage_enable)},
                           {"alpha_to_one_enable", ToStringVkBool(multisample_state.alpha_to_one_enable)}};

    return graph.create_node("Multisample State", "Core", data);
}

size_t depth_stencil_state_node(Graph &graph, const DepthStencilState &depth_stencil_state) {
    nlohmann::json data = {{"depth_test_enable", ToStringVkBool(depth_stencil_state.depth_test_enable)},
                           {"depth_write_enable", ToStringVkBool(depth_stencil_state.depth_write_enable)},
                           {"depth_compare_op", ToString(depth_stencil_state.depth_compare_op)},
                           {"depth_bounds_test_enable", ToStringVkBool(depth_stencil_state.depth_bounds_test_enable)},
                           {"stencil_test_enable", ToStringVkBool(depth_stencil_state.stencil_test_enable)},
                           {"front",
                            {{"fail_op", ToString(depth_stencil_state.front.fail_op)},
                             {"pass_op", ToString(depth_stencil_state.front.pass_op)},
                             {"depth_fail_op", ToString(depth_stencil_state.front.depth_fail_op)},
                             {"compare_op", ToString(depth_stencil_state.front.compare_op)}}},
                           {"back",
                            {{"fail_op", ToString(depth_stencil_state.back.fail_op)},
                             {"pass_op", ToString(depth_stencil_state.back.pass_op)},
                             {"depth_fail_op", ToString(depth_stencil_state.back.depth_fail_op)},
                             {"compare_op", ToString(depth_stencil_state.back.compare_op)}}}};

    return graph.create_node("Depth Stencil State", "Core", data);
}

size_t color_blend_state_node(Graph &graph, const ColorBlendState &color_blend_state) {
    nlohmann::json data = {{"logic_op_enable", ToStringVkBool(color_blend_state.logic_op_enable)},
                           {"logic_op", ToString(color_blend_state.logic_op)}};

    return graph.create_node("Color Blend State", "Core", data);
}

size_t color_blend_attachment_state_node(Graph &graph, const ColorBlendAttachmentState &state) {
    nlohmann::json data = {{"blend_enable", ToStringVkBool(state.blend_enable)},
                           {"src_color_blend_factor", ToString(state.src_color_blend_factor)},
                           {"dst_color_blend_factor", ToString(state.dst_color_blend_factor)},
                           {"color_blend_op", ToString(state.color_blend_op)},
                           {"src_alpha_blend_factor", ToString(state.src_alpha_blend_factor)},
                           {"dst_alpha_blend_factor", ToString(state.dst_alpha_blend_factor)},
                           {"alpha_blend_op", ToString(state.alpha_blend_op)},
                           {"color_write_mask", ColorComponentToString(state.color_write_mask)}};

    return graph.create_node("Color Blend Attachment State", "Core", data);
}

}  // namespace vox::graphing::framework_graph
