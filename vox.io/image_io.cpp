//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "image_io.h"

#include <unordered_map>

#include "file_system.h"
#include "logging.h"

namespace vox {

namespace {
using namespace io;

const std::unordered_map<std::string, std::function<bool(const std::string &, geometry::Image &)>>
        file_extension_to_image_read_function{
                {"png", ReadImageFromPNG},
                {"jpg", ReadImageFromJPG},
                {"jpeg", ReadImageFromJPG},
        };

const std::unordered_map<std::string, std::function<bool(const std::string &, const geometry::Image &, int)>>
        file_extension_to_image_write_function{
                {"png", WriteImageToPNG},
                {"jpg", WriteImageToJPG},
                {"jpeg", WriteImageToJPG},
        };

}  // unnamed namespace

namespace io {

std::shared_ptr<geometry::Image> CreateImageFromFile(const std::string &filename) {
    auto image = std::make_shared<geometry::Image>();
    ReadImage(filename, *image);
    return image;
}

bool ReadImage(const std::string &filename, geometry::Image &image) {
    std::string filename_ext = utility::filesystem::GetFileExtensionInLowerCase(filename);
    if (filename_ext.empty()) {
        LOGW("Read geometry::Image failed: missing file extension.")
        return false;
    }
    auto map_itr = file_extension_to_image_read_function.find(filename_ext);
    if (map_itr == file_extension_to_image_read_function.end()) {
        LOGW("Read geometry::Image failed: file extension {} unknown", filename_ext)
        return false;
    }
    return map_itr->second(filename, image);
}

bool WriteImage(const std::string &filename,
                const geometry::Image &image,
                int quality /* = kOpen3DImageIODefaultQuality*/) {
    std::string filename_ext = utility::filesystem::GetFileExtensionInLowerCase(filename);
    if (filename_ext.empty()) {
        LOGW("Write geometry::Image failed: unknown file extension.")
        return false;
    }
    auto map_itr = file_extension_to_image_write_function.find(filename_ext);
    if (map_itr == file_extension_to_image_write_function.end()) {
        LOGW("Write geometry::Image failed: unknown file extension.")
        return false;
    }
    return map_itr->second(filename, image, quality);
}

}  // namespace io
}  // namespace vox
