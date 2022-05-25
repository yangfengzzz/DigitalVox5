//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.base/helper.h"
#include "vox.render/core/vulkan_resource.h"
#include "vox.render/vk_common.h"

namespace vox {
struct Attachment;

class Device;

struct SubpassInfo {
    std::vector<uint32_t> input_attachments;

    std::vector<uint32_t> output_attachments;

    std::vector<uint32_t> color_resolve_attachments;

    bool disable_depth_stencil_attachment;

    uint32_t depth_stencil_resolve_attachment;

    VkResolveModeFlagBits depth_stencil_resolve_mode;

    std::string debug_name;
};

class RenderPass : public core::VulkanResource<VkRenderPass, VK_OBJECT_TYPE_RENDER_PASS> {
public:
    RenderPass(Device &device,
               const std::vector<Attachment> &attachments,
               const std::vector<LoadStoreInfo> &load_store_infos,
               const std::vector<SubpassInfo> &subpasses);

    RenderPass(const RenderPass &) = delete;

    RenderPass(RenderPass &&other) noexcept;

    ~RenderPass() override;

    RenderPass &operator=(const RenderPass &) = delete;

    RenderPass &operator=(RenderPass &&) = delete;

    [[nodiscard]] uint32_t GetColorOutputCount(uint32_t subpass_index) const;

    [[nodiscard]] VkExtent2D GetRenderAreaGranularity() const;

private:
    size_t subpass_count_;

    template <typename T_SubpassDescription,
              typename T_AttachmentDescription,
              typename T_AttachmentReference,
              typename T_SubpassDependency,
              typename T_RenderPassCreateInfo>
    void CreateRenderpass(const std::vector<Attachment> &attachments,
                          const std::vector<LoadStoreInfo> &load_store_infos,
                          const std::vector<SubpassInfo> &subpasses);

    std::vector<uint32_t> color_output_count_;
};

}  // namespace vox
