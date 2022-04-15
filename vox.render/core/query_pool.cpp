//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "query_pool.h"

#include "device.h"

namespace vox {
QueryPool::QueryPool(Device &d, const VkQueryPoolCreateInfo &info) :
device_{d} {
    VK_CHECK(vkCreateQueryPool(device_.get_handle(), &info, nullptr, &handle_));
}

QueryPool::QueryPool(QueryPool &&other) noexcept:
device_{other.device_},
handle_{other.handle_} {
    other.handle_ = VK_NULL_HANDLE;
}

QueryPool::~QueryPool() {
    if (handle_ != VK_NULL_HANDLE) {
        vkDestroyQueryPool(device_.get_handle(), handle_, nullptr);
    }
}

VkQueryPool QueryPool::get_handle() const {
    assert(handle_ != VK_NULL_HANDLE && "QueryPool handle is invalid");
    return handle_;
}

void QueryPool::host_reset(uint32_t first_query, uint32_t query_count) {
    assert(device_.is_enabled("VK_EXT_host_query_reset") &&
           "VK_EXT_host_query_reset needs to be enabled to call QueryPool::host_reset");
    
    vkResetQueryPoolEXT(device_.get_handle(), get_handle(), first_query, query_count);
}

VkResult QueryPool::get_results(uint32_t first_query, uint32_t num_queries,
                                size_t result_bytes, void *results, VkDeviceSize stride,
                                VkQueryResultFlags flags) {
    return vkGetQueryPoolResults(device_.get_handle(), get_handle(), first_query, num_queries,
                                 result_bytes, results, stride, flags);
}

}        // namespace vox
