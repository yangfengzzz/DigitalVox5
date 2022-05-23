//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/utils.h"

#include <queue>
#include <stdexcept>

namespace vox {
std::string GetExtension(const std::string &uri) {
    auto dot_pos = uri.find_last_of('.');
    if (dot_pos == std::string::npos) {
        throw std::runtime_error{"Uri has no extension"};
    }

    return uri.substr(dot_pos + 1);
}

void Screenshot(RenderContext &render_context, const std::string &filename) {
    assert(render_context.GetFormat() == VK_FORMAT_R8G8B8A8_UNORM ||
           render_context.GetFormat() == VK_FORMAT_B8G8R8A8_UNORM ||
           render_context.GetFormat() == VK_FORMAT_R8G8B8A8_SRGB ||
           render_context.GetFormat() == VK_FORMAT_B8G8R8A8_SRGB);

    // We want the last completed frame since we don't want to be reading from an incomplete framebuffer
    auto &frame = render_context.GetLastRenderedFrame();
    auto &src_image_view = frame.GetRenderTarget().GetViews().at(0);

    auto width = render_context.GetSurfaceExtent().width;
    auto height = render_context.GetSurfaceExtent().height;
    auto dst_size = width * height * 4;

    core::Buffer dst_buffer{render_context.GetDevice(), dst_size, VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                            VMA_MEMORY_USAGE_GPU_TO_CPU, VMA_ALLOCATION_CREATE_MAPPED_BIT};

    const auto &queue = render_context.GetDevice().GetQueueByFlags(VK_QUEUE_GRAPHICS_BIT, 0);

    auto &cmd_buf = render_context.GetDevice().RequestCommandBuffer();

    cmd_buf.Begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

    // Enable destination buffer to be written to
    {
        BufferMemoryBarrier memory_barrier{};
        memory_barrier.src_access_mask = 0;
        memory_barrier.dst_access_mask = VK_ACCESS_TRANSFER_WRITE_BIT;
        memory_barrier.src_stage_mask = VK_PIPELINE_STAGE_TRANSFER_BIT;
        memory_barrier.dst_stage_mask = VK_PIPELINE_STAGE_TRANSFER_BIT;

        cmd_buf.BufferMemoryBarrier(dst_buffer, 0, dst_size, memory_barrier);
    }

    // Enable framebuffer image view to be read from
    {
        ImageMemoryBarrier memory_barrier{};
        memory_barrier.old_layout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        memory_barrier.new_layout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        memory_barrier.src_stage_mask = VK_PIPELINE_STAGE_TRANSFER_BIT;
        memory_barrier.dst_stage_mask = VK_PIPELINE_STAGE_TRANSFER_BIT;

        cmd_buf.ImageMemoryBarrier(src_image_view, memory_barrier);
    }

    // Check if framebuffer images are in a BGR format
    auto bgr_formats = {VK_FORMAT_B8G8R8A8_SRGB, VK_FORMAT_B8G8R8A8_UNORM, VK_FORMAT_B8G8R8A8_SNORM};
    bool swizzle = std::find(bgr_formats.begin(), bgr_formats.end(), src_image_view.GetFormat()) != bgr_formats.end();

    // Copy framebuffer image memory
    VkBufferImageCopy image_copy_region{};
    image_copy_region.bufferRowLength = width;
    image_copy_region.bufferImageHeight = height;
    image_copy_region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    image_copy_region.imageSubresource.layerCount = 1;
    image_copy_region.imageExtent.width = width;
    image_copy_region.imageExtent.height = height;
    image_copy_region.imageExtent.depth = 1;

    cmd_buf.CopyImageToBuffer(src_image_view.GetImage(), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, dst_buffer,
                              {image_copy_region});

    // Enable destination buffer to map memory
    {
        BufferMemoryBarrier memory_barrier{};
        memory_barrier.src_access_mask = VK_ACCESS_TRANSFER_WRITE_BIT;
        memory_barrier.dst_access_mask = VK_ACCESS_HOST_READ_BIT;
        memory_barrier.src_stage_mask = VK_PIPELINE_STAGE_TRANSFER_BIT;
        memory_barrier.dst_stage_mask = VK_PIPELINE_STAGE_HOST_BIT;

        cmd_buf.BufferMemoryBarrier(dst_buffer, 0, dst_size, memory_barrier);
    }

    // Revert back the framebuffer image view from transfer to present
    {
        ImageMemoryBarrier memory_barrier{};
        memory_barrier.old_layout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        memory_barrier.new_layout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        memory_barrier.src_stage_mask = VK_PIPELINE_STAGE_TRANSFER_BIT;
        memory_barrier.dst_stage_mask = VK_PIPELINE_STAGE_TRANSFER_BIT;

        cmd_buf.ImageMemoryBarrier(src_image_view, memory_barrier);
    }

    cmd_buf.End();

    queue.Submit(cmd_buf, frame.RequestFence());

    queue.WaitIdle();

    auto raw_data = dst_buffer.Map();

    // Creates a pointer to the address of the first byte of the image data
    // Replace the A component with 255 (remove transparency)
    // If swapchain format is BGR, swapping the R and B components
    uint8_t *data = raw_data;
    if (swizzle) {
        for (size_t i = 0; i < height; ++i) {
            // Iterate over each pixel, swapping R and B components and writing the max value for alpha
            for (size_t j = 0; j < width; ++j) {
                auto temp = *(data + 2);
                *(data + 2) = *(data);
                *(data) = temp;
                *(data + 3) = 255;

                // Get next pixel
                data += 4;
            }
        }
    } else {
        for (size_t i = 0; i < height; ++i) {
            // Iterate over each pixel, writing the max value for alpha
            for (size_t j = 0; j < width; ++j) {
                *(data + 3) = 255;

                // Get next pixel
                data += 4;
            }
        }
    }

    vox::fs::WriteImage(raw_data, filename, width, height, 4, width * 4);

    dst_buffer.Unmap();
}  // namespace vox

std::string ToSnakeCase(const std::string &text) {
    std::stringstream result;

    for (const auto kCh : text) {
        if (std::isalpha(kCh)) {
            if (std::isspace(kCh)) {
                result << "_";
            } else {
                if (std::isupper(kCh)) {
                    result << "_";
                }

                result << static_cast<char>(std::tolower(kCh));
            }
        } else {
            result << kCh;
        }
    }

    return result.str();
}

}  // namespace vox
