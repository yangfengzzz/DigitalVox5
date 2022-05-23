//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/core/query_pool.h"

#include "vox.render/core/device.h"

namespace vox {
QueryPool::QueryPool(Device &d, const VkQueryPoolCreateInfo &info) : device_{d} {
    VK_CHECK(vkCreateQueryPool(device_.GetHandle(), &info, nullptr, &handle_));
}

QueryPool::QueryPool(QueryPool &&other) noexcept : device_{other.device_}, handle_{other.handle_} {
    other.handle_ = VK_NULL_HANDLE;
}

QueryPool::~QueryPool() {
    if (handle_ != VK_NULL_HANDLE) {
        vkDestroyQueryPool(device_.GetHandle(), handle_, nullptr);
    }
}

VkQueryPool QueryPool::GetHandle() const {
    assert(handle_ != VK_NULL_HANDLE && "QueryPool handle is invalid");
    return handle_;
}

void QueryPool::HostReset(uint32_t first_query, uint32_t query_count) {
    assert(device_.IsEnabled("VK_EXT_host_query_reset") &&
           "VK_EXT_host_query_reset needs to be enabled to call QueryPool::HostReset");

    vkResetQueryPoolEXT(device_.GetHandle(), GetHandle(), first_query, query_count);
}

VkResult QueryPool::GetResults(uint32_t first_query,
                               uint32_t num_queries,
                               size_t result_bytes,
                               void *results,
                               VkDeviceSize stride,
                               VkQueryResultFlags flags) {
    return vkGetQueryPoolResults(device_.GetHandle(), GetHandle(), first_query, num_queries, result_bytes, results,
                                 stride, flags);
}

}  // namespace vox
