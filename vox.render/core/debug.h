//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "color.h"
#include "vk_common.h"
#include <cassert>

namespace vox {
/**
 * @brief An interface over platform-specific debug extensions.
 */
class DebugUtils {
public:
    virtual ~DebugUtils() = default;
    
    /**
     * @brief Sets the debug name for a Vulkan object.
     */
    virtual void set_debug_name(VkDevice device, VkObjectType object_type, uint64_t object_handle,
                                const char *name) const = 0;
    
    /**
     * @brief Tags the given Vulkan object with some data.
     */
    virtual void set_debug_tag(VkDevice device, VkObjectType object_type, uint64_t object_handle,
                               uint64_t tag_name, const void *tag_data, size_t tag_data_size) const = 0;
    
    /**
     * @brief Inserts a command to begin a new debug label/marker scope.
     */
    virtual void cmd_begin_label(VkCommandBuffer command_buffer,
                                 const char *name, Color color = {}) const = 0;
    
    /**
     * @brief Inserts a command to end the current debug label/marker scope.
     */
    virtual void cmd_end_label(VkCommandBuffer command_buffer) const = 0;
    
    /**
     * @brief Inserts a (non-scoped) debug label/marker in the command buffer.
     */
    virtual void cmd_insert_label(VkCommandBuffer command_buffer,
                                  const char *name, Color color = {}) const = 0;
};

/**
 * @brief DebugUtils implemented on top of VK_EXT_debug_utils.
 */
class DebugUtilsExtDebugUtils final : public DebugUtils {
public:
    ~DebugUtilsExtDebugUtils() override = default;
    
    void set_debug_name(VkDevice device, VkObjectType object_type, uint64_t object_handle,
                        const char *name) const override;
    
    void set_debug_tag(VkDevice device, VkObjectType object_type, uint64_t object_handle,
                       uint64_t tag_name, const void *tag_data, size_t tag_data_size) const override;
    
    void cmd_begin_label(VkCommandBuffer command_buffer,
                         const char *name, Color color) const override;
    
    void cmd_end_label(VkCommandBuffer command_buffer) const override;
    
    void cmd_insert_label(VkCommandBuffer command_buffer,
                          const char *name, Color color) const override;
};

/**
 * @brief DebugUtils implemented on top of VK_EXT_debug_marker.
 */
class DebugMarkerExtDebugUtils final : public DebugUtils {
public:
    ~DebugMarkerExtDebugUtils() override = default;
    
    void set_debug_name(VkDevice device, VkObjectType object_type, uint64_t object_handle,
                        const char *name) const override;
    
    void set_debug_tag(VkDevice device, VkObjectType object_type, uint64_t object_handle,
                       uint64_t tag_name, const void *tag_data, size_t tag_data_size) const override;
    
    void cmd_begin_label(VkCommandBuffer command_buffer,
                         const char *name, Color color) const override;
    
    void cmd_end_label(VkCommandBuffer command_buffer) const override;
    
    void cmd_insert_label(VkCommandBuffer command_buffer,
                          const char *name, Color color) const override;
};

/**
 * @brief No-op DebugUtils.
 */
class DummyDebugUtils final : public DebugUtils {
public:
    ~DummyDebugUtils() override = default;
    
    inline void set_debug_name(VkDevice, VkObjectType, uint64_t, const char *) const override {}
    
    inline void set_debug_tag(VkDevice, VkObjectType, uint64_t,
                              uint64_t, const void *, size_t) const override {}
    
    inline void cmd_begin_label(VkCommandBuffer,
                                const char *, Color) const override {}
    
    inline void cmd_end_label(VkCommandBuffer) const override {}
    
    inline void cmd_insert_label(VkCommandBuffer,
                                 const char *, Color) const override {}
};

class CommandBuffer;

/**
 * @brief A RAII debug label.
 *        If any of EXT_debug_utils or EXT_debug_marker is available, this:
 *        - Begins a debug label / marker on construction
 *        - Ends it on destruction
 */
class ScopedDebugLabel final {
public:
    ScopedDebugLabel(const DebugUtils &debug_utils, VkCommandBuffer command_buffer,
                     const char *name, Color color = {});
    
    ScopedDebugLabel(const CommandBuffer &command_buffer,
                     const char *name, Color color = {});
    
    ~ScopedDebugLabel();
    
private:
    const DebugUtils *debug_utils_;
    VkCommandBuffer command_buffer_;
};

}        // namespace vox
