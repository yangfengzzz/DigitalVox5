//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <string>

#include "../../arc.io/image_io.h"
#include "geometry/image.h"

namespace vox {
namespace t {
namespace io {

/// Factory function to create an image from a file (ImageFactory.cpp)
/// Return an empty image if fail to read the file.
std::shared_ptr<geometry::Image> CreateImageFromFile(const std::string &filename);

/// The general entrance for reading an Image from a file
/// The function calls read functions based on the extension name of filename.
/// \param filename Full path to image. Supported file formats are png,
/// jpg/jpeg.
/// \param image An object of type arc::t::geometry::Image.
/// \return return true if the read function is successful, false otherwise.
bool ReadImage(const std::string &filename, geometry::Image &image);

constexpr int kOpen3DImageIODefaultQuality = -1;

/// The general entrance for writing an Image to a file
/// The function calls write functions based on the extension name of filename.
/// If the write function supports quality, the parameter will be used.
/// Otherwise it will be ignored.
/// \param filename Full path to image. Supported file formats are png,
/// jpg/jpeg.
/// \param image An object of type arc::t::geometry::Image.
/// \param quality: PNG: [0-9] <=2 fast write for storing intermediate data
///                            >=3 (default) normal write for balanced speed and
///                            file size
///                 JPEG: [0-100] Typically in [70,95]. 90 is default (good
///                 quality).
/// \return return true if the write function is successful, false otherwise.
///
/// Supported file extensions are png, jpg/jpeg. Data type and number of
/// channels depends on the file extension.
/// - PNG: Dtype should be one of core::UInt8, core::UInt16
///        Supported number of channels are 1, 3, and 4.
/// - JPG: Dtyppe should be core::UInt8
///        Supported number of channels are 1 and 3.
bool WriteImage(const std::string &filename, const geometry::Image &image, int quality = kOpen3DImageIODefaultQuality);

bool ReadImageFromPNG(const std::string &filename, geometry::Image &image);

bool WriteImageToPNG(const std::string &filename,
                     const geometry::Image &image,
                     int quality = kOpen3DImageIODefaultQuality);

bool ReadImageFromJPG(const std::string &filename, geometry::Image &image);

bool WriteImageToJPG(const std::string &filename,
                     const geometry::Image &image,
                     int quality = kOpen3DImageIODefaultQuality);

}  // namespace io
}  // namespace t
}  // namespace vox
