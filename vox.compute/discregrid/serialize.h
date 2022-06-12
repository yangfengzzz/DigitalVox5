//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <streambuf>

namespace vox::compute::discregrid::serialize {
namespace details {
template <class T>
bool write(std::streambuf& buf, const T& val) {
    static_assert(std::is_standard_layout<T>{}, "data is not standard layout");
    auto bytes = sizeof(T);
    return buf.sputn(reinterpret_cast<const char*>(&val), bytes) == bytes;
}
template <class T>
bool read(std::streambuf& buf, T& val) {
    static_assert(std::is_standard_layout<T>{}, "data is not standard layout");
    auto bytes = sizeof(T);
    return buf.sgetn(reinterpret_cast<char*>(&val), bytes) == bytes;
}
}  // namespace details

template <class T>
bool read(std::streambuf& buf, T& val) {
    using details::read;
    return read(buf, val);
}
template <class T>
bool write(std::streambuf& buf, T const& val) {
    using details::write;
    return write(buf, val);
}
}  // namespace vox::compute::discregrid::serialize
