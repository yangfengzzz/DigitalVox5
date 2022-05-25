//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.base/helper.h"
#include "vox.render/vk_common.h"

namespace vox {
class Device;

/**
 * @brief Represents a Vulkan Query Pool
 */
class QueryPool {
public:
    /**
     * @brief Creates a Vulkan Query Pool
     * @param d The device to use
     * @param info Creation details
     */
    QueryPool(Device &d, const VkQueryPoolCreateInfo &info);

    QueryPool(const QueryPool &) = delete;

    QueryPool(QueryPool &&pool) noexcept;

    ~QueryPool();

    QueryPool &operator=(const QueryPool &) = delete;

    QueryPool &operator=(QueryPool &&) = delete;

    /**
     * @return The vulkan query pool handle
     */
    [[nodiscard]] VkQueryPool GetHandle() const;

    /**
     * @brief Reset a range of queries in the query pool. Only call if VK_EXT_host_query_reset is enabled.
     * @param first_query The first query to reset
     * @param query_count The number of queries to reset
     */
    void HostReset(uint32_t first_query, uint32_t query_count);

    /**
     * @brief Get query pool results
     * @param first_query The initial query index
     * @param num_queries The number of queries to read
     * @param result_bytes The number of bytes in the results array
     * @param results Array of bytes result_bytes long
     * @param stride The stride in bytes between results for individual queries
     * @param flags A bitmask of VkQueryResultFlagBits
     */
    VkResult GetResults(uint32_t first_query,
                        uint32_t num_queries,
                        size_t result_bytes,
                        void *results,
                        VkDeviceSize stride,
                        VkQueryResultFlags flags);

private:
    Device &device_;

    VkQueryPool handle_{VK_NULL_HANDLE};
};

}  // namespace vox
