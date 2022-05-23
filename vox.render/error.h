//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <cassert>
#include <stdexcept>
#include <string>

#include "vox.render/logging.h"
#include "vox.render/strings.h"
#include "vox.render/vk_common.h"

#if defined(__clang__)
// CLANG ENABLE/DISABLE WARNING DEFINITION
#define VKBP_DISABLE_WARNINGS()                                                    \
    _Pragma("clang diagnostic push") _Pragma("clang diagnostic ignored \"-Wall\"") \
            _Pragma("clang diagnostic ignored \"-Wextra\"")                        \
                    _Pragma("clang diagnostic ignored \"-Wtautological-compare\"")

#define VKBP_ENABLE_WARNINGS() _Pragma("clang diagnostic pop")
#elif defined(__GNUC__) || defined(__GNUG__)
// GCC ENABLE/DISABLE WARNING DEFINITION
#define VKBP_DISABLE_WARNINGS()                                                \
    _Pragma("GCC diagnostic push") _Pragma("GCC diagnostic ignored \"-Wall\"") \
            _Pragma("clang diagnostic ignored \"-Wextra\"")                    \
                    _Pragma("clang diagnostic ignored \"-Wtautological-compare\"")

#define VKBP_ENABLE_WARNINGS() _Pragma("GCC diagnostic pop")
#elif defined(_MSC_VER)
// MSVC ENABLE/DISABLE WARNING DEFINITION
#define VKBP_DISABLE_WARNINGS() __pragma(warning(push, 0))

#define VKBP_ENABLE_WARNINGS() __pragma(warning(pop))
#endif

namespace vox {
/**
 * @brief Vulkan exception structure
 */
class VulkanException : public std::runtime_error {
public:
    /**
     * @brief Vulkan exception constructor
     */
    explicit VulkanException(VkResult result, const std::string &msg = "Vulkan error");

    /**
     * @brief Returns the Vulkan error code as string
     * @return String message of exception
     */
    [[nodiscard]] const char *what() const noexcept override;

    VkResult result_;

private:
    std::string error_message_;
};
}  // namespace vox

/// @brief Helper macro to test the result of Vulkan calls which can return an error.
#define VK_CHECK(x)                                                \
    do {                                                           \
        VkResult err = x;                                          \
        if (err) {                                                 \
            LOGE("Detected Vulkan error: {}", vox::ToString(err)); \
            abort();                                               \
        }                                                          \
    } while (0)

#define ASSERT_VK_HANDLE(handle)          \
    do {                                  \
        if ((handle) == VK_NULL_HANDLE) { \
            LOGE("Handle is NULL");       \
            abort();                      \
        }                                 \
    } while (0)

#if !defined(NDEBUG) || defined(DEBUG) || defined(_DEBUG)
#define VKB_DEBUG
#endif
