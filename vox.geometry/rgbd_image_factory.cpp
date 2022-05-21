//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "rgbd_image.h"

namespace vox::geometry {

std::shared_ptr<RGBDImage> RGBDImage::CreateFromColorAndDepth(const Image &color,
                                                              const Image &depth,
                                                              double depth_scale /* = 1000.0*/,
                                                              double depth_trunc /* = 3.0*/,
                                                              bool convert_rgb_to_intensity /* = true*/) {
    std::shared_ptr<RGBDImage> rgbd_image = std::make_shared<RGBDImage>();
    if (color.height_ != depth.height_ || color.width_ != depth.width_) {
        LOGE("Unsupported image format.")
    }
    rgbd_image->depth_ = *depth.ConvertDepthToFloatImage(depth_scale, depth_trunc);
    rgbd_image->color_ = convert_rgb_to_intensity ? *color.CreateFloatImage() : color;
    return rgbd_image;
}

/// Reference: http://redwood-data.org/indoor/
/// File format: http://redwood-data.org/indoor/dataset.html
std::shared_ptr<RGBDImage> RGBDImage::CreateFromRedwoodFormat(const Image &color,
                                                              const Image &depth,
                                                              bool convert_rgb_to_intensity /* = true*/) {
    return CreateFromColorAndDepth(color, depth, 1000.0, 4.0, convert_rgb_to_intensity);
}

/// Reference: http://vision.in.tum.de/data/datasets/rgbd-dataset
/// File format: http://vision.in.tum.de/data/datasets/rgbd-dataset/file_formats
std::shared_ptr<RGBDImage> RGBDImage::CreateFromTUMFormat(const Image &color,
                                                          const Image &depth,
                                                          bool convert_rgb_to_intensity /* = true*/) {
    return CreateFromColorAndDepth(color, depth, 5000.0, 4.0, convert_rgb_to_intensity);
}

/// Reference: http://sun3d.cs.princeton.edu/
/// File format: https://github.com/PrincetonVision/SUN3DCppReader
std::shared_ptr<RGBDImage> RGBDImage::CreateFromSUNFormat(const Image &color,
                                                          const Image &depth,
                                                          bool convert_rgb_to_intensity /* = true*/) {
    std::shared_ptr<RGBDImage> rgbd_image = std::make_shared<RGBDImage>();
    if (color.height_ != depth.height_ || color.width_ != depth.width_) {
        LOGE("Unsupported image format.")
    }
    for (int v = 0; v < depth.height_; v++) {
        for (int u = 0; u < depth.width_; u++) {
            uint16_t &d = *depth.PointerAt<uint16_t>(u, v);
            d = (d >> 3) | (d << 13);
        }
    }
    // SUN depth map has long range depth. We set depth_trunc as 7.0
    return CreateFromColorAndDepth(color, depth, 1000.0, 7.0, convert_rgb_to_intensity);
}

/// Reference: http://cs.nyu.edu/~silberman/datasets/nyu_depth_v2.html
std::shared_ptr<RGBDImage> RGBDImage::CreateFromNYUFormat(const Image &color,
                                                          const Image &depth,
                                                          bool convert_rgb_to_intensity /* = true*/) {
    std::shared_ptr<RGBDImage> rgbd_image = std::make_shared<RGBDImage>();
    if (color.height_ != depth.height_ || color.width_ != depth.width_) {
        LOGE("Unsupported image format.")
    }
    for (int v = 0; v < depth.height_; v++) {
        for (int u = 0; u < depth.width_; u++) {
            auto *d = depth.PointerAt<uint16_t>(u, v);
            auto *p = (uint8_t *)d;
            uint8_t x = *p;
            *p = *(p + 1);
            *(p + 1) = x;
            double xx = 351.3 / (1092.5 - *d);
            if (xx <= 0.0) {
                *d = 0;
            } else {
                *d = (uint16_t)(floor(xx * 1000 + 0.5));
            }
        }
    }
    // NYU depth map has long range depth. We set depth_trunc as 7.0
    return CreateFromColorAndDepth(color, depth, 1000.0, 7.0, convert_rgb_to_intensity);
}

}  // namespace vox::geometry
