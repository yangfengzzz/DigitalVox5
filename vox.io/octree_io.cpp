//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "octree_io.h"

#include <unordered_map>

#include "ijson_convertible_io.h"
#include "vox.base/file_system.h"
#include "vox.base/logging.h"

namespace vox::io {

static const std::unordered_map<std::string, std::function<bool(const std::string &, geometry::Octree &)>>
        kFileExtensionToOctreeReadFunction{
                {"json", ReadOctreeFromJson},
        };

static const std::unordered_map<std::string, std::function<bool(const std::string &, const geometry::Octree &)>>
        kFileExtensionToOctreeWriteFunction{
                {"json", WriteOctreeToJson},
        };

std::shared_ptr<geometry::Octree> CreateOctreeFromFile(const std::string &filename, const std::string &format) {
    auto octree = std::make_shared<geometry::Octree>();
    ReadOctree(filename, *octree);
    return octree;
}

bool ReadOctree(const std::string &filename, geometry::Octree &octree, const std::string &format) {
    std::string filename_ext;
    if (format == "auto") {
        filename_ext = utility::filesystem::GetFileExtensionInLowerCase(filename);
    } else {
        filename_ext = format;
    }
    if (filename_ext.empty()) {
        LOGW("Read geometry::Octree failed: unknown file extension.")
        return false;
    }
    auto map_itr = kFileExtensionToOctreeReadFunction.find(filename_ext);
    if (map_itr == kFileExtensionToOctreeReadFunction.end()) {
        LOGW("Read geometry::Octree failed: unknown file extension.")
        return false;
    }
    bool success = map_itr->second(filename, octree);
    LOGD("Read geometry::Octree.")
    return success;
}

bool WriteOctree(const std::string &filename, const geometry::Octree &octree) {
    std::string filename_ext = utility::filesystem::GetFileExtensionInLowerCase(filename);
    if (filename_ext.empty()) {
        LOGW("Write geometry::Octree failed: unknown file extension.")
        return false;
    }
    auto map_itr = kFileExtensionToOctreeWriteFunction.find(filename_ext);
    if (map_itr == kFileExtensionToOctreeWriteFunction.end()) {
        LOGW("Write geometry::Octree failed: unknown file extension.")
        return false;
    }
    bool success = map_itr->second(filename, octree);
    LOGD("Write geometry::Octree.")
    return success;
}

bool ReadOctreeFromJson(const std::string &filename, geometry::Octree &octree) {
    return ReadIJsonConvertible(filename, octree);
}

bool WriteOctreeToJson(const std::string &filename, const geometry::Octree &octree) {
    return WriteIJsonConvertibleToJSON(filename, octree);
}
}  // namespace vox::io