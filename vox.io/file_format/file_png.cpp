//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include <png.h>

#include "../image_io.h"
#include "logging.h"

namespace vox {

namespace {
using namespace io;

void SetPNGImageFromImage(const geometry::Image &image, int quality, png_image &pngimage) {
    pngimage.width = image.width_;
    pngimage.height = image.height_;
    pngimage.format = pngimage.flags = 0;

    if (image.bytes_per_channel_ == 2) {
        pngimage.format |= PNG_FORMAT_FLAG_LINEAR;
    }
    if (image.num_of_channels_ >= 3) {
        pngimage.format |= PNG_FORMAT_FLAG_COLOR;
    }
    if (image.num_of_channels_ == 4) {
        pngimage.format |= PNG_FORMAT_FLAG_ALPHA;
    }
    if (quality <= 2) {
        pngimage.flags |= PNG_IMAGE_FLAG_FAST;
    }
}

}  // unnamed namespace

namespace io {

bool ReadImageFromPNG(const std::string &filename, geometry::Image &image) {
    png_image pngimage;
    memset(&pngimage, 0, sizeof(pngimage));
    pngimage.version = PNG_IMAGE_VERSION;
    if (png_image_begin_read_from_file(&pngimage, filename.c_str()) == 0) {
        LOGW("Read PNG failed: unable to parse header.")
        return false;
    }

    // Clear colormap flag if necessary to ensure libpng expands the colo
    // indexed pixels to full color
    if (pngimage.format & PNG_FORMAT_FLAG_COLORMAP) {
        pngimage.format &= ~PNG_FORMAT_FLAG_COLORMAP;
    }

    image.Prepare(pngimage.width, pngimage.height, PNG_IMAGE_SAMPLE_CHANNELS(pngimage.format),
                  PNG_IMAGE_SAMPLE_COMPONENT_SIZE(pngimage.format));

    if (png_image_finish_read(&pngimage, nullptr, image.data_.data(), 0, nullptr) == 0) {
        LOGW("Read PNG failed: unable to read file: {}", filename)
        LOGW("PNG error: {}", pngimage.message)
        return false;
    }
    return true;
}

bool WriteImageToPNG(const std::string &filename, const geometry::Image &image, int quality) {
    if (!image.HasData()) {
        LOGW("Write PNG failed: image has no data.")
        return false;
    }
    if (quality == kOpen3DImageIODefaultQuality)  // Set default quality
        quality = 6;
    if (quality < 0 || quality > 9) {
        LOGW("Write PNG failed: quality ({}) must be in the range [0,9]", quality)
        return false;
    }
    png_image pngimage;
    memset(&pngimage, 0, sizeof(pngimage));
    pngimage.version = PNG_IMAGE_VERSION;
    SetPNGImageFromImage(image, quality, pngimage);
    if (png_image_write_to_file(&pngimage, filename.c_str(), 0, image.data_.data(), 0, nullptr) == 0) {
        LOGW("Write PNG failed: unable to write file: {}", filename)
        return false;
    }
    return true;
}

}  // namespace io
}  // namespace vox
