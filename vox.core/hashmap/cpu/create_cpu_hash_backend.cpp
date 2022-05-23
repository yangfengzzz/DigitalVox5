//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.core/hashmap/cpu/tbb_hash_backend.h"
#include "vox.core/hashmap/dispatch.h"
#include "vox.core/hashmap/hash_map.h"

namespace vox {
namespace core {

/// Non-templated factory.
std::shared_ptr<DeviceHashBackend> CreateCPUHashBackend(int64_t init_capacity,
                                                        const Dtype& key_dtype,
                                                        const SizeVector& key_element_shape,
                                                        const std::vector<Dtype>& value_dtypes,
                                                        const std::vector<SizeVector>& value_element_shapes,
                                                        const Device& device,
                                                        const HashBackendType& backend) {
    if (backend != HashBackendType::Default && backend != HashBackendType::TBB) {
        LOGE("Unsupported backend for CPU hashmap.");
    }

    int64_t dim = key_element_shape.NumElements();

    int64_t key_dsize = dim * key_dtype.ByteSize();

    // TODO: size check
    std::vector<int64_t> value_dsizes;
    for (size_t i = 0; i < value_dtypes.size(); ++i) {
        int64_t dsize_value = value_element_shapes[i].NumElements() * value_dtypes[i].ByteSize();
        value_dsizes.push_back(dsize_value);
    }

    std::shared_ptr<DeviceHashBackend> device_hashmap_ptr;
    DISPATCH_DTYPE_AND_DIM_TO_TEMPLATE(key_dtype, dim, [&] {
        device_hashmap_ptr =
                std::make_shared<TBBHashBackend<key_t, hash_t, eq_t>>(init_capacity, key_dsize, value_dsizes, device);
    });
    return device_hashmap_ptr;
}

}  // namespace core
}  // namespace vox
