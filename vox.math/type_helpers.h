//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef INCLUDE_VOX_TYPE_HELPERS_H_
#define INCLUDE_VOX_TYPE_HELPERS_H_

namespace vox {

//! Returns the type of the value itself.
template<typename T>
struct ScalarType {
    using value = T;
};

}  // namespace vox

#endif  // INCLUDE_VOX_TYPE_HELPERS_H_
