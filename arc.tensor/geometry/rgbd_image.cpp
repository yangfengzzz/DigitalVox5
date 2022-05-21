//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "rgbd_image.h"

namespace vox {
namespace t {
namespace geometry {

RGBDImage &RGBDImage::Clear() {
    color_.Clear();
    depth_.Clear();
    return *this;
}

bool RGBDImage::IsEmpty() const { return color_.IsEmpty() && depth_.IsEmpty(); }

std::string RGBDImage::ToString() const {
    return fmt::format(
            "RGBD Image pair [{}Aligned]\n"
            "Color [size=({},{}), channels={}, format={}, device={}]\n"
            "Depth [size=({},{}), channels={}, format={}, device={}]",
            AreAligned() ? "" : "Not ", color_.GetCols(), color_.GetRows(), color_.GetChannels(),
            color_.GetDtype().ToString(), color_.GetDevice().ToString(), depth_.GetCols(), depth_.GetRows(),
            depth_.GetChannels(), depth_.GetDtype().ToString(), depth_.GetDevice().ToString());
}

}  // namespace geometry
}  // namespace t
}  // namespace vox
