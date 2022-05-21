//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <string>

#include "hashmap/hash_map.h"
#include "tensor.h"

namespace vox {
namespace t {
namespace io {

/// Read a hash map's keys and values from a npz file at 'key' and 'value'.
/// Return a hash map on CPU.
///
/// \param filename The npz file name to read from.
core::HashMap ReadHashMap(const std::string& filename);

/// Save a hash map's keys and values to a npz file at 'key' and 'value'.
///
/// \param filename The npz file name to write to.
/// \param hashmap HashMap to save.
void WriteHashMap(const std::string& filename, const core::HashMap& hashmap);

}  // namespace io
}  // namespace t
}  // namespace vox
