//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/core/pipeline.h"

#include "vox.render/core/device.h"
#include "vox.render/shader/shader_module.h"

namespace vox {
Pipeline::Pipeline(Device &device) : device_{device} {}

Pipeline::Pipeline(Pipeline &&other) noexcept : device_{other.device_}, handle_{other.handle_}, state_{other.state_} {
    other.handle_ = VK_NULL_HANDLE;
}

Pipeline::~Pipeline() {
    // Destroy pipeline
    if (handle_ != VK_NULL_HANDLE) {
        vkDestroyPipeline(device_.GetHandle(), handle_, nullptr);
    }
}

VkPipeline Pipeline::GetHandle() const { return handle_; }

const PipelineState &Pipeline::GetState() const { return state_; }

ComputePipeline::ComputePipeline(Device &device, VkPipelineCache pipeline_cache, PipelineState &pipeline_state)
    : Pipeline{device} {
    const ShaderModule *shader_module = pipeline_state.GetPipelineLayout().GetShaderModules().front();

    if (shader_module->GetStage() != VK_SHADER_STAGE_COMPUTE_BIT) {
        throw VulkanException{VK_ERROR_INVALID_SHADER_NV, "Shader module stage is not compute"};
    }

    VkPipelineShaderStageCreateInfo stage{VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO};

    stage.stage = shader_module->GetStage();
    stage.pName = shader_module->GetEntryPoint().c_str();

    // Create the Vulkan handle
    VkShaderModuleCreateInfo vk_create_info{VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO};

    vk_create_info.codeSize = shader_module->GetBinary().size() * sizeof(uint32_t);
    vk_create_info.pCode = shader_module->GetBinary().data();

    VkResult result = vkCreateShaderModule(device.GetHandle(), &vk_create_info, nullptr, &stage.module);
    if (result != VK_SUCCESS) {
        throw VulkanException{result};
    }

    device.GetDebugUtils().SetDebugName(device.GetHandle(), VK_OBJECT_TYPE_SHADER_MODULE,
                                        reinterpret_cast<uint64_t>(stage.module),
                                        shader_module->GetDebugName().c_str());

    // Create specialization info from tracked state.
    std::vector<uint8_t> data{};
    std::vector<VkSpecializationMapEntry> map_entries{};

    const auto kSpecializationConstantState =
            pipeline_state.GetSpecializationConstantState().GetSpecializationConstantState();

    for (const auto &specialization_constant : kSpecializationConstantState) {
        map_entries.push_back(
                {specialization_constant.first, utility::ToU32(data.size()), specialization_constant.second.size()});
        data.insert(data.end(), specialization_constant.second.begin(), specialization_constant.second.end());
    }

    VkSpecializationInfo specialization_info{};
    specialization_info.mapEntryCount = utility::ToU32(map_entries.size());
    specialization_info.pMapEntries = map_entries.data();
    specialization_info.dataSize = data.size();
    specialization_info.pData = data.data();

    stage.pSpecializationInfo = &specialization_info;

    VkComputePipelineCreateInfo create_info{VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO};

    create_info.layout = pipeline_state.GetPipelineLayout().GetHandle();
    create_info.stage = stage;

    result = vkCreateComputePipelines(device.GetHandle(), pipeline_cache, 1, &create_info, nullptr, &handle_);

    if (result != VK_SUCCESS) {
        throw VulkanException{result, "Cannot create ComputePipelines"};
    }

    vkDestroyShaderModule(device.GetHandle(), stage.module, nullptr);
}

GraphicsPipeline::GraphicsPipeline(Device &device, VkPipelineCache pipeline_cache, PipelineState &pipeline_state)
    : Pipeline{device} {
    std::vector<VkShaderModule> shader_modules;

    std::vector<VkPipelineShaderStageCreateInfo> stage_create_infos;

    // Create specialization info from tracked state. This is shared by all shaders.
    std::vector<uint8_t> data{};
    std::vector<VkSpecializationMapEntry> map_entries{};

    const auto kSpecializationConstantState =
            pipeline_state.GetSpecializationConstantState().GetSpecializationConstantState();

    for (const auto &specialization_constant : kSpecializationConstantState) {
        map_entries.push_back(
                {specialization_constant.first, utility::ToU32(data.size()), specialization_constant.second.size()});
        data.insert(data.end(), specialization_constant.second.begin(), specialization_constant.second.end());
    }

    VkSpecializationInfo specialization_info{};
    specialization_info.mapEntryCount = utility::ToU32(map_entries.size());
    specialization_info.pMapEntries = map_entries.data();
    specialization_info.dataSize = data.size();
    specialization_info.pData = data.data();

    for (const ShaderModule *shader_module : pipeline_state.GetPipelineLayout().GetShaderModules()) {
        VkPipelineShaderStageCreateInfo stage_create_info{VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO};

        stage_create_info.stage = shader_module->GetStage();
        stage_create_info.pName = shader_module->GetEntryPoint().c_str();

        // Create the Vulkan handle
        VkShaderModuleCreateInfo vk_create_info{VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO};

        vk_create_info.codeSize = shader_module->GetBinary().size() * sizeof(uint32_t);
        vk_create_info.pCode = shader_module->GetBinary().data();

        VkResult result = vkCreateShaderModule(device.GetHandle(), &vk_create_info, nullptr, &stage_create_info.module);
        if (result != VK_SUCCESS) {
            throw VulkanException{result};
        }

        device.GetDebugUtils().SetDebugName(device.GetHandle(), VK_OBJECT_TYPE_SHADER_MODULE,
                                            reinterpret_cast<uint64_t>(stage_create_info.module),
                                            shader_module->GetDebugName().c_str());

        stage_create_info.pSpecializationInfo = &specialization_info;

        stage_create_infos.push_back(stage_create_info);
        shader_modules.push_back(stage_create_info.module);
    }

    VkGraphicsPipelineCreateInfo create_info{VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO};

    create_info.stageCount = utility::ToU32(stage_create_infos.size());
    create_info.pStages = stage_create_infos.data();

    VkPipelineVertexInputStateCreateInfo vertex_input_state{VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO};

    vertex_input_state.pVertexAttributeDescriptions = pipeline_state.GetVertexInputState().attributes.data();
    vertex_input_state.vertexAttributeDescriptionCount =
            utility::ToU32(pipeline_state.GetVertexInputState().attributes.size());

    vertex_input_state.pVertexBindingDescriptions = pipeline_state.GetVertexInputState().bindings.data();
    vertex_input_state.vertexBindingDescriptionCount =
            utility::ToU32(pipeline_state.GetVertexInputState().bindings.size());

    VkPipelineInputAssemblyStateCreateInfo input_assembly_state{
            VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO};

    input_assembly_state.topology = pipeline_state.GetInputAssemblyState().topology;
    input_assembly_state.primitiveRestartEnable = pipeline_state.GetInputAssemblyState().primitive_restart_enable;

    VkPipelineViewportStateCreateInfo viewport_state{VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO};

    viewport_state.viewportCount = pipeline_state.GetViewportState().viewport_count;
    viewport_state.scissorCount = pipeline_state.GetViewportState().scissor_count;

    VkPipelineRasterizationStateCreateInfo rasterization_state{
            VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO};

    rasterization_state.depthClampEnable = pipeline_state.GetRasterizationState().depth_clamp_enable;
    rasterization_state.rasterizerDiscardEnable = pipeline_state.GetRasterizationState().rasterizer_discard_enable;
    rasterization_state.polygonMode = pipeline_state.GetRasterizationState().polygon_mode;
    rasterization_state.cullMode = pipeline_state.GetRasterizationState().cull_mode;
    rasterization_state.frontFace = pipeline_state.GetRasterizationState().front_face;
    rasterization_state.depthBiasEnable = pipeline_state.GetRasterizationState().depth_bias_enable;
    rasterization_state.depthBiasClamp = 1.0f;
    rasterization_state.depthBiasSlopeFactor = 1.0f;
    rasterization_state.lineWidth = 1.0f;

    VkPipelineMultisampleStateCreateInfo multisample_state{VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO};

    multisample_state.sampleShadingEnable = pipeline_state.GetMultisampleState().sample_shading_enable;
    multisample_state.rasterizationSamples = pipeline_state.GetMultisampleState().rasterization_samples;
    multisample_state.minSampleShading = pipeline_state.GetMultisampleState().min_sample_shading;
    multisample_state.alphaToCoverageEnable = pipeline_state.GetMultisampleState().alpha_to_coverage_enable;
    multisample_state.alphaToOneEnable = pipeline_state.GetMultisampleState().alpha_to_one_enable;

    if (pipeline_state.GetMultisampleState().sample_mask) {
        multisample_state.pSampleMask = &pipeline_state.GetMultisampleState().sample_mask;
    }

    VkPipelineDepthStencilStateCreateInfo depth_stencil_state{
            VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO};

    depth_stencil_state.depthTestEnable = pipeline_state.GetDepthStencilState().depth_test_enable;
    depth_stencil_state.depthWriteEnable = pipeline_state.GetDepthStencilState().depth_write_enable;
    depth_stencil_state.depthCompareOp = pipeline_state.GetDepthStencilState().depth_compare_op;
    depth_stencil_state.depthBoundsTestEnable = pipeline_state.GetDepthStencilState().depth_bounds_test_enable;
    depth_stencil_state.stencilTestEnable = pipeline_state.GetDepthStencilState().stencil_test_enable;
    depth_stencil_state.front.failOp = pipeline_state.GetDepthStencilState().front.fail_op;
    depth_stencil_state.front.passOp = pipeline_state.GetDepthStencilState().front.pass_op;
    depth_stencil_state.front.depthFailOp = pipeline_state.GetDepthStencilState().front.depth_fail_op;
    depth_stencil_state.front.compareOp = pipeline_state.GetDepthStencilState().front.compare_op;
    depth_stencil_state.front.compareMask = ~0U;
    depth_stencil_state.front.writeMask = ~0U;
    depth_stencil_state.front.reference = ~0U;
    depth_stencil_state.back.failOp = pipeline_state.GetDepthStencilState().back.fail_op;
    depth_stencil_state.back.passOp = pipeline_state.GetDepthStencilState().back.pass_op;
    depth_stencil_state.back.depthFailOp = pipeline_state.GetDepthStencilState().back.depth_fail_op;
    depth_stencil_state.back.compareOp = pipeline_state.GetDepthStencilState().back.compare_op;
    depth_stencil_state.back.compareMask = ~0U;
    depth_stencil_state.back.writeMask = ~0U;
    depth_stencil_state.back.reference = ~0U;

    VkPipelineColorBlendStateCreateInfo color_blend_state{VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO};

    color_blend_state.logicOpEnable = pipeline_state.GetColorBlendState().logic_op_enable;
    color_blend_state.logicOp = pipeline_state.GetColorBlendState().logic_op;
    color_blend_state.attachmentCount = utility::ToU32(pipeline_state.GetColorBlendState().attachments.size());
    color_blend_state.pAttachments = reinterpret_cast<const VkPipelineColorBlendAttachmentState *>(
            pipeline_state.GetColorBlendState().attachments.data());
    color_blend_state.blendConstants[0] = 1.0f;
    color_blend_state.blendConstants[1] = 1.0f;
    color_blend_state.blendConstants[2] = 1.0f;
    color_blend_state.blendConstants[3] = 1.0f;

    std::array<VkDynamicState, 9> dynamic_states{
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_SCISSOR,
            VK_DYNAMIC_STATE_LINE_WIDTH,
            VK_DYNAMIC_STATE_DEPTH_BIAS,
            VK_DYNAMIC_STATE_BLEND_CONSTANTS,
            VK_DYNAMIC_STATE_DEPTH_BOUNDS,
            VK_DYNAMIC_STATE_STENCIL_COMPARE_MASK,
            VK_DYNAMIC_STATE_STENCIL_WRITE_MASK,
            VK_DYNAMIC_STATE_STENCIL_REFERENCE,
    };

    VkPipelineDynamicStateCreateInfo dynamic_state{VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO};

    dynamic_state.pDynamicStates = dynamic_states.data();
    dynamic_state.dynamicStateCount = utility::ToU32(dynamic_states.size());

    create_info.pVertexInputState = &vertex_input_state;
    create_info.pInputAssemblyState = &input_assembly_state;
    create_info.pViewportState = &viewport_state;
    create_info.pRasterizationState = &rasterization_state;
    create_info.pMultisampleState = &multisample_state;
    create_info.pDepthStencilState = &depth_stencil_state;
    create_info.pColorBlendState = &color_blend_state;
    create_info.pDynamicState = &dynamic_state;

    create_info.layout = pipeline_state.GetPipelineLayout().GetHandle();
    create_info.renderPass = pipeline_state.GetRenderPass()->GetHandle();
    create_info.subpass = pipeline_state.GetSubpassIndex();

    auto result = vkCreateGraphicsPipelines(device.GetHandle(), pipeline_cache, 1, &create_info, nullptr, &handle_);

    if (result != VK_SUCCESS) {
        throw VulkanException{result, "Cannot create GraphicsPipelines"};
    }

    for (auto shader_module : shader_modules) {
        vkDestroyShaderModule(device.GetHandle(), shader_module, nullptr);
    }

    state_ = pipeline_state;
}

}  // namespace vox
