//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "voxel_grid_io.h"

#include <unordered_map>

#include "vox.base/file_system.h"
#include "vox.base/logging.h"

namespace vox {

namespace {
using namespace io;

const std::unordered_map<std::string, std::function<bool(const std::string &, geometry::VoxelGrid &, bool)>>
        kFileExtensionToVoxelgridReadFunction{
                {"ply", ReadVoxelGridFromPLY},
        };

const std::unordered_map<
        std::string,
        std::function<bool(const std::string &, const geometry::VoxelGrid &, const bool, const bool, const bool)>>
        kFileExtensionToVoxelgridWriteFunction{
                {"ply", WriteVoxelGridToPLY},
        };
}  // unnamed namespace

namespace io {

std::shared_ptr<geometry::VoxelGrid> CreateVoxelGridFromFile(const std::string &filename,
                                                             const std::string &format,
                                                             bool print_progress) {
    auto voxelgrid = std::make_shared<geometry::VoxelGrid>();
    ReadVoxelGrid(filename, *voxelgrid, format, print_progress);
    return voxelgrid;
}

bool ReadVoxelGrid(const std::string &filename,
                   geometry::VoxelGrid &voxelgrid,
                   const std::string &format,
                   bool print_progress) {
    std::string filename_ext;
    if (format == "auto") {
        filename_ext = utility::filesystem::GetFileExtensionInLowerCase(filename);
    } else {
        filename_ext = format;
    }
    if (filename_ext.empty()) {
        LOGW("Read geometry::VoxelGrid failed: unknown file extension.")
        return false;
    }
    auto map_itr = kFileExtensionToVoxelgridReadFunction.find(filename_ext);
    if (map_itr == kFileExtensionToVoxelgridReadFunction.end()) {
        LOGW("Read geometry::VoxelGrid failed: unknown file extension.")
        return false;
    }
    bool success = map_itr->second(filename, voxelgrid, print_progress);
    LOGD("Read geometry::VoxelGrid: {:d} voxels.", (int)voxelgrid.voxels_.size())
    return success;
}

bool WriteVoxelGrid(const std::string &filename,
                    const geometry::VoxelGrid &voxelgrid,
                    bool write_ascii /* = false*/,
                    bool compressed /* = false*/,
                    bool print_progress) {
    std::string filename_ext = utility::filesystem::GetFileExtensionInLowerCase(filename);
    if (filename_ext.empty()) {
        LOGW("Write geometry::VoxelGrid failed: unknown file extension.")
        return false;
    }
    auto map_itr = kFileExtensionToVoxelgridWriteFunction.find(filename_ext);
    if (map_itr == kFileExtensionToVoxelgridWriteFunction.end()) {
        LOGW("Write geometry::VoxelGrid failed: unknown file extension.")
        return false;
    }
    bool success = map_itr->second(filename, voxelgrid, write_ascii, compressed, print_progress);
    LOGD("Write geometry::VoxelGrid: {:d} voxels.", (int)voxelgrid.voxels_.size())
    return success;
}

}  // namespace io
}  // namespace vox
