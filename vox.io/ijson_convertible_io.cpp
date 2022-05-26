//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "ijson_convertible_io.h"

#include <unordered_map>

#include "vox.base/file_system.h"
#include "vox.base/logging.h"

namespace vox {

namespace {
using namespace io;

const std::unordered_map<std::string, std::function<bool(const std::string &, utility::IJsonConvertible &)>>
        kFileExtensionToIjsonconvertibleReadFunction{
                {"json", ReadIJsonConvertibleFromJSON},
        };

const std::unordered_map<std::string, std::function<bool(const std::string &, const utility::IJsonConvertible &)>>
        kFileExtensionToIjsonconvertibleWriteFunction{
                {"json", WriteIJsonConvertibleToJSON},
        };

}  // unnamed namespace

namespace io {

bool ReadIJsonConvertible(const std::string &filename, utility::IJsonConvertible &object) {
    std::string filename_ext = utility::filesystem::GetFileExtensionInLowerCase(filename);
    if (filename_ext.empty()) {
        LOGW("Read utility::IJsonConvertible failed: unknown file "
             "extension.")
        return false;
    }
    auto map_itr = kFileExtensionToIjsonconvertibleReadFunction.find(filename_ext);
    if (map_itr == kFileExtensionToIjsonconvertibleReadFunction.end()) {
        LOGW("Read utility::IJsonConvertible failed: unknown file "
             "extension.")
        return false;
    }
    return map_itr->second(filename, object);
}

bool WriteIJsonConvertible(const std::string &filename, const utility::IJsonConvertible &object) {
    std::string filename_ext = utility::filesystem::GetFileExtensionInLowerCase(filename);
    if (filename_ext.empty()) {
        LOGW("Write utility::IJsonConvertible failed: unknown file "
             "extension.")
        return false;
    }
    auto map_itr = kFileExtensionToIjsonconvertibleWriteFunction.find(filename_ext);
    if (map_itr == kFileExtensionToIjsonconvertibleWriteFunction.end()) {
        LOGW("Write utility::IJsonConvertible failed: unknown file "
             "extension.")
        return false;
    }
    return map_itr->second(filename, object);
}

}  // namespace io
}  // namespace vox