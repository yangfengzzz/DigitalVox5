//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "line_set_io.h"

#include <unordered_map>

#include "vox.base/file_system.h"
#include "vox.base/logging.h"

namespace vox {

namespace {
using namespace io;

const std::unordered_map<std::string, std::function<bool(const std::string &, geometry::LineSet &, bool)>>
        kFileExtensionToLinesetReadFunction{
                {"ply", ReadLineSetFromPLY},
        };

const std::unordered_map<
        std::string,
        std::function<bool(const std::string &, const geometry::LineSet &, const bool, const bool, const bool)>>
        kFileExtensionToLinesetWriteFunction{
                {"ply", WriteLineSetToPLY},
        };
}  // unnamed namespace

namespace io {

std::shared_ptr<geometry::LineSet> CreateLineSetFromFile(const std::string &filename,
                                                         const std::string &format,
                                                         bool print_progress) {
    auto lineset = std::make_shared<geometry::LineSet>();
    ReadLineSet(filename, *lineset, format, print_progress);
    return lineset;
}

bool ReadLineSet(const std::string &filename,
                 geometry::LineSet &lineset,
                 const std::string &format,
                 bool print_progress) {
    std::string filename_ext;
    if (format == "auto") {
        filename_ext = utility::filesystem::GetFileExtensionInLowerCase(filename);
    } else {
        filename_ext = format;
    }
    if (filename_ext.empty()) {
        LOGW("Read geometry::LineSet failed: unknown file extension.")
        return false;
    }
    auto map_itr = kFileExtensionToLinesetReadFunction.find(filename_ext);
    if (map_itr == kFileExtensionToLinesetReadFunction.end()) {
        LOGW("Read geometry::LineSet failed: unknown file extension.")
        return false;
    }
    bool success = map_itr->second(filename, lineset, print_progress);
    LOGD("Read geometry::LineSet: {:d} vertices.", (int)lineset.points_.size())
    return success;
}

bool WriteLineSet(const std::string &filename,
                  const geometry::LineSet &lineset,
                  bool write_ascii /* = false*/,
                  bool compressed /* = false*/,
                  bool print_progress) {
    std::string filename_ext = utility::filesystem::GetFileExtensionInLowerCase(filename);
    if (filename_ext.empty()) {
        LOGW("Write geometry::LineSet failed: unknown file extension.")
        return false;
    }
    auto map_itr = kFileExtensionToLinesetWriteFunction.find(filename_ext);
    if (map_itr == kFileExtensionToLinesetWriteFunction.end()) {
        LOGW("Write geometry::LineSet failed: unknown file extension.")
        return false;
    }
    bool success = map_itr->second(filename, lineset, write_ascii, compressed, print_progress);
    LOGD("Write geometry::LineSet: {:d} vertices.", (int)lineset.points_.size())
    return success;
}

}  // namespace io
}  // namespace vox
